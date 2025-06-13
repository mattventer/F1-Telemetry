#pragma once

#include "imgui.h"
#include "implot.h"

#include "constants.h"
#include "f123/constants.h"
#include "f125/constants.h"
#include "f1telemetry.h"

#include "f123/packets/sessionhistory.h"
#include "spdlog/spdlog.h"
#include "storage/data.h"
#include "storage/sessionstorage.h"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

using namespace SessionStorage;

namespace
{
    static ImGuiTableFlags sTableFlags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY;

    // Graphing conversions
    const auto sMsPerSec = 1000.0f;
    const auto sSecPerMin = 60;

    // TODO: Dev only, use GUI to set
    const std::filesystem::path sStoragePath = "D:\\Projects\\F123_Telemetry\\build\\data\\f123history.xml";
}

class CSessionHistory
{
public:
    CSessionHistory()
    {
        SPDLOG_TRACE("CSessionHistory()");
        // Axis config
        mAxisFlagsX |= ImPlotAxisFlags_AutoFit;
        mAxisFlagsX |= ImPlotAxisFlags_NoGridLines;
        mAxisFlagsX |= ImPlotAxisFlags_NoTickMarks;

        if (mStorage.SetStoragePath(sStoragePath))
        {
            mRaces = mStorage.LoadRaceData();
            mRacesLoaded = mRaces.size();
            const std::string infoStr = mRacesLoaded > 1 ? " races" : " race";
            SPDLOG_DEBUG("Loaded {} races", mRacesLoaded);
        }

        // // For testing (hmm...maybe actually write some actual testcases)
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
            // TODO: update: If possible, we store 1 additional race because if the last race was saved part-way through,
            // and the other part of the session is now in mRaces but not storage, we want to include that
            // Skips sessions that already exist
            if (nRaces + 1 <= mRaces.size())
            {
                nRaces++;
            }

            SPDLOG_DEBUG("Storing last {} race(s)", nRaces);

            if (!mStorage.StoreRaceData(mRaces, nRaces))
            {
                SPDLOG_ERROR("Store race data failed!");
            }
        }
    }

    // TODO: Change to generic struct - not packet specific to F1 version
    void SetSessionHistoryData(const F123::SPacketSessionHistoryData &mySessionData)
    {
        // Don't care
        if ((mySessionData.numLaps == 0) || (mySessionData.header.sessionUid == 0))
        {
            return;
        }

        // Find proper race week
        std::tuple<int, int> sessionLocation = FindSession(mySessionData.header.sessionUid);
        int raceIdx = std::get<0>(sessionLocation);
        int sessionIdx = std::get<1>(sessionLocation);

        // Session doesn't exist
        // TODO: Seems I wrote this this expecting the session to already exist? Could just create it here...
        if (sessionIdx < 0)
        {
            SPDLOG_ERROR("Session UID {} doesn't exist but you are trying to store it.", mySessionData.header.sessionUid);
            return;
        }

        uint8_t lapNum = mySessionData.numLaps;                                  // Incoming lap info
        int currLapIdx = lapNum - 1;                                             // Index this lap relates to
        int lapsStored = mRaces.at(raceIdx).sessions.at(sessionIdx).laps.size(); // How many laps have we stored already?

        // Need to add a new lap
        if (lapNum > lapsStored)
        {
            SPDLOG_TRACE("{} got lap {} data", mRaces.at(raceIdx).trackName, lapNum);

            // Store all new laps
            for (int i = lapsStored; i < lapNum; ++i)
            {
                SessionStorage::SLap lap;
                lap.lapNumber = i + 1;
                lap.sector1MS = mySessionData.lapHistoryData[i].sector1TimeInMS;
                lap.sector2MS = mySessionData.lapHistoryData[i].sector2TimeInMS;
                lap.sector3MS = mySessionData.lapHistoryData[i].sector3TimeInMS;
                lap.totalLapTime = mySessionData.lapHistoryData[i].lapTimeInMS;

                if ((lap.sector1MS + lap.sector1MS + lap.sector1MS) > 0)
                {
                    mRaces.at(raceIdx).sessions.at(sessionIdx).laps.push_back(lap);
                }

                // Sector 3 + Total lap time for last lap
                if (i > 0 && lapNum == lapsStored + 1)
                {
                    mRaces.at(raceIdx).sessions.at(sessionIdx).laps.at(i - 1).sector3MS = mySessionData.lapHistoryData[i - 1].sector3TimeInMS;
                    mRaces.at(raceIdx).sessions.at(sessionIdx).laps.at(i - 1).totalLapTime = mySessionData.lapHistoryData[i - 1].lapTimeInMS;
                }
            }
        }
        // Updated times for current lap
        else
        {
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
        }

        // Update fastest times
        mRaces.at(raceIdx).sessions.at(sessionIdx).fastestLapNum = mySessionData.bestLapTimeLapNum;
        mRaces.at(raceIdx).sessions.at(sessionIdx).fastestSec1LapNum = mySessionData.bestSector1LapNum;
        mRaces.at(raceIdx).sessions.at(sessionIdx).fastestSec2LapNum = mySessionData.bestSector2LapNum;
        mRaces.at(raceIdx).sessions.at(sessionIdx).fastestSec3LapNum = mySessionData.bestSector3LapNum;
    }

    // TODO: Make more generic for both 23/25 callers
    void StartSession(const F1::Version version, const uint64_t uid, const int8_t trackId, const uint8_t sessionType)
    {
        std::string trackName{""};
        std::tuple<int, int> sessionLocation;
        int raceIdx = -1, sessionIdx = -1;
        mActiveSessionUid = uid;
        mSessionActive = true;

        trackName = F1::TrackIdToString(version, trackId);
        SPDLOG_DEBUG("Starting session: {} {}", trackName, uid);

        // Find proper race week, if exists
        sessionLocation = FindSession(uid);
        raceIdx = std::get<0>(sessionLocation);
        sessionIdx = std::get<1>(sessionLocation);

        // Already have this session, don't create a new one
        if (sessionIdx > -1)
        {
            SPDLOG_TRACE("Session {} already exists, not creating a new one.", uid);
            return;
        }
        else
        {
            // TODO: Session doesn't exist, but may be continuing a previous race weekend
            // Temporary bandaid (only works if the last stored race is the one resumed (ie not a different career/multiplayer race between))
            if ((mRaces.size() > 0) && (mRaces.at(mRaces.size() - 1).trackName == trackName))
            {
                // New session for last weekend
                SPDLOG_DEBUG("Creating new session for existing weekend at {}", trackName);
                SessionStorage::SSessionData newSession;
                newSession.version = version;
                newSession.uid = uid;
                newSession.sessionType = sessionType;
                mRaces.at(mRaces.size() - 1).sessions.push_back(newSession);
            }
            else
            {
                // New race week, new session
                SPDLOG_DEBUG("Creating new race weekend and session");
                SessionStorage::SRaceWeekend newWeekend;
                newWeekend.trackName = trackName;
                time_t now = time(0);
                newWeekend.date = std::string(ctime(&now));

                SessionStorage::SSessionData newSession;
                newSession.version = version;
                newSession.uid = uid;
                newSession.sessionType = sessionType;
                newWeekend.sessions.push_back(newSession);
                mRaces.push_back(newWeekend);
            }
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
        mSessionActive = false;
        mActiveSessionUid = 0;
    }

    bool SessionActive()
    {
        return mSessionActive;
    }

    uint64_t ActiveSessionUid()
    {
        return mActiveSessionUid;
    }

    void ShowSessionHistory(const ImVec2 spaceAvail)
    {
        IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
        static int clicked = 0;
        if (ImGui::BeginTable("SessionHistory", 5, sTableFlags, ImVec2(spaceAvail.x, (spaceAvail.y - 40)))) // TODO: bad. -40 for save button
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

                        auto sessionTypeStr = F1::SessionTypeToString(session->version, session->sessionType);
                        // Didn't find the string version, just cast to string
                        if (sessionTypeStr.empty())
                        {
                            sessionTypeStr = std::to_string((uint8_t)session->sessionType);
                        }

                        const std::string sessionHeaderStr = sessionTypeStr + " [" + std::to_string(session->uid) + "]";
                        ImGui::PushFont(mSessionHeaderFont);
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
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(F1::green));
                                }
                                ImGui::Text("%.3f", lap->sector1MS / sMsPerSec);

                                ImGui::TableNextColumn();
                                if (lap->lapNumber == session->fastestSec2LapNum)
                                {
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(F1::green));
                                }
                                ImGui::Text("%.3f", lap->sector2MS / sMsPerSec);

                                ImGui::TableNextColumn();
                                if (lap->lapNumber == session->fastestSec3LapNum)
                                {
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(F1::green));
                                }
                                ImGui::Text("%.3f", lap->sector3MS / sMsPerSec);

                                ImGui::TableNextColumn();
                                if (lap->lapNumber == session->fastestLapNum)
                                {
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(F1::green));
                                }
                                auto seconds = lap->totalLapTime / sMsPerSec;
                                int min = seconds / sSecPerMin;
                                ImGui::Text("%d:%06.3f", min, seconds - (min * sSecPerMin));
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
            ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 40);
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
    uint64_t mActiveSessionUid{0};
    bool mSessionActive{false};
    std::vector<SessionStorage::SRaceWeekend> mRaces;

    // Fonts
    ImFont *mTableHeaderFont;
    ImFont *mRaceHeaderFont;
    ImFont *mSessionHeaderFont;
    ImFont *mGeneralTableFont;

    // Return race idx and session idx if session uid exists
    std::tuple<int, int> FindSession(const uint64_t uid)
    {
        if (mRaces.size() > 0)
        {
            int raceIdx = 0, sessionIdx = 0;
            // Search race weekends
            for (auto race : mRaces)
            {
                sessionIdx = 0;
                // Search sessions
                for (auto session : race.sessions)
                {
                    if (session.uid == uid)
                    {
                        return std::tuple(raceIdx, sessionIdx);
                    }
                    sessionIdx++;
                }
                raceIdx++;
            }
        }

        return std::tuple(-1, -1); // Does not exist
    }
};