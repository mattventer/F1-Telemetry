#pragma once

#include "header.h"

#include <cstdint>

// Session Packet
// Frequency: 2 per second
// Size: 632 bytes
// Version: 1
struct SMarshalZone
{
       float zoneStart; // Fraction (0..1) of way through the lap the marshal zone starts
       int8_t zoneFlag; // -1 = invalid/unknown, 0 = none, 1 = green, 2 = blue, 3 = yellow, 4 = red

       unsigned long get(char *buffer, unsigned long offset)
       {
              memcpy(&this->zoneStart, &buffer[offset], sizeof(zoneStart));
              offset += sizeof(zoneStart);
              memcpy(&this->zoneFlag, &buffer[offset], sizeof(zoneFlag));
              offset += sizeof(zoneFlag);

              return offset;
       }
};

struct SWeatherForecastSample
{
       uint8_t sessionType;           // 0 = unknown, 1 = P1, 2 = P2, 3 = P3, 4 = Short P, 5 = Q1, 6 = Q2, 7 = Q3, 8 = Short Q, 9 = OSQ, 10 = R, 11 = R212 = R3, 13 = Time Trial
       uint8_t timeOffset;            // Time in minutes the forecast is for
       uint8_t weather;               // Weather - 0 = clear, 1 = light cloud, 2 = overcast, 3 = light rain, 4 = heavy rain, 5 = storm
       int8_t trackTemperature;       // Track temp. in degrees Celsius
       int8_t trackTemperatureChange; // Track temp. change – 0 = up, 1 = down, 2 = no change
       int8_t airTemperature;         // Air temp. in degrees celsius
       int8_t airTemperatureChange;   // Air temp. change – 0 = up, 1 = down, 2 = no change
       uint8_t rainPercentage;        // Rain percentage (0-100)

       unsigned long get(char *buffer, unsigned long offset)
       {
              memcpy(&this->sessionType, &buffer[offset], sizeof(sessionType));
              offset += sizeof(sessionType);

              memcpy(&this->timeOffset, &buffer[offset], sizeof(timeOffset));
              offset += sizeof(timeOffset);

              memcpy(&this->weather, &buffer[offset], sizeof(weather));
              offset += sizeof(weather);

              memcpy(&this->trackTemperature, &buffer[offset],
                     sizeof(trackTemperature));
              offset += sizeof(trackTemperature);

              memcpy(&this->trackTemperatureChange, &buffer[offset],
                     sizeof(trackTemperatureChange));
              offset += sizeof(trackTemperatureChange);

              memcpy(&this->airTemperature, &buffer[offset],
                     sizeof(airTemperature));
              offset += sizeof(airTemperature);

              memcpy(&this->airTemperatureChange, &buffer[offset],
                     sizeof(airTemperatureChange));
              offset += sizeof(airTemperatureChange);

              memcpy(&this->rainPercentage, &buffer[offset],
                     sizeof(rainPercentage));
              offset += sizeof(rainPercentage);

              return offset;
       }
};

struct SPacketSessionData
{
       SPacketHeader header;                              // Header
       uint8_t weather;                                   // Weather - 0 = clear, 1 = light cloud, 2 = overcast, 3 = light rain, 4 = heavy rain, 5 = storm
       int8_t trackTemperature;                           // Track temp. in degrees celsius
       int8_t airTemperature;                             // Air temp. in degrees celsius
       uint8_t totalLaps;                                 // Total number of laps in this race
       uint16_t trackLength;                              // Track length in metres
       uint8_t sessionType;                               // 0 = unknown, 1 = P1, 2 = P2, 3 = P3, 4 = Short P, 5 = Q1, 6 = Q2, 7 = Q3, 8 = Short Q, 9 = OSQ, 10 = R, 11 = R2, 12 = R3, 13 = Time Trial
       int8_t trackId;                                    // -1 for unknown, see appendix
       uint8_t formula;                                   // Formula, 0 = F1 Modern, 1 = F1 Classic, 2 = F2, 3 = F1 Generic, 4 = Beta, 5 = Supercars, 6 = Esports, 7 = F2 2021
       uint16_t sessionTimeLeft;                          // Time left in session in seconds
       uint16_t sessionDuration;                          // Session duration in seconds
       uint8_t pitSpeedLimit;                             // Pit speed limit in kilometres per hour
       uint8_t gamePaused;                                // Whether the game is paused – network game only
       uint8_t isSpectating;                              // Whether the player is spectating
       uint8_t spectatorCarIndex;                         // Index of the car being spectated
       uint8_t sliProNativeSupport;                       // SLI Pro support, 0 = inactive, 1 = active
       uint8_t numMarshalZones;                           // Number of marshal zones to follow
       SMarshalZone marshalZones[21];                     // List of marshal zones – max 21
       uint8_t safetyCarStatus;                           // 0 = no safety car, 1 = full. 2 = virtual, 3 = formation lap
       uint8_t networkGame;                               // 0 = offline, 1 = online
       uint8_t numWeatherForecastSamples;                 // Number of weather samples to follow
       SWeatherForecastSample weatherForecastSamples[56]; // Array of weather forecast samples
       uint8_t forecastAccuracy;                          // 0 = Perfect, 1 = Approximate
       uint8_t aiDifficulty;                              // AI Difficulty rating – 0-110
       uint32_t seasonLinkIdentifier;                     // Identifier for season - persists across saves
       uint32_t weekendLinkIdentifier;                    // Identifier for weekend - persists across saves
       uint32_t sessionLinkIdentifier;                    // Identifier for session - persists across saves
       uint8_t pitStopWindowIdealLap;                     // Ideal lap to pit on for current strategy (player)
       uint8_t pitStopWindowLatestLap;                    // Latest lap to pit on for current strategy (player)
       uint8_t pitStopRejoinPosition;                     // Predicted position to rejoin at (player)
       uint8_t steeringAssist;                            // 0 = off, 1 = on
       uint8_t brakingAssist;                             // 0 = off, 1 = low, 2 = medium, 3 = high
       uint8_t gearboxAssist;                             // 1 = manual, 2 = manual & suggested gear, 3 = auto
       uint8_t pitAssist;                                 // 0 = off, 1 = on
       uint8_t pitReleaseAssist;                          // 0 = off, 1 = on
       uint8_t ERSAssist;                                 // 0 = off, 1 = on
       uint8_t DRSAssist;                                 // 0 = off, 1 = on
       uint8_t dynamicRacingLine;                         // 0 = off, 1 = corners only, 2 = full
       uint8_t dynamicRacingLineType;                     // 0 = 2D, 1 = 3D
       uint8_t gameMode;                                  // Game mode id - see appendix
       uint8_t ruleSet;                                   // Ruleset - see appendix
       uint32_t timeOfDay;                                // Local time of day - minutes since midnight
       uint8_t sessionLength;                             // 0 = None, 2 = Very Short, 3 = Short, 4 = Medium, 5 = Medium Long, 6 = Long, 7 = Full
       uint8_t speedUnitsLeadPlayer;
       uint8_t temperatureUnitsLeadPlayer;
       uint8_t speedUnitsSecondaryPlayer;
       uint8_t temperatureUnitsSecondPlayer;
       uint8_t numSafetyCarPeriods;
       uint8_t numVirtualSafetyCarPeriods;
       uint8_t numRedFlagPeriods;

       void get(char *buffer)
       {
              unsigned long offset = this->header.get(buffer);

              memcpy(&this->weather, &buffer[offset], sizeof(weather));
              offset += sizeof(weather);

              memcpy(&this->trackTemperature, &buffer[offset],
                     sizeof(trackTemperature));
              offset += sizeof(trackTemperature);

              memcpy(&this->airTemperature, &buffer[offset],
                     sizeof(airTemperature));
              offset += sizeof(airTemperature);

              memcpy(&this->totalLaps, &buffer[offset], sizeof(totalLaps));
              offset += sizeof(totalLaps);

              memcpy(&this->trackLength, &buffer[offset], sizeof(trackLength));
              offset += sizeof(trackLength);

              memcpy(&this->sessionType, &buffer[offset], sizeof(sessionType));
              offset += sizeof(sessionType);

              memcpy(&this->trackId, &buffer[offset], sizeof(trackId));
              offset += sizeof(trackId);

              memcpy(&this->formula, &buffer[offset], sizeof(formula));
              offset += sizeof(formula);

              memcpy(&this->sessionTimeLeft, &buffer[offset],
                     sizeof(sessionTimeLeft));
              offset += sizeof(sessionTimeLeft);

              memcpy(&this->sessionDuration, &buffer[offset],
                     sizeof(sessionDuration));
              offset += sizeof(sessionDuration);

              memcpy(&this->pitSpeedLimit, &buffer[offset],
                     sizeof(pitSpeedLimit));
              offset += sizeof(pitSpeedLimit);

              memcpy(&this->gamePaused, &buffer[offset], sizeof(gamePaused));
              offset += sizeof(gamePaused);

              memcpy(&this->isSpectating, &buffer[offset], sizeof(isSpectating));
              offset += sizeof(isSpectating);

              memcpy(&this->spectatorCarIndex, &buffer[offset],
                     sizeof(spectatorCarIndex));
              offset += sizeof(spectatorCarIndex);

              memcpy(&this->sliProNativeSupport, &buffer[offset],
                     sizeof(sliProNativeSupport));
              offset += sizeof(sliProNativeSupport);

              memcpy(&this->numMarshalZones, &buffer[offset],
                     sizeof(numMarshalZones));
              offset += sizeof(numMarshalZones);
              for (int i = 0; i < this->numMarshalZones; i++)
              {
                     offset = this->marshalZones[i].get(buffer, offset);
              }

              memcpy(&this->safetyCarStatus, &buffer[offset],
                     sizeof(safetyCarStatus));
              offset += sizeof(safetyCarStatus);

              memcpy(&this->networkGame, &buffer[offset], sizeof(networkGame));
              offset += sizeof(networkGame);

              memcpy(&this->numWeatherForecastSamples, &buffer[offset],
                     sizeof(numWeatherForecastSamples));
              offset += sizeof(numWeatherForecastSamples);
              for (int i = 0; i < this->numWeatherForecastSamples; i++)
              {
                     offset = this->weatherForecastSamples[i].get(buffer, offset);
              }

              memcpy(&this->forecastAccuracy, &buffer[offset],
                     sizeof(forecastAccuracy));
              offset += sizeof(forecastAccuracy);

              memcpy(&this->aiDifficulty, &buffer[offset], sizeof(aiDifficulty));
              offset += sizeof(aiDifficulty);

              memcpy(&this->seasonLinkIdentifier, &buffer[offset],
                     sizeof(seasonLinkIdentifier));
              offset += sizeof(seasonLinkIdentifier);

              memcpy(&this->weekendLinkIdentifier, &buffer[offset],
                     sizeof(weekendLinkIdentifier));
              offset += sizeof(weekendLinkIdentifier);

              memcpy(&this->sessionLinkIdentifier, &buffer[offset],
                     sizeof(sessionLinkIdentifier));
              offset += sizeof(sessionLinkIdentifier);

              memcpy(&this->pitStopWindowIdealLap, &buffer[offset],
                     sizeof(pitStopWindowIdealLap));
              offset += sizeof(pitStopWindowIdealLap);

              memcpy(&this->pitStopWindowLatestLap, &buffer[offset],
                     sizeof(pitStopWindowLatestLap));
              offset += sizeof(pitStopWindowLatestLap);

              memcpy(&this->pitStopRejoinPosition, &buffer[offset],
                     sizeof(pitStopRejoinPosition));
              offset += sizeof(pitStopRejoinPosition);

              memcpy(&this->steeringAssist, &buffer[offset],
                     sizeof(steeringAssist));
              offset += sizeof(steeringAssist);

              memcpy(&this->brakingAssist, &buffer[offset],
                     sizeof(brakingAssist));
              offset += sizeof(brakingAssist);

              memcpy(&this->gearboxAssist, &buffer[offset],
                     sizeof(gearboxAssist));
              offset += sizeof(gearboxAssist);

              memcpy(&this->pitAssist, &buffer[offset], sizeof(pitAssist));
              offset += sizeof(pitAssist);

              memcpy(&this->pitReleaseAssist, &buffer[offset],
                     sizeof(pitReleaseAssist));
              offset += sizeof(pitReleaseAssist);

              memcpy(&this->ERSAssist, &buffer[offset], sizeof(ERSAssist));
              offset += sizeof(ERSAssist);

              memcpy(&this->DRSAssist, &buffer[offset], sizeof(DRSAssist));
              offset += sizeof(DRSAssist);

              memcpy(&this->dynamicRacingLine, &buffer[offset],
                     sizeof(dynamicRacingLine));
              offset += sizeof(dynamicRacingLine);

              memcpy(&this->dynamicRacingLineType, &buffer[offset],
                     sizeof(dynamicRacingLineType));
              offset += sizeof(dynamicRacingLineType);

              memcpy(&this->gameMode, &buffer[offset],
                     sizeof(gameMode));
              offset += sizeof(gameMode);

              memcpy(&this->ruleSet, &buffer[offset],
                     sizeof(ruleSet));
              offset += sizeof(ruleSet);

              memcpy(&this->timeOfDay, &buffer[offset],
                     sizeof(timeOfDay));
              offset += sizeof(timeOfDay);

              memcpy(&this->sessionLength, &buffer[offset],
                     sizeof(sessionLength));
              offset += sizeof(sessionLength);

              memcpy(&this->speedUnitsLeadPlayer, &buffer[offset],
                     sizeof(speedUnitsLeadPlayer));
              offset += sizeof(speedUnitsLeadPlayer);

              memcpy(&this->temperatureUnitsLeadPlayer, &buffer[offset],
                     sizeof(temperatureUnitsLeadPlayer));
              offset += sizeof(temperatureUnitsLeadPlayer);

              memcpy(&this->speedUnitsSecondaryPlayer, &buffer[offset],
                     sizeof(speedUnitsSecondaryPlayer));
              offset += sizeof(speedUnitsSecondaryPlayer);

              memcpy(&this->temperatureUnitsSecondPlayer, &buffer[offset],
                     sizeof(temperatureUnitsSecondPlayer));
              offset += sizeof(temperatureUnitsSecondPlayer);

              memcpy(&this->numSafetyCarPeriods, &buffer[offset],
                     sizeof(numSafetyCarPeriods));
              offset += sizeof(numSafetyCarPeriods);

              memcpy(&this->numVirtualSafetyCarPeriods, &buffer[offset],
                     sizeof(numVirtualSafetyCarPeriods));
              offset += sizeof(numVirtualSafetyCarPeriods);

              memcpy(&this->numRedFlagPeriods, &buffer[offset],
                     sizeof(numRedFlagPeriods));
              offset += sizeof(numRedFlagPeriods);
       }
};