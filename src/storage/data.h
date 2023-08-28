#pragma once

#include "f123constants.h"

#include <cstdint>
#include <vector>

namespace SessionStorage
{
    struct SLap
    {
        uint8_t lapNumber{0};
        uint16_t sector1MS{0};
        uint16_t sector2MS{0};
        uint16_t sector3MS{0};
        uint32_t totalLapTime{0};
    };

    struct SSessionData
    {
        F123::ESessionType sessionType;
        std::vector<SLap> laps;

        int fastestSec1LapNum{0};
        int fastestSec2LapNum{0};
        int fastestSec3LapNum{0};
        uint8_t fastestLapNum{0};
    };

    struct SRaceData
    {
        std::string trackName;
        std::vector<SSessionData> sessions;
    };
}