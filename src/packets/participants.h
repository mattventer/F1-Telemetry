#pragma once

#include "../f122constants.h"
#include "header.h"

#include <iostream>
#include <memory>

using namespace F122;

struct SParticipantsData
{
    uint8_t aiControlled;    // Whether the vehicle is AI (1) or Human (0) controlled
    uint8_t driverId;        // Driver id - see appendix
    uint8_t networkId;       // Network id – unique identifier for network players
    uint8_t teamId;          // Team id - see appendix
    uint8_t myTeam;          // My team = 1, 0 otherwise
    uint8_t raceNumber;      // Race number of the car
    uint8_t nationality;     // Nationality of the driver
    char name[48];           // Name of participant in UTF-8 format – null terminated. Will be truncated with … (U+2026) if too long
    uint8_t yourTelemetry;   // The player's UDP setting,0 = restricted,1 = public
    uint8_t showOnlineNames; // The player's show online names setting, 0 = off, 1 = on
    uint8_t platform;        // 1 = Steam, 3 = PlayStation, 4 = Xbox, 6 = Origin, 255 = unknown

    unsigned long get(char *buffer, unsigned long offset)
    {
        memcpy(&this->aiControlled, &buffer[offset], sizeof(this->aiControlled));
        offset += sizeof(this->aiControlled);

        memcpy(&this->driverId, &buffer[offset], sizeof(this->driverId));
        offset += sizeof(this->driverId);

        memcpy(&this->networkId, &buffer[offset], sizeof(this->networkId));
        offset += sizeof(this->networkId);

        memcpy(&this->teamId, &buffer[offset], sizeof(this->teamId));
        offset += sizeof(this->teamId);

        memcpy(&this->myTeam, &buffer[offset], sizeof(this->myTeam));
        offset += sizeof(this->myTeam);

        memcpy(&this->raceNumber, &buffer[offset], sizeof(this->raceNumber));
        offset += sizeof(this->raceNumber);

        memcpy(&this->nationality, &buffer[offset], sizeof(this->nationality));
        offset += sizeof(this->nationality);

        memcpy(&this->name, &buffer[offset], sizeof(this->name));
        offset += 48UL;

        memcpy(&this->yourTelemetry, &buffer[offset], sizeof(this->yourTelemetry));
        offset += sizeof(this->yourTelemetry);

        memcpy(&this->showOnlineNames, &buffer[offset], sizeof(this->showOnlineNames));
        offset += sizeof(this->showOnlineNames);

        memcpy(&this->platform, &buffer[offset], sizeof(this->platform));
        offset += sizeof(this->platform);

        return offset;
    }

    void print()
    {
        std::wcout << "aiControlled: " << (this->aiControlled == 0 ? false : true) << std::endl;
        std::wcout << "   "
                   << "team: " << sTeams[this->teamId] << std::endl;
        std::wcout << "   "
                   << "raceNumber: " << this->raceNumber << std::endl;
        std::wcout << "   "
                   << "nationality: " << Nationalities[this->nationality] << std::endl;
        std::wcout << "   "
                   << "name: ";
        int i = 0;
        while (this->name[i] != '\0')
        {
            std::wcout << this->name[i];
            i += 1;
        }
        std::wcout << std::endl;
    }
};

struct SPacketParticipantsData
{
    SPacketHeader header;  // Header
    uint8_t numActiveCars; // Should match number of cars on HUD
    SParticipantsData participants[22];

    void get(char *buffer)
    {
        unsigned long offset = this->header.get(buffer);

        memcpy(&this->numActiveCars, &buffer[offset], sizeof(this->numActiveCars));
        offset += sizeof(this->numActiveCars);

        for (int i = 0; i < this->numActiveCars; i++)
        {
            offset = this->participants[i].get(buffer, offset);
        }
    }

    void print()
    {
        std::wcout << " "
                   << "PacketParticipantsData {" << std::endl;
        std::wcout << "   "
                   << "numActiveCars: " << this->numActiveCars << std::endl;
        for (int i = 0; i < this->numActiveCars; i++)
        {
            std::wcout << "   "
                       << "participants[" << i << "]: " << std::endl;
            this->participants[i].print();
        }
        std::wcout << " "
                   << "}" << std::endl;
    }
};