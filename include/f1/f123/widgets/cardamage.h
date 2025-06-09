#pragma once

#include "imgui.h"
#include "implot.h"

#include "constants.h"
#include "f1telemetry.h"
#include "spdlog/spdlog.h"

#include <array>
#include <cstdint>

class CCarDamageGraph23 : public ICarDamageGraph
{
public:
    CCarDamageGraph23()
    {
        SPDLOG_TRACE("CCarDamageGraph()");
        // Axis config
        mAxisFlagsX |= ImPlotAxisFlags_NoGridLines;
        mAxisFlagsX |= ImPlotAxisFlags_NoTickMarks;
        mAxisFlagsX |= ImPlotAxisFlags_Lock;
        mAxisFlagsX |= ImPlotAxisFlags_NoSideSwitch;
        mAxisFlagsX |= ImPlotAxisFlags_NoMenus;

        mAxisFlagsY |= ImPlotAxisFlags_NoMenus;
        mAxisFlagsY |= ImPlotAxisFlags_Lock;
        mAxisFlagsY |= ImPlotAxisFlags_NoSideSwitch;
    }

    void ResetCarDamage() override
    {
        for (int i = 0; i < mDataPoints; ++i)
        {
            mCarDamageData[i] = 0;
        }
    }

    void SetCarDamage(const SCarDamageGraphData &carDamageData) override
    {
        mCarDamageData[0] = carDamageData.engineDamage;
        mCarDamageData[1] = carDamageData.gearBoxDamage;
        mCarDamageData[2] = carDamageData.engineMGUHWear;
        mCarDamageData[3] = carDamageData.engineESWear;
        mCarDamageData[4] = carDamageData.engineCEWear;
        mCarDamageData[5] = carDamageData.engineICEWear;
        mCarDamageData[6] = carDamageData.engineMGUKWear;
        mCarDamageData[7] = carDamageData.engineTCWear;
    }

    void ShowGraph(const ImVec2 spaceAvail) const override
    {
        // Display
        if (ImPlot::BeginPlot("Car Damage", ImVec2(spaceAvail.x / 2, (spaceAvail.y / 2)), ImPlotFlags_NoLegend))
        {
            static const double positions[] = {0, 1, 2, 3, 4, 5, 6, 7};
            // Configure
            ImPlot::SetupAxisLimits(ImAxis_Y1, sMinY, sMaxY);
            ImPlot::SetupAxisLimits(ImAxis_X1, sMinX - 0.5, 8 - 0.5);
            ImPlot::SetupAxisTicks(ImAxis_X1, positions, 8, mLabels);
            ImPlot::SetupAxis(ImAxis_X1, nullptr, mAxisFlagsX);
            ImPlot::SetupAxis(ImAxis_Y1, nullptr, mAxisFlagsY);

            for (int i = 0; i < 8; ++i)
            {
                // Set color
                ImPlot::SetNextFillStyle(CarDamageToColor(mCarDamageData[i]));
                ImPlot::PlotBars(mLabels[i], &mCarDamageData[i], 1, 0.95, i);
            }
            ImPlot::EndPlot();
        }
    }
};