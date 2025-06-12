#pragma once

#include "f1telemetry.h"

#include "constants.h"
#include "f125/constants.h"
#include "f125/packets/cardamage.h"
#include "f125/packets/carsetup.h"
#include "f125/packets/carstatus.h"
#include "f125/packets/cartelemetry.h"
#include "f125/packets/event.h"
#include "f125/packets/lapdata.h"
#include "f125/packets/participants.h"
#include "f125/packets/session.h"
#include "header.h"
#include "spdlog/spdlog.h"

class CF125Telemetry : public CF1Telemetry
{
public:
    CF125Telemetry(const SF1TelemetryResources rsrcs)
        : CF1Telemetry(2025, rsrcs)
    {
        SPDLOG_TRACE("CF125Telemetry()");
    }

    void ParsePacket(char *packet, const SPacketHeader &header)
    {
        if (header.packetFormat != mYear)
        {
            SPDLOG_ERROR("Invalid packet format {}. Expected {}", header.packetFormat, mYear);
            return;
        }

        mSessionInfo->NewPacket();

        // Invalid
        if (header.packetId > F125::EPacketId::Max)
        {
            return;
        }

        const auto id = F125::EPacketId(header.packetId);
        const int playerIdx = header.playerCarIndex;

        // Parse based on packet id
        switch (id)
        {
        case F125::EPacketId::Participants:
        {
            // SPacketParticipantsData packetParticipantsData;
            // packetParticipantsData.get(packet);
            break;
        }
        case F125::EPacketId::CarSetups:
        {
            // SPacketCarSetupData carSetup;
            // carSetup.get(packet);
            break;
        }
        case F125::EPacketId::CarStatus:
        {
            F125::SPacketCarStatusData carStatus;
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
        case F125::EPacketId::CarTelemetry:
        {
            F125::SPacketCarTelemetryData carTelemetry;
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
        case F125::EPacketId::CarDamage:
        {
            F125::SPacketCarDamageData damage;
            damage.get(packet);

            std::array<float, 4> tyreWear;
            // Extract tyre wear data
            for (int i = 0; i < 4; ++i)
            {
                tyreWear[i] = damage.carDamageData[playerIdx].tyresWear[i];
            }
            mTyreWearGraph->SetTyreWear(tyreWear);

            // Extract car damage graph data
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
        case F125::EPacketId::LapData:
        {
            F125::SPacketLapData lapData;
            lapData.get(packet);
            const auto myRacePosition = lapData.lapData[playerIdx].carPosition;
            F125::SLapData carBehindLapData = {0};

            // Make sure we are in a race and there is a car behind us
            if (myRacePosition < (F125::sMaxNumCarsInUDPData - 1))
            {
                for (int i = 0; i < F125::sMaxNumCarsInUDPData; ++i)
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
            playerDeltas.deltaToCarInFrontInMS = (lapData.lapData[playerIdx].deltaToCarInFrontMinutesPart * 1000) + lapData.lapData[playerIdx].deltaToCarInFrontMSPart;
            playerDeltas.deltaToRaceLeaderInMS = (lapData.lapData[playerIdx].deltaToRaceLeaderMinutesPart * 1000) + lapData.lapData[playerIdx].deltaToRaceLeaderMSPart;
            playerDeltas.currentLapTimeInMS = lapData.lapData[playerIdx].currentLapTimeInMS;

            carBehindDeltas.carPosition = carBehindLapData.carPosition;
            carBehindDeltas.deltaToCarInFrontInMS = (carBehindLapData.deltaToCarInFrontMinutesPart * 1000) + carBehindLapData.deltaToCarInFrontMSPart;
            carBehindDeltas.deltaToRaceLeaderInMS = (carBehindLapData.deltaToCarInFrontMinutesPart * 1000) + carBehindLapData.deltaToRaceLeaderMSPart;
            carBehindDeltas.currentLapTimeInMS = carBehindLapData.currentLapTimeInMS;

            mSessionInfo->SessionStarted();
            mLapDeltas->SetLapData(playerDeltas, carBehindDeltas);
            mLapInfoHeader->SetCurrentLap(lapData.lapData[playerIdx].currentLapNum);
            break;
        }
        case F125::EPacketId::Session:
        {
            F125::SPacketSessionData sessionData;
            sessionData.get(packet);

            // TODO: Update to 25
            auto trackId = static_cast<F123::ETrackId>(sessionData.trackId);
            auto sessionType = static_cast<F123::ESessionType>(sessionData.sessionType);

            // Banking on this always coming before LapData
            if (!mSessionHistory->SessionActive())
            {
                mSessionHistory->StartSession(header.sessionUid, trackId, sessionType);
            }

            mLapInfoHeader->SetPitLapWindow(sessionData.pitStopWindowIdealLap, sessionData.pitStopWindowLatestLap, sessionData.pitStopRejoinPosition);
            break;
        }
        case F125::EPacketId::SessionHistory:
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
        case F125::EPacketId::Event:
        {
            F125::SPacketEventData event;
            switch (event.getCode(packet))
            {
            case F125::EEventCode::SessionStarted:
                mSessionInfo->SessionStarted();
                break;
            case F125::EEventCode::SessionEnded:
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
