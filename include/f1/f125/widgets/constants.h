#pragma once

#include "imgui.h"
#include "f123/constants.h"

#include <cstdint>

namespace
{
    constexpr int sMinY = 0;
    constexpr int sMinX = 0;
    constexpr int sMaxY = 100;

    const ImVec4 blue = ImVec4(26.0f / 255.0f, 198.0f / 255.0f, 255.0f / 255.0f, 50.0f);
    const ImVec4 teal = ImVec4(17.0f / 255.0f, 219.0f / 255.0f, 194.0f / 255.0f, 50.0f);
    const ImVec4 green = ImVec4(97.0f / 255.0f, 198.0f / 255.0f, 36.0f / 255.0f, 50.0f);
    const ImVec4 yellow = ImVec4(247.0f / 255.0f, 224.0f / 255.0f, 31.0f / 255.0f, 50.0f);
    const ImVec4 orange = ImVec4(247.0f / 255.0f, 177.0f / 255.0f, 31.0f / 255.0f, 50.0f);
    const ImVec4 red = ImVec4(209.0f / 255.0f, 44.0f / 255.0f, 42.0f / 255.0f, 50.0f);

    static const ImVec4 TyreWearToColor(const float &wear)
    {
        if (wear < 20)
        {
            return green;
        }
        if (wear < 35)
        {
            return yellow;
        }
        if (wear < 50)
        {
            return orange;
        }
        return red;
    }

    static const ImVec4 CarDamageToColor(const int &wear)
    {
        if (wear < 50)
        {
            return green;
        }
        if (wear < 60)
        {
            return yellow;
        }
        if (wear < 70)
        {
            return orange;
        }
        return red;
    }

    static const ImVec4 TyreInnerTempToColorC5(const uint8_t &temp)
    {
        if (temp < 90)
        {
            return blue;
        }
        if (temp < 100)
        {
            return green;
        }
        if (temp < 105)
        {
            return yellow;
        }
        if (temp < 110)
        {
            return orange;
        }
        return red;
    }

    static const ImVec4 TyreInnerTempToColorC4(const uint8_t &temp)
    {
        if (temp < 93)
        {
            return blue;
        }
        if (temp < 103)
        {
            return green;
        }
        if (temp < 108)
        {
            return yellow;
        }
        if (temp < 113)
        {
            return orange;
        }
        return red;
    }

    static const ImVec4 TyreInnerTempToColorC3(const uint8_t &temp)
    {
        if (temp < 95)
        {
            return blue;
        }
        if (temp < 105)
        {
            return green;
        }
        if (temp < 110)
        {
            return yellow;
        }
        if (temp < 115)
        {
            return orange;
        }
        return red;
    }

    static const ImVec4 TyreInnerTempToColorC2(const uint8_t &temp)
    {
        if (temp < 98)
        {
            return blue;
        }
        if (temp < 108)
        {
            return green;
        }
        if (temp < 113)
        {
            return yellow;
        }
        if (temp < 118)
        {
            return orange;
        }
        return red;
    }

    static const ImVec4 TyreInnerTempToColorC1C0(const uint8_t &temp)
    {
        if (temp < 100)
        {
            return blue;
        }
        if (temp < 110)
        {
            return green;
        }
        if (temp < 115)
        {
            return yellow;
        }
        if (temp < 120)
        {
            return orange;
        }
        return red;
    }

    static const ImVec4 TyreInnerTempToColorInter(const uint8_t &temp)
    {
        if (temp < 80)
        {
            return blue;
        }
        if (temp < 90)
        {
            return green;
        }
        if (temp < 95)
        {
            return yellow;
        }
        if (temp < 100)
        {
            return orange;
        }
        return red;
    }

    static const ImVec4 TyreInnerTempToColorWet(const uint8_t &temp)
    {
        if (temp < 70)
        {
            return blue;
        }
        if (temp < 80)
        {
            return green;
        }
        if (temp < 85)
        {
            return yellow;
        }
        if (temp < 90)
        {
            return orange;
        }
        return red;
    }

    static const ImVec4 TyreInnerTempToColor(const F123::EActualTyreCompound compound, const uint8_t &temp)
    {
        switch (compound)
        {
        case F123::EActualTyreCompound::C5:
            return TyreInnerTempToColorC5(temp);
        case F123::EActualTyreCompound::C4:
            return TyreInnerTempToColorC4(temp);
        case F123::EActualTyreCompound::C3:
            return TyreInnerTempToColorC3(temp);
        case F123::EActualTyreCompound::C2:
            return TyreInnerTempToColorC2(temp);
        case F123::EActualTyreCompound::C1:
            [[fallthrough]];
        case F123::EActualTyreCompound::C0:
            return TyreInnerTempToColorC1C0(temp);
        case F123::EActualTyreCompound::Inter:
            return TyreInnerTempToColorInter(temp);
        case F123::EActualTyreCompound::Wet:
            return TyreInnerTempToColorWet(temp);
        default:
            return blue; // Just to have something
        }
    }
}