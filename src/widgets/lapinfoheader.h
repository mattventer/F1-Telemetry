#pragma once

#include "constants.h"
#include "imgui.h"

#include <cstdint>
#include <iostream>
#include <cstdio>
#include <string>

class CLapInfoHeader
{
public:
    CLapInfoHeader()
    {
    }

    void SetPitLapWindow(const uint8_t min, const uint8_t max, const uint8_t rejoinPos)
    {
        mPitLapMin = min;
        mPitLapMax = max;
        mPitRejoinPos = rejoinPos;
    }

    void SetCurrentLap(const uint8_t currLap)
    {
        mCurrentLap = currLap;
    }

    uint8_t CurrentLap()
    {
        return mCurrentLap;
    }

    float ShowLapInfoHeader() const
    {
        char text[50];
        std::sprintf(text, "Current Lap: %d\tPit Lap: %d-%d (Rejoin Pos: %d)", mCurrentLap, mPitLapMin, mPitLapMax, mPitRejoinPos);

        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize(text).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::TextColored(teal, text);

        return ImGui::GetCursorPosY();
    }

private:
    uint8_t mPitLapMax{0};
    uint8_t mPitLapMin{0};
    uint8_t mPitRejoinPos{0};
    uint8_t mCurrentLap{0};
};