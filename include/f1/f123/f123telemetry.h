#pragma once

#include "f1telemetry.h"

#include "constants.h"
#include "f123/packets/cardamage.h"
#include "f123/packets/carsetup.h"
#include "f123/packets/carstatus.h"
#include "f123/packets/cartelemetry.h"
#include "f123/packets/event.h"
#include "f123/packets/lapdata.h"
#include "f123/packets/participants.h"
#include "f123/packets/session.h"
#include "header.h"
#include "spdlog/spdlog.h"

using namespace F123;

class CF123Telemetry : public CF1Telemetry
{
public:
    CF123Telemetry(const SF1TelemetryResources rsrcs)
        : CF1Telemetry(2023, rsrcs)
    {
        SPDLOG_TRACE("CF123Telemetry()");
    }

    void ParsePacket(char *packet, const SPacketHeader &header)
    {
        SPDLOG_TRACE("CF123Telemetry::ParsePacket()");

        if (header.packetFormat != mYear)
        {
            SPDLOG_ERROR("Invalid packet format {}. Expected {}", header.packetFormat, mYear);
            return;
        }

        mSessionInfo->NewPacket();

        // Invalid
        if (header.packetId > F123::EPacketId::Max)
        {
            return;
        }

        const auto id = F123::EPacketId(header.packetId);
        const int playerIdx = header.playerCarIndex;

        switch (id)
        {
        case F123::EPacketId::Participants:
        {
            SPDLOG_TRACE("EPacketId::Participants");
            // SPacketParticipantsData packetParticipantsData;
            // packetParticipantsData.get(packet);
            break;
        }
        case F123::EPacketId::CarSetups:
        {
            SPDLOG_TRACE("EPacketId::CarSetups");
            // SPacketCarSetupData carSetup;
            // carSetup.get(packet);
            break;
        }
        case F123::EPacketId::CarStatus:
        {
            SPDLOG_TRACE("EPacketId::CarStatus");
            F123::SPacketCarStatusData carStatus;
            carStatus.get(packet);
            try
            {
                mTyreTemps->SetTyreCompound(carStatus.carStatusData[playerIdx].actualTyreCompound, carStatus.carStatusData[playerIdx].visualTyreCompound);
            }
            catch (const std::exception &e)
            {
                SPDLOG_ERROR("Car status exception: {}", e.what());
            }
            break;
        }
        case F123::EPacketId::CarTelemetry:
        {
            SPDLOG_TRACE("EPacketId::CarTelemetry");
            F123::SPacketCarTelemetryData carTelemetry;
            carTelemetry.get(packet);

            std::array<uint8_t, 4> tyreInnerTemps;
            // Get data
            for (int i = 0; i < 4; ++i)
            {
                tyreInnerTemps[i] = carTelemetry.carTelemetryData[playerIdx].tyresInnerTemperature[i];
            }
            mTyreTemps->SetTyreInnerTemps(tyreInnerTemps);
            break;
        }
        case F123::EPacketId::CarDamage:
        {
            SPDLOG_TRACE("EPacketId::CarDamage");
            F123::SPacketCarDamageData damage;
            damage.get(packet);

            std::array<float, 4> tyreWear;
            // Get data
            for (int i = 0; i < 4; ++i)
            {
                tyreWear[i] = damage.carDamageData[playerIdx].tyresWear[i];
            }
            // TODO: move CTyreWear into CCarDamage
            mTyreWearGraph->SetTyreWear(tyreWear);

            SCarDamageGraphData graphData;
            graphData.engineDamage = damage.carDamageData[playerIdx].engineDamage;
            graphData.gearBoxDamage = damage.carDamageData[playerIdx].gearBoxDamage;
            graphData.engineMGUHWear = damage.carDamageData[playerIdx].engineMGUHWear;
            graphData.engineESWear = damage.carDamageData[playerIdx].engineESWear;
            graphData.engineCEWear = damage.carDamageData[playerIdx].engineCEWear;
            graphData.engineICEWear = damage.carDamageData[playerIdx].engineICEWear;
            graphData.engineMGUKWear = damage.carDamageData[playerIdx].engineMGUKWear;
            graphData.engineTCWear = damage.carDamageData[playerIdx].engineTCWear;
            mCarDamageGraph->SetCarDamage(graphData);
            break;
        }
        case F123::EPacketId::LapData:
        {
            SPDLOG_TRACE("EPacketId::LapData");
            F123::SPacketLapData lapData;
            lapData.get(packet);
            const auto myRacePosition = lapData.lapData[playerIdx].carPosition;
            F123::SLapData carBehindLapData = {0};

            // Make sure we are in a race
            if (myRacePosition < 21)
            {
                for (int i = 0; i < 22; ++i)
                {
                    // Car behind lap data
                    if (lapData.lapData[i].carPosition == myRacePosition + 1)
                    {
                        carBehindLapData = lapData.lapData[i];
                        break;
                    }
                }
            }

            // Extract lap data
            SLapDeltasData playerDeltas, carBehindDeltas;
            playerDeltas.carPosition = lapData.lapData[playerIdx].carPosition;
            playerDeltas.deltaToCarInFrontInMS = lapData.lapData[playerIdx].deltaToCarInFrontInMS;
            playerDeltas.deltaToRaceLeaderInMS = lapData.lapData[playerIdx].deltaToRaceLeaderInMS;
            playerDeltas.currentLapTimeInMS = lapData.lapData[playerIdx].currentLapTimeInMS;

            carBehindDeltas.carPosition = carBehindLapData.carPosition;
            carBehindDeltas.deltaToCarInFrontInMS = carBehindLapData.deltaToCarInFrontInMS;
            carBehindDeltas.deltaToRaceLeaderInMS = carBehindLapData.deltaToRaceLeaderInMS;
            carBehindDeltas.currentLapTimeInMS = carBehindLapData.currentLapTimeInMS;

            mSessionInfo->SessionStarted();
            mLapDeltas->SetLapData(playerDeltas, carBehindDeltas);
            mLapInfoHeader->SetCurrentLap(lapData.lapData[playerIdx].currentLapNum);
            break;
        }
        case F123::EPacketId::Session:
        {
            SPDLOG_TRACE("EPacketId::Session");
            F123::SPacketSessionData sessionData;
            sessionData.get(packet);

            // Banking on this always coming before LapData
            if (!mSessionHistory->SessionActive())
            {
                mSessionHistory->StartSession(F1::Version::F123, header.sessionUid, sessionData.trackId, sessionData.sessionType);
            }
            mLapInfoHeader->SetPitLapWindow(sessionData.pitStopWindowIdealLap, sessionData.pitStopWindowLatestLap, sessionData.pitStopRejoinPosition);
            break;
        }
        case F123::EPacketId::SessionHistory:
        {
            SPDLOG_TRACE("EPacketId::SessionHistory");
            SPacketSessionHistoryData sessionHistory;
            sessionHistory.get(packet);

            // Only care about the player data
            if (sessionHistory.carIdx == playerIdx)
            {
                // TODO: change to generic struct, not packet
                mSessionHistory->SetSessionHistoryData(sessionHistory);
            }
            break;
        }
        case F123::EPacketId::Event:
        {
            SPDLOG_TRACE("EPacketId::Event");
            F123::SPacketEventData event;
            switch (event.getCode(packet))
            {
            case F123::EEventCode::SessionStarted:
                mSessionInfo->SessionStarted();
                break;
            case F123::EEventCode::SessionEnded:
                mSessionInfo->SessionStopped();
                try
                {
                    const auto activeSessionUid = mSessionHistory->ActiveSessionUid();
                    SPDLOG_ERROR("Storing session history for {}", activeSessionUid);
                    mSessionHistory->StoreSessionHistory();
                }
                catch (const std::exception &e)
                {
                    SPDLOG_ERROR("Storing session history caught exception {}", e.what());
                }
                mSessionHistory->StopSession();
                break;
            default:
                break;
            }
        }
        }
    }
};
