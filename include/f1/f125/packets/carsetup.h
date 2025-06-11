#pragma once

#include "constants.h"
#include "header.h"

#include <cstdint>
#include <iostream>
#include <memory>

namespace F125
{

    struct SCarSetupData
    {
        uint8_t frontWing;             // Front wing aero
        uint8_t rearWing;              // Rear wing aero
        uint8_t onThrottle;            // Differential adjustment on throttle (percentage)
        uint8_t offThrottle;           // Differential adjustment off throttle (percentage)
        float frontCamber;             // Front camber angle (suspension geometry)
        float rearCamber;              // Rear camber angle (suspension geometry)
        float frontToe;                // Front toe angle (suspension geometry)
        float rearToe;                 // Rear toe angle (suspension geometry)
        uint8_t frontSuspension;       // Front suspension
        uint8_t rearSuspension;        // Rear suspension
        uint8_t frontAntiRollBar;      // Front anti-roll bar
        uint8_t rearAntiRollBar;       // Front anti-roll bar
        uint8_t frontSuspensionHeight; // Front ride height
        uint8_t rearSuspensionHeight;  // Rear ride height
        uint8_t brakePressure;         // Brake pressure (percentage)
        uint8_t brakeBias;             // Brake bias (percentage)
        float rearLeftTyrePressure;    // Rear left tyre pressure (PSI)
        float rearRightTyrePressure;   // Rear right tyre pressure (PSI)
        float frontLeftTyrePressure;   // Front left tyre pressure (PSI)
        float frontRightTyrePressure;  // Front right tyre pressure (PSI)
        uint8_t ballast;               // Ballast
        float fuelLoad;                // Fuel load

        unsigned long get(char *buffer, unsigned long offset)
        {
            memcpy(&this->frontWing, &buffer[offset], sizeof(this->frontWing));
            offset += sizeof(this->frontWing);
            memcpy(&this->rearWing, &buffer[offset], sizeof(this->rearWing));
            offset += sizeof(this->rearWing);
            memcpy(&this->onThrottle, &buffer[offset], sizeof(this->onThrottle));
            offset += sizeof(this->onThrottle);
            memcpy(&this->offThrottle, &buffer[offset], sizeof(this->offThrottle));
            offset += sizeof(this->offThrottle);
            memcpy(&this->frontCamber, &buffer[offset], sizeof(this->frontCamber));
            offset += sizeof(this->frontCamber);
            memcpy(&this->rearCamber, &buffer[offset], sizeof(this->rearCamber));
            offset += sizeof(this->rearCamber);
            memcpy(&this->frontToe, &buffer[offset], sizeof(this->frontToe));
            offset += sizeof(this->frontToe);
            memcpy(&this->rearToe, &buffer[offset], sizeof(this->rearToe));
            offset += sizeof(this->rearToe);
            memcpy(&this->frontSuspension, &buffer[offset], sizeof(this->frontSuspension));
            offset += sizeof(this->frontSuspension);
            memcpy(&this->rearSuspension, &buffer[offset], sizeof(this->rearSuspension));
            offset += sizeof(this->rearSuspension);
            memcpy(&this->frontAntiRollBar, &buffer[offset], sizeof(this->frontAntiRollBar));
            offset += sizeof(this->frontAntiRollBar);
            memcpy(&this->rearAntiRollBar, &buffer[offset], sizeof(this->rearAntiRollBar));
            offset += sizeof(this->rearAntiRollBar);
            memcpy(&this->frontSuspensionHeight, &buffer[offset], sizeof(this->frontSuspensionHeight));
            offset += sizeof(this->frontSuspensionHeight);
            memcpy(&this->rearSuspensionHeight, &buffer[offset], sizeof(this->rearSuspensionHeight));
            offset += sizeof(this->rearSuspensionHeight);
            memcpy(&this->brakePressure, &buffer[offset], sizeof(this->brakePressure));
            offset += sizeof(this->brakePressure);
            memcpy(&this->brakeBias, &buffer[offset], sizeof(this->brakeBias));
            offset += sizeof(this->brakeBias);
            memcpy(&this->rearLeftTyrePressure, &buffer[offset], sizeof(this->rearLeftTyrePressure));
            offset += sizeof(this->rearLeftTyrePressure);
            memcpy(&this->rearRightTyrePressure, &buffer[offset], sizeof(this->rearRightTyrePressure));
            offset += sizeof(this->rearRightTyrePressure);
            memcpy(&this->frontLeftTyrePressure, &buffer[offset], sizeof(this->frontLeftTyrePressure));
            offset += sizeof(this->frontLeftTyrePressure);
            memcpy(&this->frontRightTyrePressure, &buffer[offset], sizeof(this->frontRightTyrePressure));
            offset += sizeof(this->frontRightTyrePressure);
            memcpy(&this->ballast, &buffer[offset], sizeof(this->ballast));
            offset += sizeof(this->ballast);
            memcpy(&this->fuelLoad, &buffer[offset], sizeof(this->fuelLoad));
            offset += sizeof(this->fuelLoad);
            return offset;
        }
    };

    struct SPacketCarSetupData
    {
        SPacketHeader header; // Header
        SCarSetupData carSetupData[sMaxNumCarsInUDPData];
        float nextFrontWingValue; // Value of front wing after next pit stop - player only

        unsigned long get(char *buffer)
        {
            unsigned long offset = this->header.get(buffer);
            for (int i = 0; i < sMaxNumCarsInUDPData; i++)
            {
                offset = this->carSetupData[i].get(buffer, offset);
            }

            memcpy(&this->nextFrontWingValue, &buffer[offset], sizeof(this->nextFrontWingValue));
            offset += sizeof(this->nextFrontWingValue);

            return offset;
        }
    };
}