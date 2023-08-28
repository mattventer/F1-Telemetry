#pragma once

#include <cstdint>
#include <memory>

struct SPacketHeader
{
    uint16_t packetFormat;           // 2023
    uint8_t gameYear;                // 23
    uint8_t gameMajorVersion;        // Game major version - "X.00"
    uint8_t gameMinorVersion;        // Game minor version - "1.XX"
    uint8_t packetVersion;           // Version of this packet type, all start from 1
    uint8_t packetId;                // Identifier for the packet type, see below
    uint64_t sessionUid;             // Unique identifier for the session
    float sessionTime;               // Session timestamp
    uint32_t frameIdentifier;        // Identifier for the frame the data was retrieved on
    uint32_t overallFrameIdentifier; // Overall identifier for the frame the data was retrieved on, doesn't go back after flashbacks
    uint8_t playerCarIndex;          // Index of player's car in the array
    uint8_t secondaryPlayerCarIndex; // Index of secondary player's car in the array (splitscreen), 255 if no second player

    unsigned long get(char *buffer)
    {
        unsigned long offset = 0;
        memcpy(&this->packetFormat, &buffer[offset], sizeof(this->packetFormat));
        offset = offset + sizeof(this->packetFormat);

        memcpy(&this->gameYear, &buffer[offset], sizeof(this->gameYear));
        offset = offset + sizeof(this->gameYear);

        memcpy(&this->gameMajorVersion, &buffer[offset], sizeof(this->gameMajorVersion));
        offset = offset + sizeof(this->gameMajorVersion);

        memcpy(&this->gameMinorVersion, &buffer[offset], sizeof(this->gameMinorVersion));
        offset = offset + sizeof(this->gameMinorVersion);

        memcpy(&this->packetVersion, &buffer[offset], sizeof(this->packetVersion));
        offset = offset + sizeof(this->packetVersion);

        memcpy(&this->packetId, &buffer[offset], sizeof(this->packetId));
        offset = offset + sizeof(this->packetId);

        memcpy(&this->sessionUid, &buffer[offset], sizeof(this->sessionUid));
        offset = offset + sizeof(this->sessionUid);

        memcpy(&this->sessionTime, &buffer[offset], sizeof(this->sessionTime));
        offset = offset + sizeof(this->sessionTime);

        memcpy(&this->frameIdentifier, &buffer[offset], sizeof(this->frameIdentifier));
        offset = offset + sizeof(this->frameIdentifier);

        memcpy(&this->overallFrameIdentifier, &buffer[offset], sizeof(this->overallFrameIdentifier));
        offset = offset + sizeof(this->overallFrameIdentifier);

        memcpy(&this->playerCarIndex, &buffer[offset], sizeof(this->playerCarIndex));
        offset = offset + sizeof(this->playerCarIndex);

        memcpy(&this->secondaryPlayerCarIndex, &buffer[offset], sizeof(this->secondaryPlayerCarIndex));
        offset = offset + sizeof(this->secondaryPlayerCarIndex);

        return offset;
    }
};