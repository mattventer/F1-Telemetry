#pragma once

#include "constants.h"
#include "f125/constants.h"
#include "f1telemetry.h"
#include "imgui.h"
#include "spdlog/spdlog.h"

#include <cstdint>
#include <iostream>
#include <cstdio>
#include <string>

using namespace F125;

class CLapInfoHeader25 : public ILapInfoHeader
{
public:
    CLapInfoHeader25()
    {
        SPDLOG_TRACE("CLapInfoHeader25()");
    }

    void SetPitLapWindow(const uint8_t min, const uint8_t max, const uint8_t rejoinPos) override
    {
        mPitLapMin = min;
        mPitLapMax = max;
        mPitRejoinPos = rejoinPos;
    }

    void SetCurrentLap(const uint8_t currLap) override
    {
        mCurrentLap = currLap;
    }

    uint8_t CurrentLap() override
    {
        return mCurrentLap;
    }

    float ShowLapInfoHeader() const override
    {
        char text[50];
        std::sprintf(text, "Current Lap: %d\tPit Lap: %d-%d (Rejoin Pos: %d)", mCurrentLap, mPitLapMin, mPitLapMax, mPitRejoinPos);

        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize(text).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::TextColored(F1::teal, text);

        return ImGui::GetCursorPosY();
    }
};