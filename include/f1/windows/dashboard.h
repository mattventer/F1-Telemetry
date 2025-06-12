#pragma once

#include "imgui.h"

#include "spdlog/spdlog.h"
#include "f1telemetry.h"
#include "f123/widgets/cardamage.h"
#include "f123/widgets/lapdeltas.h"
#include "f123/widgets/lapinfoheader.h"
#include "f123/widgets/sessioninfo.h"
#include "f123/widgets/tyretemps.h"
#include "f123/widgets/tyrewear.h"

#include <memory>
#include <cstdint>

class CDashboard
{
public:
    CDashboard(const std::shared_ptr<ITyreWearGraph> &tyreWearGraph,
               const std::shared_ptr<ITyreTemps> &tyreTemps,
               const std::shared_ptr<ICarDamageGraph> &carDamageGraph,
               const std::shared_ptr<ILapDeltas> &lapDeltas,
               const std::shared_ptr<ILapInfoHeader> &lapInfoHeader)
        : mTyreWearGraph(tyreWearGraph), mCarDamageGraph(carDamageGraph), mTyreTemps(tyreTemps), mLapDeltas(lapDeltas), mLapInfoHeader(lapInfoHeader)
    {
        SPDLOG_TRACE("CDashboard()");
    }
    void ShowWindow(const ImVec2 spaceAvail)
    {
        IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
        auto spaceAvailDash = spaceAvail;
        spaceAvailDash.x -= 15; // Not sure why. Gap between graphs? Tab window padding?
        if (mLapInfoHeader->CurrentLap() > 0)
        {
            auto posY = mLapInfoHeader->ShowLapInfoHeader();
            spaceAvailDash.y -= (posY / 2);
        }
        // Top row
        mCarDamageGraph->ShowGraph(spaceAvailDash);
        ImGui::SameLine();
        mTyreTemps->ShowTyreInnerTemps(spaceAvailDash);

        // Bottom row
        mTyreWearGraph->ShowGraph(spaceAvailDash);
        ImGui::SameLine();
        mLapDeltas->ShowDeltas(spaceAvailDash);
    }

private:
    const std::shared_ptr<ITyreTemps> mTyreTemps;
    const std::shared_ptr<ITyreWearGraph> mTyreWearGraph;
    const std::shared_ptr<ICarDamageGraph> mCarDamageGraph;
    const std::shared_ptr<ILapDeltas> mLapDeltas;
    const std::shared_ptr<ILapInfoHeader> mLapInfoHeader;
};