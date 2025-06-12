#pragma once

#include "header.h"

#include <cstdint>
#include <memory>

namespace F125
{

       struct SLapData
       {
              uint32_t lastLapTimeInMS;
              uint32_t currentLapTimeInMS;
              uint16_t sector1TimeInMS;
              uint8_t sector1TimeMinutes;
              uint16_t sector2TimeInMS;
              uint8_t sector2TimeMinutes;
              uint16_t deltaToCarInFrontMSPart;     // Time delta to car in front milliseconds part
              uint8_t deltaToCarInFrontMinutesPart; // Time delta to car in front whole minute part
              uint16_t deltaToRaceLeaderMSPart;     // Time delta to race leader milliseconds part
              uint8_t deltaToRaceLeaderMinutesPart; // Time delta to race leader whole minute part
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
              float speedTrapFastestSpeed; // Fastest speed through speed trap for this car in kmph
              uint8_t speedTrapFastestLap; // Lap no the fastest speed was achieved, 255 = not set

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

                     memcpy(&this->deltaToCarInFrontMSPart, &buffer[offset],
                            sizeof(this->deltaToCarInFrontMSPart));
                     offset += sizeof(this->deltaToCarInFrontMSPart);

                     memcpy(&this->deltaToCarInFrontMinutesPart, &buffer[offset],
                            sizeof(this->deltaToCarInFrontMinutesPart));
                     offset += sizeof(this->deltaToCarInFrontMinutesPart);

                     memcpy(&this->deltaToRaceLeaderMSPart, &buffer[offset],
                            sizeof(this->deltaToRaceLeaderMSPart));
                     offset += sizeof(this->deltaToRaceLeaderMSPart);

                     memcpy(&this->deltaToRaceLeaderMinutesPart, &buffer[offset],
                            sizeof(this->deltaToRaceLeaderMinutesPart));
                     offset += sizeof(this->deltaToRaceLeaderMinutesPart);

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

                     memcpy(&this->speedTrapFastestSpeed, &buffer[offset],
                            sizeof(this->speedTrapFastestSpeed));
                     offset += sizeof(this->speedTrapFastestSpeed);

                     memcpy(&this->speedTrapFastestLap, &buffer[offset],
                            sizeof(this->speedTrapFastestLap));
                     offset += sizeof(this->speedTrapFastestLap);

                     return offset;
              }
       };

       struct SPacketLapData
       {
              SPacketHeader header; // Header
              SLapData lapData[sMaxNumCarsInUDPData];
              uint8_t timeTrialPBCarIdx;    // Index of Personal Best car in time trial (255 if invalid)
              uint8_t timeTrialRivalCarIdx; // Index of Rival car in time trial (255 if invalid)

              unsigned long get(char *buffer)
              {
                     unsigned long offset = this->header.get(buffer);
                     for (int i = 0; i < sMaxNumCarsInUDPData; i++)
                     {
                            offset = this->lapData[i].get(buffer, offset);
                     }

                     memcpy(&this->timeTrialPBCarIdx, &buffer[offset],
                            sizeof(this->timeTrialPBCarIdx));
                     offset += sizeof(this->timeTrialPBCarIdx);

                     memcpy(&this->timeTrialRivalCarIdx, &buffer[offset],
                            sizeof(this->timeTrialRivalCarIdx));
                     offset += sizeof(this->timeTrialRivalCarIdx);

                     return offset;
              }
       };
}