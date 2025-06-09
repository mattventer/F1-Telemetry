#pragma once

#include "../f1telemetry.h"

// TODO: Update to 2025
#include "../f125/cardamage.h"
#include "../f125/carsetup.h"
#include "../f125/carstatus.h"
#include "../f125/cartelemetry.h"
#include "../f125/constants.h"
#include "../f125/constants.h" // Most are the same
#include "../f125/event.h"
#include "../f125/lapdata.h"
#include "../f125/participants.h"
#include "../f125/session.h"
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
        SPDLOG_TRACE("CF125Telemetry::ParsePacket()");

        if (header.packetFormat != mYear)
        {
            SPDLOG_ERROR("Invalid packet format {}. Expected {}", header.packetFormat, mYear);
            return;
        }

        // Parse based on packet id
        const F125::EPacketId id = F125::sPacketIds[header.packetId];
        const int playerIdx = header.playerCarIndex;

        switch (id)
        {
        case F125::EPacketId::Participants:
        {
            SPacketParticipantsData packetParticipantsData;
            packetParticipantsData.get(packet);
            break;
        }
        case F125::EPacketId::CarSetups:
        {
            SPacketCarSetupData carSetup;
            carSetup.get(packet);
            break;
        }
        case F125::EPacketId::CarStatus:
        {
            SPacketCarStatusData carStatus;
            carStatus.get(packet);
            try
            {
                auto actual = static_cast<F125::EActualTyreCompound>(carStatus.carStatusData[playerIdx].actualTyreCompound);
                auto visual = static_cast<F125::EVisualTyreCompound>(carStatus.carStatusData[playerIdx].visualTyreCompound);
                mTyreTemps->SetTyreCompound(actual, visual);
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
            mTyreWearGraph->SetTyreWear(tyreWear);
            mCarDamageGraph->SetCarDamage(damage.carDamageData[playerIdx]);
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

            mSessionInfo->SessionStarted();
            mLapDeltas->SetLapData(lapData.lapData[playerIdx], carBehindLapData);
            mLapInfoHeader->SetCurrentLap(lapData.lapData[playerIdx].currentLapNum);
            break;
        }
        case EPacketId::Session:
        {
            const auto sessionUid = header.sessionUid;
            // sSessionInfo->SessionStarted(); // Handled in events
            SPacketSessionData sessionData;
            sessionData.get(packet);

            auto trackId = static_cast<ETrackId>(sessionData.trackId);
            auto sessionType = static_cast<ESessionType>(sessionData.sessionType);

            // Banking on this always coming before LapData
            if (!mSessionHistory->SessionActive())
            {
                mSessionHistory->StartSession(sessionUid, trackId, sessionType);
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
                mLapDeltas->ResetLapData();
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
            break;
        }
        default:
            break;
        }
    }
};