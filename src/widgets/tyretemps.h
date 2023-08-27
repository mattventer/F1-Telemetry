#pragma once

#include "imgui.h"
#include "implot.h"

#include "constants.h"
#include "f122constants.h"
#include "packets/session.h"

#include <array>
#include <cstdint>

class CTyreTemps
{
public:
    CTyreTemps()
    {
        // Axis config
        mAxisFlagsX |= ImPlotAxisFlags_NoGridLines;
        mAxisFlagsX |= ImPlotAxisFlags_NoTickMarks;
        mAxisFlagsX |= ImPlotAxisFlags_Lock;
        mAxisFlagsX |= ImPlotAxisFlags_NoSideSwitch;

        mAxisFlagsY |= ImPlotAxisFlags_Lock;
        mAxisFlagsY |= ImPlotAxisFlags_NoSideSwitch;
    }

    void SetTyreCompound(F122::EActualTyreCompound actual, F122::EVisualTyreCompound visual)
    {
        mActualCompound = actual;
        mActualTyreCompoundStr = F122::sActualTyreCompoundToString.at(actual);

        mVisualTyreCompound = visual;
        mVisualTyreCompoundStr = F122::sVisualTyreCompoundToString.at(visual);
    }

    void SetTyreInnerTemps(const std::array<uint8_t, 4> tyreTempData)
    {
        for (int i = 0; i < 4; ++i)
        {
            mTyreInnerTemps[i] = tyreTempData.at(i);
        }
    }

    // TODO: magic numbers everywhere
    void ShowTyreInnerTemps() const
    {
        const int item_count = 4;
        auto size = ImGui::GetWindowContentRegionMax();
        std::string title = "Tyre Inner Temp (C)";
        if (mActualCompound != F122::EActualTyreCompound::Unknown)
        {
            title += " | Compound: " + mActualTyreCompoundStr + " (" + mVisualTyreCompoundStr + ")";
        }
        if (ImPlot::BeginPlot(title.c_str(), ImVec2(-1, (size.y / 2) - (30))))
        {
            const double positions[] = {0, 1, 2, 3};
            // Configure
            ImPlot::SetupAxisLimits(ImAxis_Y1, 60, 130);
            ImPlot::SetupAxisLimits(ImAxis_X1, sMinX - 0.5, item_count - 0.5);
            ImPlot::SetupAxisTicks(ImAxis_X1, positions, item_count, mLabels);
            ImPlot::SetupAxis(ImAxis_X1, "", mAxisFlagsX);
            ImPlot::SetupAxis(ImAxis_Y1, "", mAxisFlagsY);

            for (int i = 0; i < item_count; ++i)
            {
                ImPlot::SetNextFillStyle(TyreInnerTempToColor(mActualCompound, mTyreInnerTemps[i]));
                ImPlot::PlotBars(mLabels[i], &mTyreInnerTemps[i], 1, 0.95, i);
            }

            ImPlot::EndPlot();
        }
    }

private:
    ImPlotAxisFlags mAxisFlagsX{0};
    ImPlotAxisFlags mAxisFlagsY{0};
    F122::EActualTyreCompound mActualCompound{F122::EActualTyreCompound::Unknown};
    std::string mActualTyreCompoundStr{""};
    F122::EVisualTyreCompound mVisualTyreCompound{F122::EVisualTyreCompound::Unknown};
    std::string mVisualTyreCompoundStr{""};

    uint8_t mTyreInnerTemps[4] = {0, 0, 0, 0};
    const char *mLabels[4] = {"RL", "RR", "FL", "FR"};
};