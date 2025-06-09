#pragma once

#include "constants.h"
#include "header.h"

#include <cstdint>
#include <memory>

namespace F125
{

       struct SCarTelemetryData
       {
              uint16_t speed;                     // Speed of car in kilometres per hour
              float throttle;                     // Amount of throttle applied (0.0 to 1.0)
              float steer;                        // Steering (-1.0 (full lock left) to 1.0 (full lock right))
              float brake;                        // Amount of brake applied (0.0 to 1.0)
              uint8_t clutch;                     // Amount of clutch applied (0 to 100)
              int8_t gear;                        // Gear selected (1-8, N=0, R=-1)
              uint16_t engineRPM;                 // Engine RPM
              uint8_t drs;                        // 0 = off, 1 = on
              uint8_t revLightsPercent;           // Rev lights indicator (percentage)
              uint16_t revLightsBitValue;         // Rev lights (bit 0 = leftmost LED, bit 14 = rightmost LED)
              uint16_t brakesTemperature[4];      // Brakes temperature (celsius)
              uint8_t tyresSurfaceTemperature[4]; // Tyres surface temperature (celsius)
              uint8_t tyresInnerTemperature[4];   // Tyres inner temperature (celsius)
              uint16_t engineTemperature;         // Engine temperature (celsius)
              float tyresPressure[4];             // Tyres pressure (PSI)
              uint8_t surfaceType[4];             // Driving surface, see appendices

              unsigned long get(char *buffer, unsigned long offset)
              {
                     memcpy(&this->speed, &buffer[offset],
                            sizeof(this->speed));
                     offset = offset + sizeof(this->speed);

                     memcpy(&this->throttle, &buffer[offset],
                            sizeof(this->throttle));
                     offset = offset + sizeof(this->throttle);

                     memcpy(&this->steer, &buffer[offset],
                            sizeof(this->steer));
                     offset = offset + sizeof(this->steer);

                     memcpy(&this->brake, &buffer[offset],
                            sizeof(this->brake));
                     offset = offset + sizeof(this->brake);

                     memcpy(&this->clutch, &buffer[offset],
                            sizeof(this->clutch));
                     offset = offset + sizeof(this->clutch);

                     memcpy(&this->gear, &buffer[offset],
                            sizeof(this->gear));
                     offset = offset + sizeof(this->gear);

                     memcpy(&this->engineRPM, &buffer[offset],
                            sizeof(this->engineRPM));
                     offset = offset + sizeof(this->engineRPM);

                     memcpy(&this->drs, &buffer[offset],
                            sizeof(this->drs));
                     offset = offset + sizeof(this->drs);

                     memcpy(&this->revLightsPercent, &buffer[offset],
                            sizeof(this->revLightsPercent));
                     offset = offset + sizeof(this->revLightsPercent);

                     memcpy(&this->revLightsBitValue, &buffer[offset],
                            sizeof(this->revLightsBitValue));
                     offset = offset + sizeof(this->revLightsBitValue);

                     for (int i = 0; i < 4; i++)
                     {
                            memcpy(&this->brakesTemperature[i], &buffer[offset],
                                   sizeof(this->brakesTemperature[i]));
                            offset = offset + sizeof(this->brakesTemperature[i]);
                     }

                     for (int i = 0; i < 4; i++)
                     {
                            memcpy(&this->tyresSurfaceTemperature[i], &buffer[offset],
                                   sizeof(this->tyresSurfaceTemperature[i]));
                            offset = offset + sizeof(this->tyresSurfaceTemperature[i]);
                     }

                     for (int i = 0; i < 4; i++)
                     {
                            memcpy(&this->tyresInnerTemperature[i], &buffer[offset],
                                   sizeof(this->tyresInnerTemperature[i]));
                            offset = offset + sizeof(this->tyresInnerTemperature[i]);
                     }

                     memcpy(&this->engineTemperature, &buffer[offset],
                            sizeof(this->engineTemperature));
                     offset = offset + sizeof(this->engineTemperature);

                     for (int i = 0; i < 4; i++)
                     {
                            memcpy(&this->tyresPressure[i], &buffer[offset],
                                   sizeof(this->tyresPressure[i]));
                            offset = offset + sizeof(this->tyresPressure[i]);
                     }

                     for (int i = 0; i < 4; i++)
                     {
                            memcpy(&this->surfaceType[i], &buffer[offset],
                                   sizeof(this->surfaceType[i]));
                            offset = offset + sizeof(this->surfaceType[i]);
                     }

                     return offset;
              }
       };

       struct SPacketCarTelemetryData
       {
              SPacketHeader header; // Header
              SCarTelemetryData carTelemetryData[22];

              unsigned long get(char *buffer)
              {
                     unsigned long offset = this->header.get(buffer);
                     for (int i = 0; i < 22; i++)
                     {
                            offset = this->carTelemetryData[i].get(buffer, offset);
                     }

                     return offset;
              }
       };
}