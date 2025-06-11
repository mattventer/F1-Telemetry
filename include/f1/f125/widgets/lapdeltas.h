#pragma once

#include "imgui.h"
#include "implot.h"

#include "f1telemetry.h"
#include "constants.h"
#include "f125/constants.h"
#include "f125/packets/lapdata.h"
#include "spdlog/spdlog.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace F125;

class CLapDeltas25 : public ILapDeltas
{
public:
    CLapDeltas25()
    {
        SPDLOG_TRACE("CLapDeltas25()");

        // Axis config
        mAxisFlagsX |= ImPlotAxisFlags_AutoFit;
        mAxisFlagsX |= ImPlotAxisFlags_NoGridLines;
        mAxisFlagsX |= ImPlotAxisFlags_NoTickMarks;
        mAxisFlagsX |= ImPlotAxisFlags_NoMenus;
        mAxisFlagsX |= ImPlotCond_Always;

        mAxisFlagsY |= ImPlotAxisFlags_NoMenus;
        mAxisFlagsY |= ImPlotCond_Always;
        // For testing
        // SRaceWeekend dummyRace1;
        // dummyRace1.trackName = "TrackName1";
        // SRaceWeekend dummyRace2;
        // dummyRace2.trackName = "TrackName2";

        // SSessionData practiceSession1;
        // practiceSession1.sessionType = ESessionType::P1;

        // SSessionData practiceSession2;
        // practiceSession2.sessionType = ESessionType::P2;

        // SSessionData raceSession;
        // raceSession.sessionType = ESessionType::R;

        // for (int i = 1; i < 10; ++i)
        // {
        //     SLap newLap;
        //     newLap.lapNumber = i;
        //     newLap.sector1MS = i * 10000;
        //     newLap.sector2MS = i * 11000;
        //     newLap.totalLapTime = i * 20000;

        //     practiceSession1.laps.push_back(newLap);
        //     practiceSession1.fastestLapNum = 1;
        //     practiceSession2.laps.push_back(newLap);
        //     practiceSession2.fastestLapNum = 1;

        //     raceSession.laps.push_back(newLap);
        //     raceSession.fastestLapNum = 1;
        // }

        // dummyRace1.sessions.push_back(practiceSession1);
        // dummyRace1.sessions.push_back(practiceSession2);
        // dummyRace1.sessions.push_back(raceSession);
        // dummyRace2.sessions.push_back(practiceSession1);
        // dummyRace2.sessions.push_back(practiceSession2);
        // dummyRace2.sessions.push_back(raceSession);

        // mRaces.push_back(dummyRace1);
        // mRaces.push_back(dummyRace2);
    }

    void SetLapData(const SLapDeltasData &myLapData, const SLapDeltasData &carBehindLapData) override
    {
        SetDeltaData(myLapData, carBehindLapData);
    }

    void ResetLapData() override
    {
        mDataCount = 0;
        mMaxDelta = 0.01f;
        mMinDelta = FLT_MAX;
        memset(mXaxis, 0.0f, sizeof(mXaxis));
        memset(mDeltaLeader, 0.0f, sizeof(mDeltaLeader));
        memset(mDeltaFront, 0.0f, sizeof(mDeltaLeader));
        memset(mDeltaBehind, 0.0f, sizeof(mDeltaLeader));
    }

    void ShowDeltas(const ImVec2 spaceAvail) const override
    {
        if (ImPlot::BeginPlot("Deltas (s)", ImVec2((spaceAvail.x / 2), (spaceAvail.y / 2))))
        {
            // Configure plot size
            ImPlot::SetupAxisLimits(ImAxis_Y1, ((mMinDelta == FLT_MAX) ? 0.0f : mMinDelta / 2.0f), (mMaxDelta * 1.50f), ImPlotCond_Always);
            ImPlot::SetupAxis(ImAxis_Y1, "Seconds", mAxisFlagsY);
            ImPlot::SetupAxis(ImAxis_X1, nullptr, mAxisFlagsX);
            ImPlot::SetNextLineStyle(F1::yellow, 3);
            ImPlot::PlotLine("Leader", mXaxis, mDeltaLeader, mDataCount);
            ImPlot::SetNextLineStyle(F1::red, 3);
            ImPlot::PlotLine("Ahead", mXaxis, mDeltaFront, mDataCount);
            ImPlot::SetNextLineStyle(F1::green, 3);
            ImPlot::PlotLine("Behind", mXaxis, mDeltaBehind, mDataCount);
            ImPlot::EndPlot();
        }
    }

private:
    void SetDeltaData(const SLapDeltasData &myLapData, const SLapDeltasData &carBehindLapData)
    {
        uint16_t deltaLeaderMS = 0, deltaFrontMs = 0, deltaBehindMs = 0;
        float deltaLeaderSec = 0, deltaFrontSec = 0, deltaBehindSec = 0;

        const auto playerPosition = myLapData.carPosition;
        const auto playerLapTime = myLapData.currentLapTimeInMS;

        // New lap, or max data reached
        if ((mDataCount >= mMaxDataCount) || (playerLapTime < mCurrentLapTimeInMS))
        {
            ResetLapData();
        }
        mPlayerPosition = playerPosition;
        mCurrentLapTimeInMS = playerLapTime;

        // Hoping these are 0 when player is the leader
        deltaLeaderMS = myLapData.deltaToRaceLeaderInMS;
        deltaFrontMs = myLapData.deltaToCarInFrontInMS;

        // Is there a car behind the player?
        if (carBehindLapData.carPosition > 1)
        {
            deltaBehindMs = carBehindLapData.deltaToCarInFrontInMS;
        }

        deltaLeaderSec = deltaLeaderMS / 1000.0f;
        deltaFrontSec = deltaFrontMs / 1000.0f;
        deltaBehindSec = deltaBehindMs / 1000.0f;

        // Convert to seconds
        // TODO: Getting some outliers when passing or getting passed
        mDeltaLeader[mDataCount] = deltaLeaderSec;
        mDeltaFront[mDataCount] = deltaFrontSec;
        mDeltaBehind[mDataCount] = deltaBehindSec;
        mXaxis[mDataCount] = playerLapTime / 1000.0f;

        // Chart Y-axis max
        if (deltaLeaderSec > mMaxDelta)
        {
            mMaxDelta = deltaLeaderSec;
        }
        if (deltaFrontSec > mMaxDelta)
        {
            mMaxDelta = deltaFrontSec;
        }
        if (deltaBehindSec > mMaxDelta)
        {
            mMaxDelta = deltaBehindSec;
        }

        // Chart Y-axis min
        if (deltaLeaderSec < mMinDelta)
        {
            mMinDelta = deltaLeaderSec;
        }
        if (deltaFrontSec < mMinDelta)
        {
            mMinDelta = deltaFrontSec;
        }
        if (deltaBehindSec < mMinDelta)
        {
            mMinDelta = deltaBehindSec;
        }
        mDataCount++;
    }
};