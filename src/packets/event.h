#pragma once

#include "../f122constants.h"
#include "header.h"

#include <cstdint>
#include <memory>
#include <string>
#include <stdlib.h>

using namespace F122;

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
    } SRetirement;

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
        uint8_t vehicleIdx;
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
};

struct SPacketEventData
{
    SPacketHeader header;

    uint8_t eventStringCode[4];
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