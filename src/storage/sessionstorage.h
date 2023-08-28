#pragma once

#include "data.h"
#include "tinyxml2.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace
{
    const char *sRaceNodeKey = "Race";
    const char *sRootNodeKey = "RaceHistory";
    const char *sSessionNodeKey = "session";
    const char *sLapNodeKey = "lap";
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

    bool SetStoragePath(const std::filesystem::path &path)
    {
        if (!std::filesystem::exists(path))
        {
            std::cout << "Storage path " << path.string() << " does not exist. Session storage unavailable." << std::endl;
            return false;
        }

        mPath = path;
        return true;
    }

    // Load n most recent sessions/race weeks (probably want a pointer return)
    std::vector<SessionStorage::SRaceData> LoadRaceData(const int n = -1)
    {
        std::vector<SessionStorage::SRaceData> races;
        if (mPath.string().empty())
        {
            std::cout << "No storage path specified." << std::endl;
            return races;
        }

        tinyxml2::XMLDocument doc;
        doc.LoadFile(mPath.string().c_str());

        auto root = doc.FirstChildElement(sRootNodeKey);

        // Iterate over races
        int i = 0;
        for (auto race = root->FirstChildElement(sRaceNodeKey); race != nullptr; race = race->NextSiblingElement())
        {
            SessionStorage::SRaceData newRace;
            newRace.trackName = race->FirstChildElement("track")->GetText();

            // Iterate over sessions within race
            for (auto session = race->FirstChildElement(sSessionNodeKey); session != nullptr; session = session->NextSiblingElement())
            {
                SessionStorage::SSessionData newSession;
                newSession.sessionType = static_cast<F122::ESessionType>(session->UnsignedAttribute("type"));
                newSession.fastestLapNum = static_cast<uint8_t>(session->FirstChildElement(sFastestLapNumNodeKey)->UnsignedText());
                newSession.fastestSec1LapNum = static_cast<uint8_t>(session->FirstChildElement(sFastestSec1LapNumNodeKey)->UnsignedText());
                newSession.fastestSec2LapNum = static_cast<uint8_t>(session->FirstChildElement(sFastestSec2LapNumNodeKey)->UnsignedText());
                newSession.fastestSec3LapNum = static_cast<uint8_t>(session->FirstChildElement(sFastestSec3LapNumNodeKey)->UnsignedText());

                // Iterate over laps within session
                for (auto lap = session->FirstChildElement(sLapNodeKey); lap != nullptr; lap = lap->NextSiblingElement())
                {
                    SessionStorage::SLap newLap;
                    newLap.lapNumber = static_cast<uint8_t>(lap->UnsignedAttribute("num"));
                    newLap.sector1MS = static_cast<uint16_t>(lap->FirstChildElement(sSector1MSNodeKey)->UnsignedText());
                    newLap.sector2MS = static_cast<uint16_t>(lap->FirstChildElement(sSector2MSNodeKey)->UnsignedText());
                    newLap.sector3MS = static_cast<uint16_t>(lap->FirstChildElement(sSector3MSNodeKey)->UnsignedText());
                    newLap.totalLapTime = static_cast<uint32_t>(lap->FirstChildElement(sLapTimeNodeKey)->UnsignedText());
                    newSession.laps.push_back(newLap);
                }
                newRace.sessions.push_back(newSession);
            }

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

    bool StoreRaceData(std::vector<SessionStorage::SRaceData> races, int count)
    {
        if (mPath.string().empty())
        {
            std::cout << "No storage path specified." << std::endl;
            return false;
        }
        if (count <= 0)
        {
            std::cout << "No new races" << std::endl;
            return false;
        }

        tinyxml2::XMLDocument doc;
        doc.LoadFile(mPath.string().c_str());

        auto root = doc.FirstChildElement(sRootNodeKey);

        if (!root)
        {
            std::cout << "Storage is empty" << std::endl;
            return false;
        }

        std::cout << "storing " << count << " races" << std::endl;
        int startIdx = races.size() - count;
        // Races
        for (int i = startIdx; i < races.size(); ++i)
        {
            auto raceNode = doc.NewElement(sRaceNodeKey);
            auto trackName = doc.NewElement("track");
            trackName->SetText(races[i].trackName.c_str());
            raceNode->InsertEndChild(trackName);
            // Race sessions
            for (auto session : races[i].sessions)
            {
                auto sessionNode = doc.NewElement(sSessionNodeKey);
                sessionNode->SetAttribute("type", static_cast<uint8_t>(session.sessionType));

                auto fastestSec1LapNum = doc.NewElement(sFastestSec1LapNumNodeKey);
                fastestSec1LapNum->SetText(std::to_string(session.fastestSec1LapNum).c_str());
                auto fastestSec2LapNum = doc.NewElement(sFastestSec2LapNumNodeKey);
                fastestSec2LapNum->SetText(std::to_string(session.fastestSec2LapNum).c_str());
                auto fastestSec3LapNum = doc.NewElement(sFastestSec3LapNumNodeKey);
                fastestSec3LapNum->SetText(std::to_string(session.fastestSec3LapNum).c_str());
                auto fastestLapNum = doc.NewElement(sFastestLapNumNodeKey);
                fastestLapNum->SetText(std::to_string(session.fastestLapNum).c_str());
                sessionNode->InsertEndChild(fastestSec1LapNum);
                sessionNode->InsertEndChild(fastestSec2LapNum);
                sessionNode->InsertEndChild(fastestSec3LapNum);
                sessionNode->InsertEndChild(fastestLapNum);

                // Session laps
                for (auto lap : session.laps)
                {
                    auto lapNode = doc.NewElement(sLapNodeKey);
                    lapNode->SetAttribute("num", lap.lapNumber);

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
};