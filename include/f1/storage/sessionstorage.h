#pragma once

#include "data.h"
#include "spdlog/spdlog.h"
#include "tinyxml2.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <windows.h>

namespace
{
    const char *sRootNodeKey = "RaceHistory";
    const char *sRaceWeekendNodeKey = "RaceWeekend";
    const char *sRaceWeekendTrackKey = "track";
    const char *sRaceWeekendDateKey = "date";
    const char *sSessionUidKey = "uid";
    const char *sSessionNodeKey = "session";
    const char *sSessionTypeKey = "type";
    const char *sLapKey = "lap";
    const char *sLapNumKey = "num";
    const char *sFastestSec1LapNumNodeKey = "fastestSec1LapNum";
    const char *sFastestSec2LapNumNodeKey = "fastestSec2LapNum";
    const char *sFastestSec3LapNumNodeKey = "fastestSec3LapNum";
    const char *sFastestLapNumNodeKey = "fastestLapNum";
    const char *sLapTimeNodeKey = "time";
    const char *sSector1MSNodeKey = "sector1MS";
    const char *sSector2MSNodeKey = "sector2MS";
    const char *sSector3MSNodeKey = "sector3MS";
}

class CSessionStorage
{
public:
    CSessionStorage()
    {
        SPDLOG_TRACE("CSessionStorage");
    }

    bool SetStoragePath(const std::filesystem::path &filePath)
    {
        if (!std::filesystem::exists(filePath))
        {
            std::cout << "Storage does not exist. Creating..." << std::endl;
            SPDLOG_DEBUG("Storage does not exist. Creating...");

            if (!CreateStorage(filePath))
            {
                SPDLOG_DEBUG("Failed to create race history storage");
                return false;
            }
        }

        mPath = filePath;
        SPDLOG_DEBUG("Storage path: {}", mPath.string());

        return true;
    }

    // Load n most recent sessions/race weeks (probably want a pointer return)
    std::vector<SessionStorage::SRaceWeekend> LoadRaceData(const int n = -1)
    {
        std::vector<SessionStorage::SRaceWeekend> races;
        if (mPath.string().empty())
        {
            SPDLOG_DEBUG("No storage path specified.");
            return races;
        }

        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(mPath.string().c_str()) != 0)
        {
            SPDLOG_DEBUG("Failed to read race history from {}", mPath.string());
            return races;
        }

        tinyxml2::XMLElement *root;
        try
        {
            root = doc.FirstChildElement(sRootNodeKey);
        }
        catch (const std::exception &e)
        {
            std::cout << e.what() << std::endl;
            return races;
        }

        // Iterate over races
        int i = 0;
        for (auto raceWkd = root->FirstChildElement(sRaceWeekendNodeKey); raceWkd != nullptr; raceWkd = raceWkd->NextSiblingElement())
        {
            SessionStorage::SRaceWeekend newRace;

            auto trackNameAttr = raceWkd->Attribute(sRaceWeekendTrackKey);
            if (trackNameAttr)
            {
                newRace.trackName = trackNameAttr;
            }
            auto dateAttr = raceWkd->Attribute(sRaceWeekendDateKey);
            if (dateAttr)
            {
                newRace.date = dateAttr;
            }

            // Iterate over sessions within raceWkd
            for (auto session = raceWkd->FirstChildElement(sSessionNodeKey); session != nullptr; session = session->NextSiblingElement())
            {
                SessionStorage::SSessionData newSession;
                try
                {
                    newSession.uid = static_cast<uint64_t>(session->Unsigned64Attribute(sSessionUidKey));
                    newSession.sessionType = static_cast<F123::ESessionType>(session->UnsignedAttribute(sSessionTypeKey));
                    newSession.fastestLapNum = static_cast<uint8_t>(session->FirstChildElement(sFastestLapNumNodeKey)->UnsignedText());
                    newSession.fastestSec1LapNum = static_cast<uint8_t>(session->FirstChildElement(sFastestSec1LapNumNodeKey)->UnsignedText());
                    newSession.fastestSec2LapNum = static_cast<uint8_t>(session->FirstChildElement(sFastestSec2LapNumNodeKey)->UnsignedText());
                    newSession.fastestSec3LapNum = static_cast<uint8_t>(session->FirstChildElement(sFastestSec3LapNumNodeKey)->UnsignedText());
                }
                catch (const std::exception &e)
                {
                    std::cout << e.what() << std::endl;
                    continue;
                }

                // Iterate over laps within session
                for (auto lap = session->FirstChildElement(sLapKey); lap != nullptr; lap = lap->NextSiblingElement())
                {
                    SessionStorage::SLap newLap;
                    try
                    {
                        newLap.lapNumber = static_cast<uint8_t>(lap->UnsignedAttribute(sLapNumKey));
                        newLap.sector1MS = static_cast<uint16_t>(lap->FirstChildElement(sSector1MSNodeKey)->UnsignedText());
                        newLap.sector2MS = static_cast<uint16_t>(lap->FirstChildElement(sSector2MSNodeKey)->UnsignedText());
                        newLap.sector3MS = static_cast<uint16_t>(lap->FirstChildElement(sSector3MSNodeKey)->UnsignedText());
                        newLap.totalLapTime = static_cast<uint32_t>(lap->FirstChildElement(sLapTimeNodeKey)->UnsignedText());
                    }
                    catch (const std::exception &e)
                    {
                        std::cout << e.what() << std::endl;
                        continue;
                    }

                    newSession.laps.push_back(newLap);
                }
                newRace.sessions.push_back(newSession);
            }

            // Increment loaded race count, if it was passed
            // TODO: pushing the race before checking if it should be
            // This needs work
            races.push_back(newRace);
            if (n != -1)
            {
                i++;
                if (i >= n)
                {
                    break;
                }
            }
        }
        return races;
    }

    // TODO: Return how many races were stored
    bool StoreRaceData(std::vector<SessionStorage::SRaceWeekend> races, int count)
    {
        if (mPath.string().empty())
        {
            SPDLOG_DEBUG("No storage path specified");
            return false;
        }
        if (count <= 0)
        {
            SPDLOG_DEBUG("No new races to store");
            return true;
        }

        tinyxml2::XMLDocument doc;
        doc.LoadFile(mPath.string().c_str());

        auto root = doc.FirstChildElement(sRootNodeKey);
        if (!root)
        {
            SPDLOG_DEBUG("Storage is not formatted properly!");
            return false;
        }

        SPDLOG_DEBUG("Storing {} races. Total in memory: {}", count, races.size());

        int startIdx = races.size() - count;
        SPDLOG_DEBUG("startIdx {}", startIdx);

        int newRaceIdx;
        for (int newRaceIdx = startIdx; newRaceIdx < races.size(); ++newRaceIdx)
        {
            int firstSessionToStore = 0; // Some sessions might be stored already
            bool sessionExists = false;
            // Loop through sessions in memory
            for (auto session : races[newRaceIdx].sessions)
            {
                sessionExists = false;
                auto sessionUid = std::to_string(session.uid);

                // Check if track already exists in storage (could have the same name as another, need to compare session uids)
                for (auto raceWkd = root->FirstChildElement(sRaceWeekendNodeKey); raceWkd != nullptr; raceWkd = raceWkd->NextSiblingElement())
                {
                    auto trackNameAttr = raceWkd->Attribute(sRaceWeekendTrackKey);

                    SPDLOG_DEBUG("Checking if trackname {} has uid {}", trackNameAttr, sessionUid);

                    if (trackNameAttr && std::string(trackNameAttr) == races[newRaceIdx].trackName)
                    {
                        // Check if session uid exists in this race weekend
                        for (auto session = raceWkd->FirstChildElement(sSessionNodeKey); session != nullptr; session = session->NextSiblingElement())
                        {
                            auto storedSessionUidAttr = session->Attribute(sSessionUidKey);
                            if (storedSessionUidAttr && std::string(storedSessionUidAttr) == sessionUid)
                            {
                                // TODO: Check if all the laps of this session are in storage, if not add them
                                SPDLOG_DEBUG("Found matching session on track {} {}/{} (new/stored)", trackNameAttr, sessionUid, std::string(storedSessionUidAttr));

                                // Session at this index already exists, check next
                                firstSessionToStore++;
                                sessionExists = true;
                                break;
                            }
                        }

                        if (sessionExists)
                        {
                            // Already found this session, move to next one
                            break;
                        }
                    }
                }
            }

            if (firstSessionToStore >= races[newRaceIdx].sessions.size())
            {
                SPDLOG_DEBUG("All sessions within {} already exist in storage", races[newRaceIdx].trackName);
                continue;
            }

            SPDLOG_DEBUG("Session {} at {} does not exist in storage", races[newRaceIdx].sessions.at(firstSessionToStore).uid, races[newRaceIdx].trackName);

            tinyxml2::XMLElement *raceNode;
            std::string lastRaceWkndTrack{""};
            // Was the last race weekend on the same track as this session?
            auto lastRaceWknd = root->LastChildElement(sRaceWeekendNodeKey);
            if (lastRaceWknd)
            {
                lastRaceWkndTrack = std::string(lastRaceWknd->Attribute(sRaceWeekendTrackKey));
            }
            SPDLOG_DEBUG("Last stored track: {}, new track: {}", lastRaceWkndTrack, races[newRaceIdx].trackName);
            if ((lastRaceWknd) && (lastRaceWkndTrack == races[newRaceIdx].trackName))
            {
                SPDLOG_DEBUG("Session {} {} track matches the most recent stored track {}", races[newRaceIdx].sessions.at(firstSessionToStore).uid, races[newRaceIdx].trackName, lastRaceWkndTrack);
                raceNode = lastRaceWknd;
            }
            else
            {
                SPDLOG_DEBUG("New race weekend on track {}", races[newRaceIdx].trackName);
                raceNode = doc.NewElement(sRaceWeekendNodeKey);
                raceNode->SetAttribute(sRaceWeekendTrackKey, races[newRaceIdx].trackName.c_str());
                raceNode->SetAttribute(sRaceWeekendDateKey, races[newRaceIdx].date.c_str());
            }

            SPDLOG_DEBUG("First session to store idx: {} / uid {}", firstSessionToStore, races[newRaceIdx].sessions[firstSessionToStore].uid);

            // Race sessions to be stored
            for (int j = firstSessionToStore; j < races[newRaceIdx].sessions.size(); ++j)
            {
                auto sessionNode = doc.NewElement(sSessionNodeKey);
                sessionNode->SetAttribute(sSessionUidKey, std::to_string(races[newRaceIdx].sessions[j].uid).c_str());
                sessionNode->SetAttribute(sSessionTypeKey, static_cast<uint8_t>(races[newRaceIdx].sessions[j].sessionType));

                auto fastestSec1LapNum = doc.NewElement(sFastestSec1LapNumNodeKey);
                fastestSec1LapNum->SetText(std::to_string(races[newRaceIdx].sessions[j].fastestSec1LapNum).c_str());
                auto fastestSec2LapNum = doc.NewElement(sFastestSec2LapNumNodeKey);
                fastestSec2LapNum->SetText(std::to_string(races[newRaceIdx].sessions[j].fastestSec2LapNum).c_str());
                auto fastestSec3LapNum = doc.NewElement(sFastestSec3LapNumNodeKey);
                fastestSec3LapNum->SetText(std::to_string(races[newRaceIdx].sessions[j].fastestSec3LapNum).c_str());
                auto fastestLapNum = doc.NewElement(sFastestLapNumNodeKey);
                fastestLapNum->SetText(std::to_string(races[newRaceIdx].sessions[j].fastestLapNum).c_str());
                sessionNode->InsertEndChild(fastestSec1LapNum);
                sessionNode->InsertEndChild(fastestSec2LapNum);
                sessionNode->InsertEndChild(fastestSec3LapNum);
                sessionNode->InsertEndChild(fastestLapNum);

                // Session laps
                for (auto lap : races[newRaceIdx].sessions[j].laps)
                {
                    auto lapNode = doc.NewElement(sLapKey);
                    lapNode->SetAttribute(sLapNumKey, lap.lapNumber);

                    auto time = doc.NewElement(sLapTimeNodeKey);
                    time->SetText(std::to_string(lap.totalLapTime).c_str());
                    auto sector1Time = doc.NewElement(sSector1MSNodeKey);
                    sector1Time->SetText(std::to_string(lap.sector1MS).c_str());
                    auto sector2Time = doc.NewElement(sSector2MSNodeKey);
                    sector2Time->SetText(std::to_string(lap.sector2MS).c_str());
                    auto sector3Time = doc.NewElement(sSector3MSNodeKey);
                    sector3Time->SetText(std::to_string(lap.sector3MS).c_str());

                    lapNode->InsertEndChild(time);
                    lapNode->InsertEndChild(sector1Time);
                    lapNode->InsertEndChild(sector2Time);
                    lapNode->InsertEndChild(sector3Time);

                    sessionNode->InsertEndChild(lapNode);
                }
                raceNode->InsertEndChild(sessionNode);
            }
            root->InsertEndChild(raceNode);
        }

        // Save to file
        return doc.SaveFile(mPath.string().c_str()) == 0;
    }

private:
    std::filesystem::path mPath{""};

    // Create race history storage directory and XML file
    bool CSessionStorage::CreateStorage(const std::filesystem::path &filePath)
    {
        try
        {
            // Create storage directory if does not exist
            if (!std::filesystem::exists(filePath.parent_path()))
            {
                std::filesystem::create_directory(filePath.parent_path());
            }

            // Create XML skeleton
            tinyxml2::XMLDocument storage;
            tinyxml2::XMLNode *pRoot = storage.NewElement(sRootNodeKey);
            storage.InsertEndChild(pRoot);
            tinyxml2::XMLDeclaration *declaration = storage.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
            storage.InsertFirstChild(declaration);

            return (storage.SaveFile(filePath.string().c_str()) == tinyxml2::XML_SUCCESS);
        }
        catch (const std::exception &e)
        {
            SPDLOG_ERROR("Error when creating storage at {}: {}", filePath.string(), e.what());
            return false;
        }
    }
};