#pragma once

#include "header.h"

#include <cstdint>
#include <memory>

namespace F123
{

       struct SLapData
       {
              uint32_t lastLapTimeInMS;
              uint32_t currentLapTimeInMS;
              uint16_t sector1TimeInMS;
              uint8_t sector1TimeMinutes;
              uint16_t sector2TimeInMS;
              uint8_t sector2TimeMinutes;
              uint16_t deltaToCarInFrontInMS;
              uint16_t deltaToRaceLeaderInMS;
              float lapDistance;
              float totalDistance;
              float safetyCarDelta;
              uint8_t carPosition; // Car race position
              uint8_t currentLapNum;
              uint8_t pitStatus;
              uint8_t numPitStops;
              uint8_t sector;
              uint8_t currentLapInvalid; // 0 = valid, 1 = invalid
              uint8_t penalties;
              uint8_t totalWarnings;
              uint8_t cornerCuttingWarnings;
              uint8_t numUnservedDriveThroughPens;
              uint8_t numUnservedStopGoPens;
              uint8_t gridPosition;
              uint8_t driverStatus;
              uint8_t resultStatus;
              uint8_t pitLaneTimerActive;
              uint16_t pitLaneTimeInLaneInMS;
              uint16_t pitStopTimerInMS;
              uint8_t pitStopShouldServePen;

              unsigned long get(char *buffer, unsigned long offset)
              {
                     memcpy(&this->lastLapTimeInMS, &buffer[offset],
                            sizeof(this->lastLapTimeInMS));
                     offset += sizeof(this->lastLapTimeInMS);

                     memcpy(&this->currentLapTimeInMS, &buffer[offset],
                            sizeof(this->currentLapTimeInMS));
                     offset += sizeof(this->currentLapTimeInMS);

                     memcpy(&this->sector1TimeInMS, &buffer[offset],
                            sizeof(this->sector1TimeInMS));
                     offset += sizeof(this->sector1TimeInMS);

                     memcpy(&this->sector1TimeMinutes, &buffer[offset],
                            sizeof(this->sector1TimeMinutes));
                     offset += sizeof(this->sector1TimeMinutes);

                     memcpy(&this->sector2TimeInMS, &buffer[offset],
                            sizeof(this->sector2TimeInMS));
                     offset += sizeof(this->sector2TimeInMS);

                     memcpy(&this->sector2TimeMinutes, &buffer[offset],
                            sizeof(this->sector2TimeMinutes));
                     offset += sizeof(this->sector2TimeMinutes);

                     memcpy(&this->deltaToCarInFrontInMS, &buffer[offset],
                            sizeof(this->deltaToCarInFrontInMS));
                     offset += sizeof(this->deltaToCarInFrontInMS);

                     memcpy(&this->deltaToRaceLeaderInMS, &buffer[offset],
                            sizeof(this->deltaToRaceLeaderInMS));
                     offset += sizeof(this->deltaToRaceLeaderInMS);

                     memcpy(&this->lapDistance, &buffer[offset],
                            sizeof(this->lapDistance));
                     offset += sizeof(this->lapDistance);

                     memcpy(&this->totalDistance, &buffer[offset],
                            sizeof(this->totalDistance));
                     offset += sizeof(this->totalDistance);

                     memcpy(&this->safetyCarDelta, &buffer[offset],
                            sizeof(this->safetyCarDelta));
                     offset += sizeof(this->safetyCarDelta);

                     memcpy(&this->carPosition, &buffer[offset],
                            sizeof(this->carPosition));
                     offset += sizeof(this->carPosition);

                     memcpy(&this->currentLapNum, &buffer[offset],
                            sizeof(this->currentLapNum));
                     offset += sizeof(this->currentLapNum);

                     memcpy(&this->pitStatus, &buffer[offset],
                            sizeof(this->pitStatus));
                     offset += sizeof(this->pitStatus);

                     memcpy(&this->numPitStops, &buffer[offset],
                            sizeof(this->numPitStops));
                     offset += sizeof(this->numPitStops);

                     memcpy(&this->sector, &buffer[offset],
                            sizeof(this->sector));
                     offset += sizeof(this->sector);

                     memcpy(&this->currentLapInvalid, &buffer[offset],
                            sizeof(this->currentLapInvalid));
                     offset += sizeof(this->currentLapInvalid);

                     memcpy(&this->penalties, &buffer[offset],
                            sizeof(this->penalties));
                     offset += sizeof(this->penalties);

                     memcpy(&this->totalWarnings, &buffer[offset],
                            sizeof(this->totalWarnings));
                     offset += sizeof(this->totalWarnings);

                     memcpy(&this->cornerCuttingWarnings, &buffer[offset],
                            sizeof(this->cornerCuttingWarnings));
                     offset += sizeof(this->cornerCuttingWarnings);

                     memcpy(&this->numUnservedDriveThroughPens, &buffer[offset],
                            sizeof(this->numUnservedDriveThroughPens));
                     offset += sizeof(this->numUnservedDriveThroughPens);

                     memcpy(&this->numUnservedStopGoPens, &buffer[offset],
                            sizeof(this->numUnservedStopGoPens));
                     offset += sizeof(this->numUnservedStopGoPens);

                     memcpy(&this->gridPosition, &buffer[offset],
                            sizeof(this->gridPosition));
                     offset += sizeof(this->gridPosition);

                     memcpy(&this->driverStatus, &buffer[offset],
                            sizeof(this->driverStatus));
                     offset += sizeof(this->driverStatus);

                     memcpy(&this->resultStatus, &buffer[offset],
                            sizeof(this->resultStatus));
                     offset += sizeof(this->resultStatus);

                     memcpy(&this->pitLaneTimerActive, &buffer[offset],
                            sizeof(this->pitLaneTimerActive));
                     offset += sizeof(this->pitLaneTimerActive);

                     memcpy(&this->pitLaneTimeInLaneInMS, &buffer[offset],
                            sizeof(this->pitLaneTimeInLaneInMS));
                     offset += sizeof(this->pitLaneTimeInLaneInMS);

                     memcpy(&this->pitStopTimerInMS, &buffer[offset],
                            sizeof(this->pitStopTimerInMS));
                     offset += sizeof(this->pitStopTimerInMS);

                     memcpy(&this->pitStopShouldServePen, &buffer[offset],
                            sizeof(this->pitStopShouldServePen));
                     offset += sizeof(this->pitStopShouldServePen);

                     return offset;
              }
       };

       struct SPacketLapData
       {
              SPacketHeader header; // Header
              SLapData lapData[22];

              unsigned long get(char *buffer)
              {
                     unsigned long offset = this->header.get(buffer);
                     for (int i = 0; i < 22; i++)
                     {
                            offset = this->lapData[i].get(buffer, offset);
                     }

                     return offset;
              }
       };

}