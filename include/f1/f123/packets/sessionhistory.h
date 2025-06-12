#pragma once

#include "header.h"

#include <cstdint>
#include <memory>

namespace F123
{

    struct SLapHistoryData
    {
        uint32_t lapTimeInMS;       // Lap time in milliseconds
        uint16_t sector1TimeInMS;   // Sector 1 time in milliseconds
        uint8_t sector1TimeMinutes; // Sector 1 whole minute part
        uint16_t sector2TimeInMS;   // Sector 2 time in milliseconds
        uint8_t sector2TimeMinutes; // Sector 2 whole minute part
        uint16_t sector3TimeInMS;   // Sector 3 time in milliseconds
        uint8_t sector3TimeMinutes; // Sector 3 whole minute part
        uint8_t lapValidBitFlags;   // 0x01 bit set-lap valid, 0x02 bit set-sector 1 valid
        // 0x04 bit set-sector 2 valid, 0x08_t bit set-sector 3 valid

        unsigned long get(char *buffer, unsigned long offset)
        {
            memcpy(&this->lapTimeInMS, &buffer[offset], sizeof(lapTimeInMS));
            offset += sizeof(lapTimeInMS);

            memcpy(&this->sector1TimeInMS, &buffer[offset], sizeof(sector1TimeInMS));
            offset += sizeof(sector1TimeInMS);

            memcpy(&this->sector1TimeMinutes, &buffer[offset], sizeof(sector1TimeMinutes));
            offset += sizeof(sector1TimeMinutes);

            memcpy(&this->sector2TimeInMS, &buffer[offset],
                   sizeof(sector2TimeInMS));
            offset += sizeof(sector2TimeInMS);

            memcpy(&this->sector2TimeMinutes, &buffer[offset],
                   sizeof(sector2TimeMinutes));
            offset += sizeof(sector2TimeMinutes);

            memcpy(&this->sector3TimeInMS, &buffer[offset],
                   sizeof(sector3TimeInMS));
            offset += sizeof(sector3TimeInMS);

            memcpy(&this->sector3TimeMinutes, &buffer[offset],
                   sizeof(sector3TimeMinutes));
            offset += sizeof(sector3TimeMinutes);

            memcpy(&this->lapValidBitFlags, &buffer[offset],
                   sizeof(lapValidBitFlags));
            offset += sizeof(lapValidBitFlags);

            return offset;
        }
    };

    struct STyreStintHistoryData
    {
        uint8_t endLap;             // Lap the tyre usage ends on (255 of current tyre)
        uint8_t tyreActualCompound; // Actual tyres used by this driver
        uint8_t tyreVisualCompound; // Visual tyres used by this driver

        unsigned long get(char *buffer, unsigned long offset)
        {
            memcpy(&this->endLap, &buffer[offset], sizeof(endLap));
            offset += sizeof(endLap);

            memcpy(&this->tyreActualCompound, &buffer[offset], sizeof(tyreActualCompound));
            offset += sizeof(tyreActualCompound);

            memcpy(&this->tyreVisualCompound, &buffer[offset], sizeof(tyreVisualCompound));
            offset += sizeof(tyreVisualCompound);

            return offset;
        }
    };

    struct SPacketSessionHistoryData
    {
        SPacketHeader header;                // Header
        uint8_t carIdx;                      // Index of the car this lap data relates to
        uint8_t numLaps;                     // Num laps in the data (including current partial lap)
        uint8_t numTyreStints;               // Number of tyre stints in the data
        uint8_t bestLapTimeLapNum;           // Lap the best lap time was achieved on
        uint8_t bestSector1LapNum;           // Lap the best Sector 1 time was achieved on
        uint8_t bestSector2LapNum;           // Lap the best Sector 2 time was achieved on
        uint8_t bestSector3LapNum;           // Lap the best Sector 3 time was achieved on
        SLapHistoryData lapHistoryData[100]; // 100 laps of data max
        STyreStintHistoryData tyreStintsHistoryData[8];

        void get(char *buffer)
        {
            unsigned long offset = this->header.get(buffer);

            memcpy(&this->carIdx, &buffer[offset], sizeof(carIdx));
            offset += sizeof(carIdx);

            memcpy(&this->numLaps, &buffer[offset], sizeof(numLaps));
            offset += sizeof(numLaps);

            memcpy(&this->numTyreStints, &buffer[offset],
                   sizeof(numTyreStints));
            offset += sizeof(numTyreStints);

            memcpy(&this->bestLapTimeLapNum, &buffer[offset], sizeof(bestLapTimeLapNum));
            offset += sizeof(bestLapTimeLapNum);

            memcpy(&this->bestSector1LapNum, &buffer[offset], sizeof(bestSector1LapNum));
            offset += sizeof(bestSector1LapNum);

            memcpy(&this->bestSector2LapNum, &buffer[offset], sizeof(bestSector2LapNum));
            offset += sizeof(bestSector2LapNum);

            memcpy(&this->bestSector3LapNum, &buffer[offset], sizeof(bestSector3LapNum));
            offset += sizeof(bestSector3LapNum);

            for (int i = 0; i < this->numLaps; i++)
            {
                offset = this->lapHistoryData[i].get(buffer, offset);
            }

            for (int i = 0; i < this->numTyreStints; i++)
            {
                offset = this->tyreStintsHistoryData[i].get(buffer, offset);
            }
        }
    };

}