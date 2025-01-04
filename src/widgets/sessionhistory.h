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
#include <memory>
#include <string>
#include <vector>

using namespace F123;
using namespace SessionStorage;

namespace
{
    static ImGuiTableFlags sTableFlags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

    // TODO: Dev only, use GUI to set
    const std::filesystem::path sStoragePath = "D:\\Projects\\F123_Telemetry\\build\\data\\f123history.xml";
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
            const std::string infoStr = mRacesLoaded > 1 ? " races" : " race";
            std::cout << "Loaded " << mRacesLoaded << infoStr << std::endl;
        }

        // // For testing
        // SRaceWeekend dummyRace1;
        // dummyRace1.trackName = "TrackName1";
        // dummyRace1.date = "dummy date1";
        // SRaceWeekend dummyRace2;
        // dummyRace2.trackName = "TrackName2";
        // dummyRace2.date = "dummy date2";

        // SSessionData practiceSession1;
        // practiceSession1.uid = 12345;
        // practiceSession1.sessionType = ESessionType::P1;
        // practiceSession1.fastestLapNum = 5;

        // SSessionData practiceSession2;
        // practiceSession2.uid = 67890;
        // practiceSession2.sessionType = ESessionType::P2;

        // SSessionData raceSession;
        // raceSession.uid = 98765;
        // raceSession.sessionType = ESessionType::R;

        // for (int i = 1; i < 10; ++i)
        // {
        //     SLap newLap;
        //     newLap.lapNumber = i;
        //     newLap.sector1MS = i * 10980;
        //     newLap.sector2MS = i * 11650;
        //     newLap.sector3MS = i * 12650;
        //     newLap.totalLapTime = i * 15255;

        //     practiceSession1.laps.push_back(newLap);
        //     practiceSession2.laps.push_back(newLap);

        //     raceSession.laps.push_back(newLap);
        // }

        // practiceSession1.fastestLapNum = 7;
        // practiceSession1.fastestSec1LapNum = 2;
        // practiceSession1.fastestSec2LapNum = 7;
        // practiceSession1.fastestSec3LapNum = 5;
        // practiceSession2.fastestLapNum = 1;
        // practiceSession2.fastestSec1LapNum = 2;
        // practiceSession2.fastestSec2LapNum = 7;
        // practiceSession2.fastestSec3LapNum = 5;
        // raceSession.fastestLapNum = 4;

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
        // Only want to store new races
        int nRaces = mRaces.size() - mRacesLoaded;

        if (mRaces.size() > 0)
        {
            // TODO: Temporary fix for when a race was save partway through
            if (nRaces + 1 <= mRaces.size())
            {
                nRaces++;
            }

            std::cout << "Storing last " << nRaces << " race(s)" << std::endl;

            if (!mStorage.StoreRaceData(mRaces, nRaces))
            {
                std::cout << "Store failed" << std::endl;
            }
        }
        else
        {
            std::cout << "No new races to store" << std::endl;
        }
    }

    void SetSessionHistoryData(const SPacketSessionHistoryData &mySessionData)
    {

        if (mRaces.empty() || mRaces.at(mRaces.size() - 1).sessions.empty())
        {
            // std::cout << "Getting lap data without a race or session available to update" << std::endl;
            return;
        }

        // Don't care
        if (mySessionData.numLaps == 0)
        {
            return;
        }

        // Find proper race (last in vector)
        auto raceIdx = mRaces.size() - 1; // week

        // TODO: Check the session uid to make sure
        auto sessionIdx = mRaces.at(raceIdx).sessions.size() - 1;

        uint8_t lapNum = mySessionData.numLaps;
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

        // TODO: Figure out why I'm doing this again
        SessionStorage::SLap lap;
        lap.lapNumber = lapNum;
        lap.sector1MS = mySessionData.lapHistoryData[currLapIdx].sector1TimeInMS;
        lap.sector2MS = mySessionData.lapHistoryData[currLapIdx].sector2TimeInMS;
        lap.sector3MS = mySessionData.lapHistoryData[currLapIdx].sector3TimeInMS;
        lap.totalLapTime = mySessionData.lapHistoryData[currLapIdx].lapTimeInMS;

        // Crossing the finish line starts a new "lap", but times are all 0. Ignore this
        if (lap.sector1MS + lap.sector2MS + lap.sector3MS > 0)
        {
            mRaces[raceIdx].sessions[sessionIdx].laps[currLapIdx] = lap;
        }

        // Update fastest times
        mRaces[raceIdx].sessions[sessionIdx].fastestLapNum = mySessionData.bestLapTimeLapNum;
        mRaces[raceIdx].sessions[sessionIdx].fastestSec1LapNum = mySessionData.bestSector1LapNum;
        mRaces[raceIdx].sessions[sessionIdx].fastestSec2LapNum = mySessionData.bestSector2LapNum;
        mRaces[raceIdx].sessions[sessionIdx].fastestSec3LapNum = mySessionData.bestSector3LapNum;
    }

    void StartSession(const uint64_t uid, const ETrackId trackId, const ESessionType sessionType)
    {
        std::cout << "Starting session: " << uid << std::endl;

        mRecord = true;
        auto trackName = sTrackIdToString.at(trackId);

        SessionStorage::SSessionData newSession;
        newSession.uid = uid;
        newSession.sessionType = sessionType;
        // Continuing previous week (may be practice/qual/race)
        if (!mRaces.empty() && mRaces.at(mRaces.size() - 1).trackName == trackName)
        {
            auto sessions = mRaces.at(mRaces.size() - 1).sessions;
            // Push a new session if one doesn't already exist
            if (sessions.size() < 1 || sessions.at(sessions.size() - 1).uid != uid)
            {
                mRaces[mRaces.size() - 1].sessions.push_back(newSession);
            }
        }
        else
        {
            // New race week, new session
            SessionStorage::SRaceWeekend newWeekend;
            newWeekend.trackName = trackName;
            time_t now = time(0);
            newWeekend.date = std::string(ctime(&now));
            newWeekend.sessions.push_back(newSession);
            mRaces.push_back(newWeekend);
        }
    }

    void SetFonts(ImFont *tableHeader, ImFont *raceHeader, ImFont *sessionHeader, ImFont *general)
    {
        mTableHeaderFont = tableHeader;
        mRaceHeaderFont = raceHeader;
        mSessionHeaderFont = sessionHeader;
        mGeneralTableFont = general;
    }

    void StopSession()
    {
        mRecord = false;
    }

    bool SessionActive()
    {
        return mRecord;
    }

    void ShowSessionHistory(bool *p_open)
    {
        IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
        static int clicked = 0;
        if (ImGui::BeginTable("SessionHistory", 5, sTableFlags))
        {
            ImGui::PushFont(mTableHeaderFont);
            ImGui::TableSetupColumn("Track", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Sector 1");
            ImGui::TableSetupColumn("Sector 2");
            ImGui::TableSetupColumn("Sector 3");
            ImGui::TableSetupColumn("Lap Time");
            ImGui::TableHeadersRow();

            ImGui::PopFont();

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

                // Entire race week (Track name + date)
                ImGui::PushFont(mRaceHeaderFont);
                std::string raceWeekLabel = race->trackName + " " + race->date;
                bool open = ImGui::TreeNodeEx(raceWeekLabel.c_str(), flags);
                ImGui::PopFont();
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

                        ImGui::PushFont(mSessionHeaderFont);
                        const std::string sessionHeaderStr = sSessionTypeToString.at(session->sessionType) + " [" + std::to_string(session->uid) + "]";
                        bool open = ImGui::TreeNodeEx(sessionHeaderStr.c_str(), sessionFlags);
                        ImGui::PopFont();
                        if (open)
                        {
                            // Lap info of session
                            for (auto lap = session->laps.begin(); lap != session->laps.end(); ++lap)
                            {
                                ImGui::PushFont(mGeneralTableFont);

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
                                ImGui::PopFont();
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
            ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 35);
            if (ImGui::Button("Save"))
            {
                clicked++;
            }
            if (clicked & 1)
            {
                StoreSessionHistory();
                clicked = 0;
            }
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
    bool mRecord{false};
    std::vector<SessionStorage::SRaceWeekend> mRaces;

    // Fonts
    ImFont *mTableHeaderFont;
    ImFont *mRaceHeaderFont;
    ImFont *mSessionHeaderFont;
    ImFont *mGeneralTableFont;
};