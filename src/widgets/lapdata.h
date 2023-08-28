#pragma once

#include "imgui.h"
#include "implot.h"

#include "constants.h"
#include "f123constants.h"
#include "packets/lapdata.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using namespace F123;

namespace
{
    constexpr int sMaxDataCount = 10000;
    static float mMaxDelta{0.01f};

    // static ImGuiTableFlags sTableFlags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

    // struct SLap
    // {
    //     uint8_t lapNumber{0};
    //     uint16_t sector1MS{0};
    //     uint16_t sector2MS{0};
    //     uint16_t sector3MS{0};
    //     uint32_t totalLapTime{0};
    // };

    // struct SSessionData
    // {
    //     // std::string trackName;
    //     ESessionType sessionType;
    //     std::vector<SLap> laps;

    //     int fastestSec1LapNum{0};
    //     int fastestSec2LapNum{0};
    //     int fastestSec3LapNum{0};
    //     int fastestLapNum{0};
    // };

    // struct SRaceData
    // {
    //     std::string trackName;
    //     std::vector<SSessionData> sessions;
    // };
}

class CLapData
{
public:
    CLapData()
    {
        // Axis config
        mAxisFlagsX |= ImPlotAxisFlags_AutoFit;
        mAxisFlagsX |= ImPlotAxisFlags_NoGridLines;
        mAxisFlagsX |= ImPlotAxisFlags_NoTickMarks;

        // For testing
        // SRaceData dummyRace1;
        // dummyRace1.trackName = "TrackName1";
        // SRaceData dummyRace2;
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

    void SetLapData(const SLapData &myLapData, const SLapData &carBehindLapData)
    {
        SetDeltaData(myLapData, carBehindLapData);
        // if (mRecordRaces)
        // {
        //     StoreLapData(myLapData);
        // }
    }

    void ResetLapData()
    {
        mDataCount = 0;
        mMaxDelta = 0.01f;
        memset(mXaxis, 0.0f, sizeof(mXaxis));
        memset(mDeltas, 0.0f, sizeof(mDeltas));
    }

    // TODO: magic numbers everywhere
    void ShowDeltas() const
    {
        // ImPlotScale_Time TODO: could probably use this
        auto size = ImGui::GetWindowContentRegionMax();
        if (mCarRacePosition > 1)
        {
            if (ImPlot::BeginPlot("Delta to leader", ImVec2(-1, (size.y / 2) - 30)))
            {
                // Configure
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, mMaxDelta * 1.5f, ImPlotCond_Always);
                ImPlot::SetupAxis(ImAxis_Y1, "Seconds", mAxisFlagsY);
                ImPlot::SetupAxis(ImAxis_X1, nullptr, mAxisFlagsX);
                ImPlot::SetNextLineStyle(red, 3);
                ImPlot::PlotLine("Delta Leader", mXaxis, mDeltas, mDataCount);
                ImPlot::EndPlot();
            }
        }
        else
        {
            if (ImPlot::BeginPlot("Delta to car behind", ImVec2(-1, (size.y / 2) - 30)))
            {
                // Configure
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, mMaxDelta * 1.5f, ImPlotCond_Always);
                ImPlot::SetupAxis(ImAxis_Y1, "Seconds", mAxisFlagsY);
                ImPlot::SetupAxis(ImAxis_X1, nullptr, mAxisFlagsX);
                ImPlot::SetNextLineStyle(green, 3);
                ImPlot::PlotLine("Delta Car behind", mXaxis, mDeltas, mDataCount, (ImPlotLineFlags_SkipNaN));
                ImPlot::EndPlot();
            }
        }
    }

    // void StartRecording(const ETrackId trackId, const ESessionType sessionType)
    // {
    //     mRecordRaces = true;
    //     auto trackName = sTrackIdToString.at(trackId);

    //     SSessionData newSession;
    //     newSession.sessionType = sessionType;
    //     // Continuing previous week (may be practice/qual/race)
    //     if (!mRaces.empty() && mRaces.at(mRaces.size() - 1).trackName == trackName)
    //     {
    //         // Push a new session for most recent race
    //         mRaces.at(mRaces.size() - 1).sessions.push_back(newSession);
    //     }
    //     else
    //     {
    //         // New race week, new session
    //         SRaceData newRace;
    //         newRace.trackName = trackName;
    //         newRace.sessions.push_back(newSession);
    //         mRaces.push_back(newRace);
    //     }
    // }

    // void StopRecording()
    // {
    //     mRecordRaces = false;
    // }

    // bool RecordingActive()
    // {
    //     return mRecordRaces;
    // }

    // void ShowLapHistory(bool *p_open)
    // {
    //     IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
    //     if (ImGui::BeginTable("RaceHistory", 5, sTableFlags))
    //     {
    //         ImGui::TableSetupColumn("Track", ImGuiTableColumnFlags_NoHide);
    //         ImGui::TableSetupColumn("Sector 1");
    //         ImGui::TableSetupColumn("Sector 2");
    //         ImGui::TableSetupColumn("Sector 3");
    //         ImGui::TableSetupColumn("Lap Time");
    //         ImGui::TableHeadersRow();

    //         if (mRaces.empty())
    //         {
    //             ImGui::EndTable();
    //             return;
    //         }

    //         // Show most recent first
    //         int raceWeek = 0;
    //         for (auto race = mRaces.end() - 1; race >= mRaces.begin(); --race)
    //         {
    //             ImGui::TableNextRow();
    //             ImGui::TableNextColumn();
    //             ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;

    //             // Auto-open most recent track
    //             if (raceWeek == 0)
    //             {
    //                 flags |= ImGuiTreeNodeFlags_DefaultOpen;
    //             }

    //             // Entire race week (Track name)
    //             bool open = ImGui::TreeNodeEx(race->trackName.c_str(), flags);
    //             if (open)
    //             {
    //                 // Each session within the week, most recent last
    //                 int sessionNum = 0;
    //                 for (auto session = race->sessions.begin(); session != race->sessions.end(); ++session)
    //                 {
    //                     ImGui::TableNextRow();
    //                     ImGui::TableNextColumn();
    //                     ImGuiTreeNodeFlags sessionFlags = ImGuiTreeNodeFlags_SpanFullWidth;

    //                     // Auto-open most recent session
    //                     if (sessionNum == race->sessions.size() - 1)
    //                     {
    //                         sessionFlags |= ImGuiTreeNodeFlags_DefaultOpen;
    //                     }
    //                     bool open = ImGui::TreeNodeEx(sSessionTypeToString.at(session->sessionType).c_str(), sessionFlags);
    //                     if (open)
    //                     {
    //                         // Lap info of session
    //                         for (auto lap = session->laps.begin(); lap != session->laps.end(); ++lap)
    //                         {
    //                             ImGui::TableNextRow();
    //                             ImGui::TableNextColumn();
    //                             ImGui::Text("L%d", lap->lapNumber);

    //                             ImGui::TableNextColumn();
    //                             if (lap->lapNumber == session->fastestSec1LapNum)
    //                             {
    //                                 ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(green));
    //                             }
    //                             ImGui::Text("%.3f", lap->sector1MS / 1000.0f);

    //                             ImGui::TableNextColumn();
    //                             if (lap->lapNumber == session->fastestSec2LapNum)
    //                             {
    //                                 ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(green));
    //                             }
    //                             ImGui::Text("%.3f", lap->sector2MS / 1000.0f);

    //                             ImGui::TableNextColumn();
    //                             if (lap->lapNumber == session->fastestSec3LapNum)
    //                             {
    //                                 ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(green));
    //                             }
    //                             ImGui::Text("%.3f", lap->sector3MS / 1000.0f);

    //                             ImGui::TableNextColumn();
    //                             if (lap->lapNumber == session->fastestLapNum)
    //                             {
    //                                 ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(green));
    //                             }
    //                             auto seconds = lap->totalLapTime / 1000.0f;
    //                             int min = seconds / 60;
    //                             ImGui::Text("%d:%06.3f", min, seconds - (min * 60));
    //                         }
    //                         ImGui::TreePop();
    //                     }
    //                     sessionNum++;
    //                 }
    //                 ImGui::TreePop();
    //             }
    //             raceWeek++;
    //         }
    //         ImGui::EndTable();
    //     }
    // }

private:
    ImPlotAxisFlags mAxisFlagsX{0};
    ImPlotAxisFlags mAxisFlagsY{0};
    // Deltas
    uint8_t mCarRacePosition{1};
    uint32_t mCurrentLapTimeInMS{0};
    float mDeltas[sMaxDataCount]{};
    float mXaxis[sMaxDataCount]{};
    int mDataCount{0};

    // Race history
    // bool mRecordRaces{false};
    // std::vector<SRaceData> mRaces;

    // void StoreLapData(const SLapData &myLapData)
    // {
    //     if (mRaces.empty() || mRaces.at(mRaces.size() - 1).sessions.empty())
    //     {
    //         std::cout << "Getting lap data without a race or session available to update" << std::endl;
    //         return;
    //     }

    //     // Find proper race (last in vector)
    //     auto raceIdx = mRaces.size() - 1; // week
    //     auto sessionIdx = mRaces.at(raceIdx).sessions.size() - 1;
    //     auto lapNum = myLapData.currentLapNum;
    //     auto lapIdx = lapNum - 1;

    //     // Started recording mid-race, fill with empty laps
    //     if (mRaces.at(raceIdx).sessions.at(sessionIdx).laps.empty())
    //     {
    //         int i = 0;
    //         SLap emptyLap;
    //         while (i < lapNum)
    //         {
    //             emptyLap.lapNumber = i + 1;
    //             mRaces[raceIdx].sessions[sessionIdx].laps.push_back(emptyLap);
    //             i++;
    //         }
    //     }

    //     // Last lap time
    //     if (myLapData.lastLapTimeInMS > 0 && mRaces.at(raceIdx).sessions.at(sessionIdx).laps.at(lapIdx - 1).totalLapTime == 0)
    //     {
    //         mRaces[raceIdx].sessions[sessionIdx].laps[lapIdx - 1].totalLapTime = myLapData.lastLapTimeInMS;

    //         // Calculate sector 3 of last lap
    //         // Don't calculate this if missing previous data
    //         if (mRaces[raceIdx].sessions[sessionIdx].laps[lapIdx - 1].sector1MS != 0.0f && mRaces[raceIdx].sessions[sessionIdx].laps[lapIdx - 1].sector2MS != 0.0f)
    //         {
    //             auto sector3TimeMS = myLapData.lastLapTimeInMS - mRaces[raceIdx].sessions[sessionIdx].laps[lapIdx - 1].sector1MS - mRaces[raceIdx].sessions[sessionIdx].laps[lapIdx - 1].sector2MS;
    //             mRaces[raceIdx].sessions[sessionIdx].laps[lapIdx - 1].sector3MS = sector3TimeMS;
    //             if (mRaces[raceIdx].sessions[sessionIdx].fastestSec3LapNum == 0 || sector3TimeMS < mRaces[raceIdx].sessions[sessionIdx].laps[mRaces[raceIdx].sessions[sessionIdx].fastestSec3LapNum - 1].sector3MS)
    //             {
    //                 mRaces[raceIdx].sessions[sessionIdx].fastestSec3LapNum = lapNum - 1; // Fastest sector 3 time
    //             }
    //         }

    //         if (mRaces[raceIdx].sessions[sessionIdx].fastestLapNum == 0 || myLapData.lastLapTimeInMS < mRaces[raceIdx].sessions[sessionIdx].laps[mRaces[raceIdx].sessions[sessionIdx].fastestLapNum - 1].totalLapTime)
    //         {
    //             mRaces[raceIdx].sessions[sessionIdx].fastestLapNum = lapNum - 1; // Fastest lap was the last one
    //         }
    //     }

    //     // New lap
    //     if (lapNum > mRaces.at(raceIdx).sessions.at(sessionIdx).laps.size())
    //     {
    //         SLap newLap;
    //         newLap.lapNumber = lapNum;
    //         mRaces[raceIdx].sessions[sessionIdx].laps.push_back(newLap);
    //     }
    //     else
    //     {
    //         // Find proper lap within race
    //         if (myLapData.sector1TimeInMS > 0)
    //         {
    //             mRaces[raceIdx].sessions[sessionIdx].laps[lapIdx].sector1MS = myLapData.sector1TimeInMS;

    //             if (mRaces[raceIdx].sessions[sessionIdx].fastestSec1LapNum == 0 || myLapData.sector1TimeInMS < mRaces[raceIdx].sessions[sessionIdx].laps[mRaces[raceIdx].sessions[sessionIdx].fastestSec1LapNum - 1].sector1MS)
    //             {
    //                 mRaces[raceIdx].sessions[sessionIdx].fastestSec1LapNum = lapNum; // Fastest sector 1 time
    //             }
    //         }
    //         if (myLapData.sector2TimeInMS > 0)
    //         {
    //             mRaces[raceIdx].sessions[sessionIdx].laps[lapIdx].sector2MS = myLapData.sector2TimeInMS;

    //             if (mRaces[raceIdx].sessions[sessionIdx].fastestSec2LapNum == 0 || myLapData.sector2TimeInMS < mRaces[raceIdx].sessions[sessionIdx].laps[mRaces[raceIdx].sessions[sessionIdx].fastestSec2LapNum - 1].sector2MS)
    //             {
    //                 mRaces[raceIdx].sessions[sessionIdx].fastestSec2LapNum = lapNum; // Fastest sector 2 time
    //             }
    //         }
    //     }
    // }

    void SetDeltaData(const SLapData &myLapData, const SLapData &carBehindLapData)
    {
        const auto currPosition = myLapData.carPosition;
        const auto currLapTime = myLapData.currentLapTimeInMS;

        // Took or lost leader position, new lap, max data reached
        if ((currPosition == 1 && mCarRacePosition > 1) || (mCarRacePosition == 1 && currPosition > 1) || (mDataCount >= sMaxDataCount) || (currLapTime < mCurrentLapTimeInMS))
        {
            ResetLapData();
        }
        mCarRacePosition = currPosition;
        mCurrentLapTimeInMS = currLapTime;

        uint16_t deltaMS;
        if (mCarRacePosition > 1)
        {
            deltaMS = myLapData.deltaToRaceLeaderInMS;
        }
        else
        {
            deltaMS = carBehindLapData.deltaToCarInFrontInMS;
        }

        float deltaSecs = deltaMS / 1000.0f;
        float currLapTimeSecs = currLapTime / 1000.0f;

        // Outlier when car behind gets overtaken
        if (mDataCount != 0 && deltaSecs > 30 && (deltaSecs > mDeltas[mDataCount - 1] * 10))
        {
            return;
        }

        mDeltas[mDataCount] = deltaSecs;
        mXaxis[mDataCount] = currLapTimeSecs;

        if (deltaSecs > mMaxDelta)
        {
            mMaxDelta = deltaSecs;
        }
        mDataCount++;
    }
};