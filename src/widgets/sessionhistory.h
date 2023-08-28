#pragma once

#include "imgui.h"
#include "implot.h"

#include "constants.h"
#include "f123constants.h"
#include "packets/sessionhistory.h"
#include "storage/data.h"
#include "storage/sessionstorage.h"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace F123;
using namespace SessionStorage;

namespace
{
    static ImGuiTableFlags sTableFlags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

    // TODO: Testing only, use GUI to set
    const std::filesystem::path sStoragePath = "../storage_testing/racedata.xml";
}

class CSessionHistory
{
public:
    CSessionHistory()
    {
        // Axis config
        mAxisFlagsX |= ImPlotAxisFlags_AutoFit;
        mAxisFlagsX |= ImPlotAxisFlags_NoGridLines;
        mAxisFlagsX |= ImPlotAxisFlags_NoTickMarks;

        if (mStorage.SetStoragePath(sStoragePath))
        {
            mRaces = mStorage.LoadRaceData();
            mRacesLoaded = mRaces.size();
        }

        // // For testing
        // SRaceData dummyRace1;
        // dummyRace1.trackName = "TrackName1";
        // SRaceData dummyRace2;
        // dummyRace2.trackName = "TrackName2";

        // SSessionData practiceSession1;
        // practiceSession1.sessionType = ESessionType::P1;
        // practiceSession1.fastestLapNum = 5;

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

    void StoreSessionHistory()
    {
        int nRaces = mRaces.size() - mRacesLoaded;
        if (nRaces > 0)
        {
            if (!mStorage.StoreRaceData(mRaces, nRaces))
            {
                std::cout << "Store failed" << std::endl;
            }
        }
    }

    void SetSessionHistoryData(const SPacketSessionHistoryData &mySessionData)
    {
        if (mRaces.empty() || mRaces.at(mRaces.size() - 1).sessions.empty())
        {
            std::cout << "Getting lap data without a race or session available to update" << std::endl;
            return;
        }

        uint8_t lapNum = mySessionData.numLaps;

        if (lapNum == 0)
        {
            return;
        }

        // Find proper race (last in vector)
        auto raceIdx = mRaces.size() - 1; // week
        auto sessionIdx = mRaces.at(raceIdx).sessions.size() - 1;
        auto currLapIdx = lapNum - 1;
        int lapsStored = mRaces.at(raceIdx).sessions.at(sessionIdx).laps.size();
        if (lapNum > lapsStored)
        {
            for (int i = lapsStored; i < lapNum; ++i)
            {
                SessionStorage::SLap lap;
                lap.lapNumber = i + 1;
                lap.sector1MS = mySessionData.lapHistoryData[i].sector1TimeInMS;
                lap.sector2MS = mySessionData.lapHistoryData[i].sector2TimeInMS;
                lap.sector3MS = mySessionData.lapHistoryData[i].sector3TimeInMS;
                lap.totalLapTime = mySessionData.lapHistoryData[i].lapTimeInMS;
                mRaces.at(raceIdx).sessions.at(sessionIdx).laps.push_back(lap);

                // Sector 3 + Total lap time for last lap
                if (i > 0 && lapNum == lapsStored + 1)
                {
                    mRaces.at(raceIdx).sessions.at(sessionIdx).laps[i - 1].sector3MS = mySessionData.lapHistoryData[i - 1].sector3TimeInMS;
                    mRaces.at(raceIdx).sessions.at(sessionIdx).laps[i - 1].totalLapTime = mySessionData.lapHistoryData[i - 1].lapTimeInMS;
                }
            }
        }

        SessionStorage::SLap lap;
        lap.lapNumber = lapNum;
        lap.sector1MS = mySessionData.lapHistoryData[currLapIdx].sector1TimeInMS;
        lap.sector2MS = mySessionData.lapHistoryData[currLapIdx].sector2TimeInMS;
        lap.sector3MS = mySessionData.lapHistoryData[currLapIdx].sector3TimeInMS;
        lap.totalLapTime = mySessionData.lapHistoryData[currLapIdx].lapTimeInMS;
        mRaces[raceIdx].sessions[sessionIdx].laps[currLapIdx] = lap;

        // Update fastest times
        mRaces[raceIdx].sessions[sessionIdx].fastestLapNum = mySessionData.bestLapTimeLapNum;
        mRaces[raceIdx].sessions[sessionIdx].fastestSec1LapNum = mySessionData.bestSector1LapNum;
        mRaces[raceIdx].sessions[sessionIdx].fastestSec2LapNum = mySessionData.bestSector2LapNum;
        mRaces[raceIdx].sessions[sessionIdx].fastestSec3LapNum = mySessionData.bestSector3LapNum;
    }

    void StartSession(const ETrackId trackId, const ESessionType sessionType)
    {
        mRecordRaces = true;
        auto trackName = sTrackIdToString.at(trackId);

        SessionStorage::SSessionData newSession;
        newSession.sessionType = sessionType;
        // Continuing previous week (may be practice/qual/race)
        if (!mRaces.empty() && mRaces.at(mRaces.size() - 1).trackName == trackName)
        {
            // Push a new session for most recent race
            mRaces.at(mRaces.size() - 1).sessions.push_back(newSession);
        }
        else
        {
            // New race week, new session
            SessionStorage::SRaceData newRace;
            newRace.trackName = trackName;
            newRace.sessions.push_back(newSession);
            mRaces.push_back(newRace);
        }
    }

    void StopSession()
    {
        mRecordRaces = false;
    }

    bool SessionActive()
    {
        return mRecordRaces;
    }

    void ShowSessionHistory(bool *p_open)
    {
        IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
        static int clicked = 0;

        if (ImGui::Button("Save"))
            clicked++;
        if (clicked & 1)
        {
            std::cout << "Save clicked" << std::endl;
            StoreSessionHistory();
            clicked = 0;
        }
        if (ImGui::BeginTable("SessionHistory", 5, sTableFlags))
        {
            ImGui::TableSetupColumn("Track", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Sector 1");
            ImGui::TableSetupColumn("Sector 2");
            ImGui::TableSetupColumn("Sector 3");
            ImGui::TableSetupColumn("Lap Time");
            ImGui::TableHeadersRow();

            if (mRaces.empty())
            {
                ImGui::EndTable();
                return;
            }

            // Show most recent first
            int raceWeek = 0;
            for (auto race = mRaces.end() - 1; race >= mRaces.begin(); --race)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;

                // Auto-open most recent track
                if (raceWeek == 0)
                {
                    flags |= ImGuiTreeNodeFlags_DefaultOpen;
                }

                // Entire race week (Track name)
                bool open = ImGui::TreeNodeEx(race->trackName.c_str(), flags);
                if (open)
                {
                    // Each session within the week, most recent last
                    int sessionNum = 0;
                    for (auto session = race->sessions.begin(); session != race->sessions.end(); ++session)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGuiTreeNodeFlags sessionFlags = ImGuiTreeNodeFlags_SpanFullWidth;

                        // Auto-open most recent session
                        if (sessionNum == race->sessions.size() - 1)
                        {
                            sessionFlags |= ImGuiTreeNodeFlags_DefaultOpen;
                        }
                        bool open = ImGui::TreeNodeEx(sSessionTypeToString.at(session->sessionType).c_str(), sessionFlags);
                        if (open)
                        {
                            // Lap info of session
                            for (auto lap = session->laps.begin(); lap != session->laps.end(); ++lap)
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::Text("L%d", lap->lapNumber);

                                ImGui::TableNextColumn();
                                if (lap->lapNumber == session->fastestSec1LapNum)
                                {
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(green));
                                }
                                ImGui::Text("%.3f", lap->sector1MS / 1000.0f);

                                ImGui::TableNextColumn();
                                if (lap->lapNumber == session->fastestSec2LapNum)
                                {
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(green));
                                }
                                ImGui::Text("%.3f", lap->sector2MS / 1000.0f);

                                ImGui::TableNextColumn();
                                if (lap->lapNumber == session->fastestSec3LapNum)
                                {
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(green));
                                }
                                ImGui::Text("%.3f", lap->sector3MS / 1000.0f);

                                ImGui::TableNextColumn();
                                if (lap->lapNumber == session->fastestLapNum)
                                {
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(green));
                                }
                                auto seconds = lap->totalLapTime / 1000.0f;
                                int min = seconds / 60;
                                ImGui::Text("%d:%06.3f", min, seconds - (min * 60));
                            }
                            ImGui::TreePop();
                        }
                        sessionNum++;
                    }
                    ImGui::TreePop();
                }
                raceWeek++;
            }
            ImGui::EndTable();
        }
    }

private:
    ImPlotAxisFlags mAxisFlagsX{0};
    ImPlotAxisFlags mAxisFlagsY{0};
    // Deltas
    uint8_t mCarRacePosition{1};
    uint32_t mCurrentLapTimeInMS{0};

    // Race storage
    CSessionStorage mStorage;
    uint32_t mRacesLoaded{0}; // Number loaded from storage

    // Local race history
    bool mRecordRaces{false};
    std::vector<SessionStorage::SRaceData> mRaces;
};