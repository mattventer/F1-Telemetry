#pragma once

#include "../f122constants.h"
#include "header.h"

#include <cstdint>
#include <memory>

using namespace F122;

// Car Damage Packet
// This packet details car damage parameters for all the cars in the race.
// Frequency: 2 per second
// Size: 948 bytes
// Version: 1
struct SCarDamageData
{
       float tyresWear[4];           // Tyre wear (percentage)
       uint8_t tyresDamage[4];       // Tyre damage (percentage)
       uint8_t brakesDamage[4];      // Brakes damage (percentage)
       uint8_t frontLeftWingDamage;  // Front left wing damage (percentage)
       uint8_t frontRightWingDamage; // Front right wing damage (percentage)
       uint8_t rearWingDamage;       // Rear wing damage (percentage)
       uint8_t floorDamage;          // Floor damage (percentage)
       uint8_t diffuserDamage;       // Diffuser damage (percentage)
       uint8_t sidepodDamage;        // Sidepod damage (percentage)
       uint8_t drsFault;             // Indicator for DRS fault, 0 = OK, 1 = fault
       uint8_t ersFault;             // Indicator for ERS fault, 0 = OK, 1 = fault
       uint8_t gearBoxDamage;        // Gear box damage (percentage)
       uint8_t engineDamage;         // Engine damage (percentage)
       uint8_t engineMGUHWear;       // Engine wear MGU-H (percentage)
       uint8_t engineESWear;         // Engine wear ES (percentage)
       uint8_t engineCEWear;         // Engine wear CE (percentage)
       uint8_t engineICEWear;        // Engine wear ICE (percentage)
       uint8_t engineMGUKWear;       // Engine wear MGU-K (percentage)
       uint8_t engineTCWear;         // Engine wear TC (percentage)
       uint8_t engineBlown;          // Engine blown, 0 = OK, 1 = fault
       uint8_t engineSeized;         // Engine seized, 0 = OK, 1 = fault

       unsigned long get(char *buffer, unsigned long offset)
       {
              for (int i = 0; i < 4; i++)
              {
                     memcpy(&this->tyresWear[i], &buffer[offset],
                            sizeof(this->tyresWear[i]));
                     offset = offset + sizeof(this->tyresWear[i]);
              }
              for (int i = 0; i < 4; i++)
              {
                     memcpy(&this->tyresDamage[i], &buffer[offset],
                            sizeof(this->tyresDamage[i]));
                     offset = offset + sizeof(this->tyresDamage[i]);
              }
              for (int i = 0; i < 4; i++)
              {
                     memcpy(&this->brakesDamage[i], &buffer[offset],
                            sizeof(this->brakesDamage[i]));
                     offset = offset + sizeof(this->brakesDamage[i]);
              }
              memcpy(&this->frontLeftWingDamage, &buffer[offset],
                     sizeof(this->frontLeftWingDamage));
              offset = offset + sizeof(this->frontLeftWingDamage);

              memcpy(&this->frontRightWingDamage, &buffer[offset],
                     sizeof(this->frontRightWingDamage));
              offset = offset + sizeof(this->frontRightWingDamage);

              memcpy(&this->rearWingDamage, &buffer[offset],
                     sizeof(this->rearWingDamage));
              offset = offset + sizeof(this->rearWingDamage);

              memcpy(&this->floorDamage, &buffer[offset],
                     sizeof(this->floorDamage));
              offset = offset + sizeof(this->floorDamage);

              memcpy(&this->diffuserDamage, &buffer[offset],
                     sizeof(this->diffuserDamage));
              offset = offset + sizeof(this->diffuserDamage);

              memcpy(&this->sidepodDamage, &buffer[offset],
                     sizeof(this->sidepodDamage));
              offset = offset + sizeof(this->sidepodDamage);

              memcpy(&this->drsFault, &buffer[offset], sizeof(this->drsFault));
              offset = offset + sizeof(this->drsFault);

              memcpy(&this->ersFault, &buffer[offset], sizeof(this->ersFault));
              offset = offset + sizeof(this->ersFault);

              memcpy(&this->gearBoxDamage, &buffer[offset],
                     sizeof(this->gearBoxDamage));
              offset = offset + sizeof(this->gearBoxDamage);

              memcpy(&this->engineDamage, &buffer[offset],
                     sizeof(this->engineDamage));
              offset = offset + sizeof(this->engineDamage);

              memcpy(&this->engineMGUHWear, &buffer[offset],
                     sizeof(this->engineMGUHWear));
              offset = offset + sizeof(this->engineMGUHWear);

              memcpy(&this->engineESWear, &buffer[offset],
                     sizeof(this->engineESWear));
              offset = offset + sizeof(this->engineESWear);

              memcpy(&this->engineCEWear, &buffer[offset],
                     sizeof(this->engineCEWear));
              offset = offset + sizeof(this->engineCEWear);

              memcpy(&this->engineICEWear, &buffer[offset],
                     sizeof(this->engineICEWear));
              offset = offset + sizeof(this->engineICEWear);

              memcpy(&this->engineMGUKWear, &buffer[offset],
                     sizeof(this->engineMGUKWear));
              offset = offset + sizeof(this->engineMGUKWear);

              memcpy(&this->engineTCWear, &buffer[offset],
                     sizeof(this->engineTCWear));
              offset = offset + sizeof(this->engineTCWear);

              memcpy(&this->engineBlown, &buffer[offset],
                     sizeof(this->engineBlown));
              offset = offset + sizeof(this->engineBlown);

              memcpy(&this->engineSeized, &buffer[offset],
                     sizeof(this->engineSeized));
              offset = offset + sizeof(this->engineSeized);

              return offset;
       }
};

struct SPacketCarDamageData
{
       SPacketHeader header; // Header
       SCarDamageData carDamageData[22];

       unsigned long get(char *buffer)
       {
              unsigned long offset = this->header.get(buffer);
              for (int i = 0; i < 22; i++)
              {
                     offset = this->carDamageData[i].get(buffer, offset);
              }

              return offset;
       }
};