#pragma once

#include "imgui.h"
#include "implot.h"

#include "header.h"
#include "spdlog/spdlog.h"
#include "windows/sessionhistory.h"

#include <memory>
#include <array>
#include <cstdint>

// Car damage graph data structur
struct SCarDamageGraphData
{
    // F123
    uint8_t engineDamage;
    uint8_t gearBoxDamage;
    uint8_t engineMGUHWear;
    uint8_t engineESWear;
    uint8_t engineCEWear;
    uint8_t engineICEWear;
    uint8_t engineMGUKWear;
    uint8_t engineTCWear;
};

// Base classes for all visuals
class ICarDamageGraph
{
public:
    ICarDamageGraph() {}

    virtual void ResetCarDamage() = 0;
    virtual void SetCarDamage(const SCarDamageGraphData &carDamageData) = 0;
    virtual void ShowGraph(const ImVec2 spaceAvail) const = 0;

protected:
    ImPlotAxisFlags mAxisFlagsX{0};
    ImPlotAxisFlags mAxisFlagsY{0};
    uint8_t mDataPoints{8};

    uint8_t mCarDamageData[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    const char *mLabels[8] = {"Engine", "Gearbox", "MGUH", "ES", "CE", "ICE", "MGUK", "Turbo"};
};

class ISessionInfo
{
public:
    ISessionInfo() {}

    virtual void ShowSessionStatus() const = 0;
    virtual void SetFont(ImFont *font) = 0;
    virtual void SessionStarted() = 0;
    virtual void SessionStopped() = 0;
    virtual void NewPacket(const uint16_t year) = 0;
    // TODO: include ip
    virtual void SetSocketInfo(const int port) = 0;

protected:
    bool mSessionActive{false};
    std::string mLastPacketTime{"Never"};
    uint16_t mPacketVersion{0};
    int mPort{-1};

    // Style
    ImFont *mFont;
};

class ITyreTemps
{
public:
    ITyreTemps() {}

    virtual void SetTyreCompound(uint8_t actual, uint8_t visual) = 0;
    virtual void SetTyreInnerTemps(const std::array<uint8_t, 4> tyreTempData) = 0;
    virtual void ShowTyreInnerTemps(const ImVec2 spaceAvail) const = 0;

protected:
    ImPlotAxisFlags mAxisFlagsX{0};
    ImPlotAxisFlags mAxisFlagsY{0};
    std::string mActualTyreCompoundStr{""};
    std::string mVisualTyreCompoundStr{""};

    uint8_t mTyreInnerTemps[4] = {0, 0, 0, 0};
    const char *mLabels[4] = {"RL", "RR", "FL", "FR"};
};

class ITyreWearGraph
{
public:
    ITyreWearGraph() {}

    virtual void SetTyreWear(const std::array<float, 4> tyreWearData) = 0;
    virtual void ResetTyreWear() = 0;
    virtual void ShowGraph(const ImVec2 spaceAvail) const = 0;

protected:
    ImPlotAxisFlags mAxisFlagsX{0};
    ImPlotAxisFlags mAxisFlagsY{0};

    float mTyreWearData[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    const char *mLabels[4] = {"RL", "RR", "FL", "FR"};
};

class ILapInfoHeader
{
public:
    ILapInfoHeader() {}

    virtual void SetPitLapWindow(const uint8_t min, const uint8_t max, const uint8_t rejoinPos) = 0;
    virtual void SetCurrentLap(const uint8_t currLap) = 0;
    virtual uint8_t CurrentLap() = 0;
    virtual float ShowLapInfoHeader() const = 0;

protected:
    uint8_t mPitLapMax{0};
    uint8_t mPitLapMin{0};
    uint8_t mPitRejoinPos{0};
    uint8_t mCurrentLap{0};
};

struct SLapDeltasData
{
    uint8_t carPosition; // Car race position
    uint16_t deltaToCarInFrontInMS;
    uint16_t deltaToRaceLeaderInMS;
    uint32_t currentLapTimeInMS;
};

class ILapDeltas
{
public:
    ILapDeltas() {}

    virtual void SetLapData(const SLapDeltasData &myLapData, const SLapDeltasData &carBehindLapData) = 0;
    virtual void ResetLapData() = 0;
    virtual void ShowDeltas(const ImVec2 spaceAvail) const = 0;

protected:
    ImPlotAxisFlags mAxisFlagsX{0};
    ImPlotAxisFlags mAxisFlagsY{0};
    // Deltas
    uint8_t mPlayerPosition{1};
    uint32_t mCurrentLapTimeInMS{0};

    // Deltas (sec)
    float mDeltaLeader[10000]{0}; // Delta to race leader
    float mDeltaFront[10000]{0};  // Delta to car ahead of player
    float mDeltaBehind[10000]{0}; // Delta to car behin player

    float mXaxis[10000]{};
    int mDataCount{0};
    int mMaxDataCount{10000};

    float mMaxDelta{0.01f};
    float mMinDelta{FLT_MAX};
};

struct SF1TelemetryResources
{
    // Windows
    std::shared_ptr<CSessionHistory> sessionHistory;

    // Widgets
    std::shared_ptr<ISessionInfo> sessionInfo;
    std::shared_ptr<ILapInfoHeader> lapInfoHeader;

    // Graphs
    std::shared_ptr<ICarDamageGraph> carDamageGraph;
    std::shared_ptr<ITyreTemps> tyreTemps;
    std::shared_ptr<ITyreWearGraph> tyreWearGraph;
    std::shared_ptr<ILapDeltas> lapDeltas;
};

class CF1Telemetry
{
public:
    uint16_t mYear;

    CF1Telemetry(const uint16_t year, const SF1TelemetryResources &rsrcs)
        : mYear(year)
    {
        SPDLOG_TRACE("CF1Telemetry() {}", year);
        mSessionInfo = rsrcs.sessionInfo;
        mCarDamageGraph = rsrcs.carDamageGraph;
        mSessionHistory = rsrcs.sessionHistory;
        mTyreTemps = rsrcs.tyreTemps;
        mTyreWearGraph = rsrcs.tyreWearGraph;
        mLapDeltas = rsrcs.lapDeltas;
        mLapInfoHeader = rsrcs.lapInfoHeader;
    }

    void ParsePacket(char *packet, const SPacketHeader &header);

protected:
    std::shared_ptr<ISessionInfo> mSessionInfo;
    std::shared_ptr<ICarDamageGraph> mCarDamageGraph;
    std::shared_ptr<CSessionHistory> mSessionHistory;
    std::shared_ptr<ITyreTemps> mTyreTemps;
    std::shared_ptr<ITyreWearGraph> mTyreWearGraph;
    std::shared_ptr<ILapDeltas> mLapDeltas;
    std::shared_ptr<ILapInfoHeader> mLapInfoHeader;
};