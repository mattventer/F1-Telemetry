#pragma once

#include "f123/constants.h"

#include <cstdint>
#include <string>
#include <vector>

namespace SessionStorage
{
    struct SLap
    {
        bool complete{false};
        uint8_t lapNumber{0};
        uint16_t sector1MS{0};
        uint16_t sector2MS{0};
        uint16_t sector3MS{0};
        uint32_t totalLapTime{0};
    };

    struct SSessionData
    {
        uint64_t uid{0};
        bool complete{false};
        F123::ESessionType sessionType{F123::ESessionType::Unknown};
        std::vector<SLap> laps{0};

        int fastestSec1LapNum{0};
        int fastestSec2LapNum{0};
        int fastestSec3LapNum{0};
        uint8_t fastestLapNum{0};
    };

    struct SRaceWeekend
    {
        std::string trackName{"Unknown"};
        std::string date{""};
        std::vector<SSessionData> sessions{0};
        bool complete{false};
    };
}