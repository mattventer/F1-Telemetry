#pragma once

#include "header.h"

#include <cstdint>
#include <memory>

namespace F125
{

       struct SCarStatusData
       {
              uint8_t tractionControl;
              uint8_t antiLockBrakes;
              uint8_t fuelMix;
              uint8_t frontBrakeBias;
              uint8_t pitLimiterStatus;
              float fuelInTank;
              float fuelCapacity;
              float fuelRemainingLaps;
              uint16_t maxRPM;
              uint16_t idleRPM;
              uint8_t maxGears;
              uint8_t drsAllowed;
              uint16_t drsActivationDistance;
              uint8_t actualTyreCompound;
              uint8_t visualTyreCompound;
              uint8_t tyreAgeLaps;
              int8_t vehicleFiaFlags;
              float enginePowerICE;
              float enginePowerMGUK;
              float ersStoreEnergy;
              uint8_t ersDeployMode;
              float ersHarvestedThisLapMGUK;
              float ersHarvestedThisLapMGUH;
              float ersDeployedThisLap;
              uint8_t networkPaused;

              unsigned long get(char *buffer, unsigned long offset)
              {
                     memcpy(&this->tractionControl, &buffer[offset],
                            sizeof(this->tractionControl));
                     offset = offset + sizeof(this->tractionControl);

                     memcpy(&this->antiLockBrakes, &buffer[offset],
                            sizeof(this->antiLockBrakes));
                     offset = offset + sizeof(this->antiLockBrakes);

                     memcpy(&this->fuelMix, &buffer[offset],
                            sizeof(this->fuelMix));
                     offset = offset + sizeof(this->fuelMix);

                     memcpy(&this->frontBrakeBias, &buffer[offset],
                            sizeof(this->frontBrakeBias));
                     offset = offset + sizeof(this->frontBrakeBias);

                     memcpy(&this->pitLimiterStatus, &buffer[offset],
                            sizeof(this->pitLimiterStatus));
                     offset = offset + sizeof(this->pitLimiterStatus);

                     memcpy(&this->fuelInTank, &buffer[offset],
                            sizeof(this->fuelInTank));
                     offset = offset + sizeof(this->fuelInTank);

                     memcpy(&this->fuelCapacity, &buffer[offset],
                            sizeof(this->fuelCapacity));
                     offset = offset + sizeof(this->fuelCapacity);

                     memcpy(&this->fuelRemainingLaps, &buffer[offset],
                            sizeof(this->fuelRemainingLaps));
                     offset = offset + sizeof(this->fuelRemainingLaps);

                     memcpy(&this->maxRPM, &buffer[offset],
                            sizeof(this->maxRPM));
                     offset = offset + sizeof(this->maxRPM);

                     memcpy(&this->idleRPM, &buffer[offset],
                            sizeof(this->idleRPM));
                     offset = offset + sizeof(this->idleRPM);

                     memcpy(&this->maxGears, &buffer[offset],
                            sizeof(this->maxGears));
                     offset = offset + sizeof(this->maxGears);

                     memcpy(&this->drsAllowed, &buffer[offset],
                            sizeof(this->drsAllowed));
                     offset = offset + sizeof(this->drsAllowed);

                     memcpy(&this->drsActivationDistance, &buffer[offset],
                            sizeof(this->drsActivationDistance));
                     offset = offset + sizeof(this->drsActivationDistance);

                     memcpy(&this->actualTyreCompound, &buffer[offset],
                            sizeof(this->actualTyreCompound));
                     offset = offset + sizeof(this->actualTyreCompound);

                     memcpy(&this->visualTyreCompound, &buffer[offset],
                            sizeof(this->visualTyreCompound));
                     offset = offset + sizeof(this->visualTyreCompound);

                     memcpy(&this->tyreAgeLaps, &buffer[offset],
                            sizeof(this->tyreAgeLaps));
                     offset = offset + sizeof(this->tyreAgeLaps);

                     memcpy(&this->vehicleFiaFlags, &buffer[offset],
                            sizeof(this->vehicleFiaFlags));
                     offset = offset + sizeof(this->vehicleFiaFlags);

                     memcpy(&this->enginePowerICE, &buffer[offset],
                            sizeof(this->enginePowerICE));
                     offset = offset + sizeof(this->enginePowerICE);

                     memcpy(&this->enginePowerMGUK, &buffer[offset],
                            sizeof(this->enginePowerMGUK));
                     offset = offset + sizeof(this->enginePowerMGUK);

                     memcpy(&this->ersStoreEnergy, &buffer[offset],
                            sizeof(this->ersStoreEnergy));
                     offset = offset + sizeof(this->ersStoreEnergy);

                     memcpy(&this->ersDeployMode, &buffer[offset],
                            sizeof(this->ersDeployMode));
                     offset = offset + sizeof(this->ersDeployMode);

                     memcpy(&this->ersHarvestedThisLapMGUK, &buffer[offset],
                            sizeof(this->ersHarvestedThisLapMGUK));
                     offset = offset + sizeof(this->ersHarvestedThisLapMGUK);

                     memcpy(&this->ersHarvestedThisLapMGUH, &buffer[offset],
                            sizeof(this->ersHarvestedThisLapMGUH));
                     offset = offset + sizeof(this->ersHarvestedThisLapMGUH);

                     memcpy(&this->ersDeployedThisLap, &buffer[offset],
                            sizeof(this->ersDeployedThisLap));
                     offset = offset + sizeof(this->ersDeployedThisLap);

                     memcpy(&this->networkPaused, &buffer[offset],
                            sizeof(this->networkPaused));
                     offset = offset + sizeof(this->networkPaused);

                     return offset;
              }
       };

       struct SPacketCarStatusData
       {
              SPacketHeader header; // Header
              SCarStatusData carStatusData[22];

              unsigned long get(char *buffer)
              {
                     unsigned long offset = this->header.get(buffer);
                     for (int i = 0; i < 22; i++)
                     {
                            offset = this->carStatusData[i].get(buffer, offset);
                     }

                     return offset;
              }
       };

}