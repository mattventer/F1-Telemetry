#pragma once

#include "data.h"
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
    }

    bool SetStoragePath(const std::filesystem::path &filePath)
    {
        if (!std::filesystem::exists(filePath))
        {
            std::cout << "Storage does not exist. Creating..." << std::endl;
            if (!CreateStorage(filePath))
            {
                std::cout << "Failed to create race history storage" << std::endl;
                return false;
            }
        }

        mPath = filePath;
        return true;
    }

    // Load n most recent sessions/race weeks (probably want a pointer return)
    std::vector<SessionStorage::SRaceWeekend> LoadRaceData(const int n = -1)
    {
        std::vector<SessionStorage::SRaceWeekend> races;
        if (mPath.string().empty())
        {
            std::cout << "No storage path specified." << std::endl;
            return races;
        }

        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(mPath.string().c_str()) != 0)
        {
            std::cout << "Failed to read race history from " << mPath.string() << std::endl;
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

            races.push_back(newRace); // TODO: pushing the race before checking if it should be
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

    bool StoreRaceData(std::vector<SessionStorage::SRaceWeekend> races, int count)
    {
        if (mPath.string().empty())
        {
            std::cout << "No storage path specified" << std::endl;
            return false;
        }
        if (count <= 0)
        {
            std::cout << "No new races" << std::endl;
            return true;
        }

        tinyxml2::XMLDocument doc;
        doc.LoadFile(mPath.string().c_str());

        auto root = doc.FirstChildElement(sRootNodeKey);
        if (!root)
        {
            std::cout << "Storage is not formatted properly" << std::endl;
            return false;
        }

        std::cout << "Storing " << count << " race(s)" << std::endl;

        int startIdx = races.size() - count;
        for (int i = startIdx; i < races.size(); ++i)
        {
            int firstSessionToStore = startIdx; // Some sessions might be stored already
            for (auto session : races[i].sessions)
            {
                bool sessionExists = false;
                auto sessionUid = std::to_string(session.uid);

                // Check if track already exists in storage (could have the same name as another, need to compare session uids)
                for (auto raceWkd = root->FirstChildElement(sRaceWeekendNodeKey); raceWkd != nullptr; raceWkd = raceWkd->NextSiblingElement())
                {
                    auto trackNameAttr = raceWkd->Attribute(sRaceWeekendTrackKey);
                    if (trackNameAttr && std::string(trackNameAttr) == races[i].trackName)
                    {
                        // Check if session uid exists in this race weekend
                        for (auto session = raceWkd->FirstChildElement(sSessionNodeKey); session != nullptr; session = session->NextSiblingElement())
                        {
                            auto sessionUidAttr = session->Attribute(sSessionUidKey);
                            if (sessionUidAttr && std::string(sessionUidAttr) == sessionUid)
                            {
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

            if (firstSessionToStore >= races[i].sessions.size() - 1)
            {
                std::cout << "All sessions within " << races[i].trackName << " already exist in storage" << std::endl;
                continue;
            }

            std::cout << "New race weekend with trackname " << races[i].trackName << std::endl;
            // New race weekend
            auto raceNode = doc.NewElement(sRaceWeekendNodeKey);
            raceNode->SetAttribute(sRaceWeekendTrackKey, races[i].trackName.c_str());
            raceNode->SetAttribute(sRaceWeekendDateKey, races[i].date.c_str());

            // Race sessions to be stored
            for (int j = firstSessionToStore; j < races[i].sessions.size(); ++j)
            {
                auto sessionNode = doc.NewElement(sSessionNodeKey);
                sessionNode->SetAttribute(sSessionUidKey, std::to_string(races[i].sessions[j].uid).c_str());
                sessionNode->SetAttribute(sSessionTypeKey, static_cast<uint8_t>(races[i].sessions[j].sessionType));

                auto fastestSec1LapNum = doc.NewElement(sFastestSec1LapNumNodeKey);
                fastestSec1LapNum->SetText(std::to_string(races[i].sessions[j].fastestSec1LapNum).c_str());
                auto fastestSec2LapNum = doc.NewElement(sFastestSec2LapNumNodeKey);
                fastestSec2LapNum->SetText(std::to_string(races[i].sessions[j].fastestSec2LapNum).c_str());
                auto fastestSec3LapNum = doc.NewElement(sFastestSec3LapNumNodeKey);
                fastestSec3LapNum->SetText(std::to_string(races[i].sessions[j].fastestSec3LapNum).c_str());
                auto fastestLapNum = doc.NewElement(sFastestLapNumNodeKey);
                fastestLapNum->SetText(std::to_string(races[i].sessions[j].fastestLapNum).c_str());
                sessionNode->InsertEndChild(fastestSec1LapNum);
                sessionNode->InsertEndChild(fastestSec2LapNum);
                sessionNode->InsertEndChild(fastestSec3LapNum);
                sessionNode->InsertEndChild(fastestLapNum);

                // Session laps
                for (auto lap : races[i].sessions[j].laps)
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
            std::cout << "Error when creating storage at " << filePath.string() << ": " << e.what() << std::endl;
            return false;
        }
    }
};