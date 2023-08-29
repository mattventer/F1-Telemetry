#pragma once

#include "constants.h"
#include "imgui.h"
#include "implot.h"

#include <array>
#include <cstdint>

class CTyreWearGraph
{
public:
    CTyreWearGraph()
    {
        // Axis config
        mAxisFlagsX |= ImPlotAxisFlags_NoGridLines;
        mAxisFlagsX |= ImPlotAxisFlags_NoTickMarks;
        mAxisFlagsX |= ImPlotAxisFlags_Lock;
        mAxisFlagsX |= ImPlotAxisFlags_NoSideSwitch;
        mAxisFlagsX |= ImPlotAxisFlags_NoMenus;

        mAxisFlagsY |= ImPlotAxisFlags_Lock;
        mAxisFlagsY |= ImPlotAxisFlags_NoMenus;
        mAxisFlagsY |= ImPlotAxisFlags_NoSideSwitch;
    }

    void SetTyreWear(const std::array<float, 4> tyreWearData)
    {
        for (int i = 0; i < 4; ++i)
        {
            mTyreWearData[i] = tyreWearData.at(i);
        }
    }

    void ResetTyreWear()
    {
        for (int i = 0; i < 4; ++i)
        {
            mTyreWearData[i] = 0;
        }
    }

    void ShowGraph(const ImVec2 spaceAvail) const
    {
        const int item_count = 4;
        // Display
        if (ImPlot::BeginPlot("Tyre Wear", ImVec2(spaceAvail.x / 2, (spaceAvail.y / 2)), ImPlotFlags_NoLegend))
        {
            const double positions[] = {0, 1, 2, 3};
            // Configure
            ImPlot::SetupAxisLimits(ImAxis_Y1, sMinY, sMaxY);
            ImPlot::SetupAxisLimits(ImAxis_X1, sMinX - 0.5, item_count - 0.5);
            ImPlot::SetupAxisTicks(ImAxis_X1, positions, item_count, mLabels);
            ImPlot::SetupAxis(ImAxis_X1, "", mAxisFlagsX);
            ImPlot::SetupAxis(ImAxis_Y1, "", mAxisFlagsY);

            for (int i = 0; i < item_count; ++i)
            {
                // Set color
                ImPlot::SetNextFillStyle(TyreWearToColor(mTyreWearData[i]));
                ImPlot::PlotBars(mLabels[i], &mTyreWearData[i], 1, 0.95, i);
            }

            ImPlot::EndPlot();
        }
    }

private:
    ImPlotAxisFlags mAxisFlagsX{0};
    ImPlotAxisFlags mAxisFlagsY{0};

    float mTyreWearData[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    const char *mLabels[4] = {"RL", "RR", "FL", "FR"};
};