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

        mSessionInfo->NewPacket(header.packetFormat);

        // Parse based on packet id
        const EPacketId id = sPacketIds[header.packetId];
        const int playerIdx = header.playerCarIndex;

        switch (id)
        {
        case EPacketId::Participants:
        {
            // SPacketParticipantsData packetParticipantsData;
            // packetParticipantsData.get(packet);
            break;
        }
        case EPacketId::CarSetups:
        {
            // SPacketCarSetupData carSetup;
            // carSetup.get(packet);
            break;
        }
        case EPacketId::CarStatus:
        {
            SPacketCarStatusData carStatus;
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
        case EPacketId::CarTelemetry:
        {
            SPacketCarTelemetryData carTelemetry;
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
        case EPacketId::CarDamage:
        {
            SPacketCarDamageData damage;
            damage.get(packet);

            std::array<float, 4> tyreWear;
            // Get data
            for (int i = 0; i < 4; ++i)
            {
                tyreWear[i] = damage.carDamageData[playerIdx].tyresWear[i];
            }
            // TODO: move CTyreWear into CCarDamage
            // mTyreWearGraph->SetTyreWear(tyreWear);

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
        case EPacketId::LapData:
        {
            SPacketLapData lapData;
            lapData.get(packet);
            const auto myRacePosition = lapData.lapData[playerIdx].carPosition;
            SLapData carBehindLapData = {0};

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
        case EPacketId::Session:
        {
            SPacketSessionData sessionData;
            sessionData.get(packet);

            auto trackId = static_cast<ETrackId>(sessionData.trackId);
            auto sessionType = static_cast<ESessionType>(sessionData.sessionType);

            // Banking on this always coming before LapData
            if (!mSessionHistory->SessionActive())
            {
                mSessionHistory->StartSession(header.sessionUid, trackId, sessionType);
            }

            // // TODO: fix, always 0
            // SPDLOG_INFO("Pit: {}-{} rejoin: {}", sessionData.pitStopWindowIdealLap, sessionData.pitStopWindowLatestLap, sessionData.pitStopRejoinPosition);
            // sLapInfoHeader->SetPitLapWindow(sessionData.pitStopWindowIdealLap, sessionData.pitStopWindowLatestLap, sessionData.pitStopRejoinPosition);
            break;
        }
        case EPacketId::SessionHistory:
        {
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
        case EPacketId::Event:
        {
            SPacketEventData event;
            switch (event.getCode(packet))
            {
            case EEventCode::SessionStarted:
                mSessionInfo->SessionStarted();
                break;
            case EEventCode::SessionEnded:
                mSessionInfo->SessionStopped();
                mSessionHistory->StopSession();
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
                break;
            default:
                break;
            }
        }
        }
    }
};
