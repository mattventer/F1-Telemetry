#pragma once

#include "constants.h"
#include "header.h"

#include <cstdint>
#include <memory>
#include <string>
#include <stdlib.h>

namespace F125
{
    static constexpr uint8_t sEventStringCodeLen = 4;

    union UEventDataDetails
    {
        struct
        {
            uint8_t vehicleIdx;
            float lapTime;
        } SFastestLap;

        struct
        {
            uint8_t vehicleIdx;
            uint8_t reason; // Result reason - 0 = invalid, 1 = retired, 2 = finished, 3 = terminal damage, 4 = inactive, 5 = not enough laps completed, 6 = black flagged
                            // 7 = red flagged, 8 = mechanical failure, 9 = session skipped, 10 = session simulated
        } SRetirement;

        struct
        {
            uint8_t reason; // 0 = Wet track, 1 = Safety car deployed, 2 = Red flag, 3 = Min lap not reached
        } DRSDisabled;

        struct
        {
            uint8_t vehicleIdx;
        } STeammateInPits;

        struct
        {
            uint8_t vehicleIdx;
        } SRaceWinner;

        struct
        {
            uint8_t penaltyType;
            uint8_t infringementType;
            uint8_t vehicleIdx;
            uint8_t otherVehicleIdx;
            uint8_t time;
            uint8_t lapNum;
            uint8_t placesGained;
        } SPenalty;

        struct
        {
            uint8_t vehicleIdx;
            float speed;
            uint8_t isOverallFastestInSession;
            uint8_t isDriverFastestInSession;
            uint8_t fastestVehicleIdxInSession;
            float fastestSpeedInSession;
        } SSpeedTrap;

        struct
        {
            uint8_t numLights;
        } SStartLights;

        struct
        {
            uint8_t vehicleIdx;
        } SDriveThroughPenaltyServed;

        struct
        {
            uint8_t vehicleIdx; // Vehicle index of the vehicle serving stop go
            float stopTime;     // Time spent serving stop go in seconds
        } SStopGoPenaltyServed;

        struct
        {
            uint32_t flashbackFrameIdentifier;
            float flashbackSessionTime;
        } SFlashback;

        struct
        {
            uint32_t buttonStatus;
        } SButtons;

        struct
        {
            uint8_t overtakingVehicleIdx;
            uint8_t beingOvertakenVehicleIdx;
        } SOvertake;

        struct
        {
            uint8_t safetyCarType; // 0 = No Safety Car, 1 = Full Safety Car, 2 = Virtual Safety Car, 3 = Formation Lap Safety Car
            uint8_t eventType;     // 0 = Deployed, 1 = Returning, 2 = Returned, 3 = Resume Race
        } SSafetyCar;

        struct
        {
            uint8_t vehicle1Idx; // Vehicle index of the first vehicle involved in the collision
            uint8_t vehicle2Idx; // Vehicle index of the second vehicle involved in the collision
        } SCollision;
    };

    struct SPacketEventData
    {
        SPacketHeader header;

        uint8_t eventStringCode[sEventStringCodeLen];
        UEventDataDetails eventDetails;

        EEventCode getCode(char *buffer)
        {
            unsigned long offset = this->header.get(buffer);

            for (int i = 0; i < 4; i++)
            {
                memcpy(&this->eventStringCode[i], &buffer[offset],
                       sizeof(this->eventStringCode[i]));
                offset = offset + sizeof(this->eventStringCode[i]);
            }

            std::string eventString = (const char *)this->eventStringCode;
            return sEventCodes.at(eventString.substr(0, 4));
        }
    };

}