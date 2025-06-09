#pragma once

#include "imgui.h"
#include "implot.h"

#include "constants.h"
#include "f123/constants.h"
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

using namespace F123;
using namespace SessionStorage;

namespace
{
    static ImGuiTableFlags sTableFlags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

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
        SPDLOG_TRACE("StoreSessionHistory() entry");

        // Only want to store new races
        int nRaces = mRaces.size() - mRacesLoaded;

        if (mRaces.size() > 0)
        {
            // TODO: Temporary fix for when a race was save partway through
            if (nRaces + 1 <= mRaces.size())
            {
                nRaces++;
            }

            SPDLOG_DEBUG("Storing last {} race(s)", nRaces);

            if (!mStorage.StoreRaceData(mRaces, nRaces))
            {
                SPDLOG_ERROR("Store failed!");
            }
        }
        else
        {
            SPDLOG_DEBUG("No new races to store");
        }
    }

    // TODO: Change to generic struct - not packet
    void SetSessionHistoryData(const SPacketSessionHistoryData &mySessionData)
    {
        SPDLOG_TRACE("SetSessionHistoryData() entry");
        // Don't care
        if ((mySessionData.numLaps == 0) || (mySessionData.header.sessionUid == 0))
        {
            return;
        }

        // Find proper race week
        std::tuple<int, int> location = FindSession(mySessionData.header.sessionUid);
        auto raceIdx = std::get<0>(location);
        auto sessionIdx = std::get<1>(location);
        SPDLOG_TRACE("New session data: uid {}, raceIdx {}, sessionIdx {}", mySessionData.header.sessionUid, raceIdx, sessionIdx);

        // Session doesn't exist, create a new one
        if (sessionIdx < 0)
        {
            SPDLOG_ERROR("Session UID {} doesn't exist but got SPacketSessionHistoryData for it", mySessionData.header.sessionUid);
            return;
        }

        // No lap data yet
        if (mySessionData.numLaps == 0)
        {
            SPDLOG_TRACE("Lap #{}, ignoring", mySessionData.numLaps);
            return;
        }

        uint8_t lapNum = mySessionData.numLaps;                                  // Incoming lap info
        auto currLapIdx = lapNum - 1;                                            // Index this lap relates to
        int lapsStored = mRaces.at(raceIdx).sessions.at(sessionIdx).laps.size(); // How many laps have we stored already?
        SPDLOG_TRACE("Got lap #{}. Last lap received #{}", lapNum, lapsStored);

        // Need to add a new lap
        if (lapNum > lapsStored)
        {
            SPDLOG_TRACE("{} {} new laps to store", mRaces.at(raceIdx).trackName, (lapNum - lapsStored));

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
                    SPDLOG_TRACE("Pushed new lap {} on track {}", lap.lapNumber, mRaces.at(raceIdx).trackName);
                }

                // Sector 3 + Total lap time for last lap
                if (i > 0 && lapNum == lapsStored + 1)
                {
                    SPDLOG_TRACE("Sector 3 + Total lap time for last lap");
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
            else
            {
                // TODO: remove
                SPDLOG_TRACE("Lap #{} data but all 0's", lapNum);
            }
        }

        // Update fastest times
        mRaces.at(raceIdx).sessions.at(sessionIdx).fastestLapNum = mySessionData.bestLapTimeLapNum;
        mRaces.at(raceIdx).sessions.at(sessionIdx).fastestSec1LapNum = mySessionData.bestSector1LapNum;
        mRaces.at(raceIdx).sessions.at(sessionIdx).fastestSec2LapNum = mySessionData.bestSector2LapNum;
        mRaces.at(raceIdx).sessions.at(sessionIdx).fastestSec3LapNum = mySessionData.bestSector3LapNum;
    }

    void StartSession(const uint64_t uid, const ETrackId trackId, const ESessionType sessionType)
    {
        SPDLOG_INFO("Starting session: {}", uid);
        mActiveSessionUid = uid;

        if (sSessionTypeToString.find(sessionType) == sSessionTypeToString.end())
        {
            SPDLOG_WARN("Could not find session type {}", (uint8_t)sessionType);
        }

        mRecord = true;
        auto trackName = sTrackIdToString.at(trackId);

        SPDLOG_DEBUG("Track {}", trackName);

        // Find proper race week, if exists
        std::tuple<int, int> location = FindSession(uid);
        auto raceIdx = std::get<0>(location);
        auto sessionIdx = std::get<1>(location);

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
        mRecord = false;
        mActiveSessionUid = 0;
    }

    bool SessionActive()
    {
        return mRecord;
    }

    uint16_t ActiveSessionUid()
    {
        return mActiveSessionUid;
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

                        // TODO: Getting session type 15 which does not exist? Maybe when race is quit then resumed?
                        std::string sessionTypeStr;
                        if (sSessionTypeToString.find(session->sessionType) == sSessionTypeToString.end())
                        {
                            sessionTypeStr = std::to_string((uint8_t)session->sessionType);
                        }
                        else
                        {
                            sessionTypeStr = sSessionTypeToString.at(session->sessionType);
                        }

                        const std::string sessionHeaderStr = sessionTypeStr + " [" + std::to_string(session->uid) + "]";

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
                                ImGui::Text("%.3f", lap->sector1MS / sMsPerSec);

                                ImGui::TableNextColumn();
                                if (lap->lapNumber == session->fastestSec2LapNum)
                                {
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(green));
                                }
                                ImGui::Text("%.3f", lap->sector2MS / sMsPerSec);

                                ImGui::TableNextColumn();
                                if (lap->lapNumber == session->fastestSec3LapNum)
                                {
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(green));
                                }
                                ImGui::Text("%.3f", lap->sector3MS / sMsPerSec);

                                ImGui::TableNextColumn();
                                if (lap->lapNumber == session->fastestLapNum)
                                {
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(green));
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
    uint64_t mActiveSessionUid{0};
    bool mRecord{false};
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
                        SPDLOG_TRACE("Found existing session. raceIdx {}, sessionIdx {}", raceIdx, sessionIdx);
                        return std::tuple(raceIdx, sessionIdx);
                    }
                    sessionIdx++;
                }
                raceIdx++;
            }
        }
        else
        {
            SPDLOG_TRACE("Session {} does not exist", uid);
        }

        return std::tuple(-1, -1); // Does not exist
    }
};