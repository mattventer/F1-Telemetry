#pragma once

#include "imgui.h"

#include "spdlog/spdlog.h"
#include "../widgets/cardamage.h"
#include "../widgets/lapdeltas.h"
#include "../widgets/lapinfoheader.h"
#include "../widgets/sessioninfo.h"
#include "../widgets/tyretemps.h"
#include "../widgets/tyrewear.h"

#include <memory>
#include <cstdint>

class CDashboard
{
public:
    CDashboard(const std::shared_ptr<CTyreWearGraph> &tyreWearGraph,
               const std::shared_ptr<CTyreTemps> &tyreTemps,
               const std::shared_ptr<CCarDamageGraph> &carDamageGraph,
               const std::shared_ptr<CLapDeltas> &lapDeltas,
               const std::shared_ptr<CLapInfoHeader> &lapInfoHeader)
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
    const std::shared_ptr<CTyreTemps> mTyreTemps;
    const std::shared_ptr<CTyreWearGraph> mTyreWearGraph;
    const std::shared_ptr<CCarDamageGraph> mCarDamageGraph;
    const std::shared_ptr<CLapDeltas> mLapDeltas;
    const std::shared_ptr<CLapInfoHeader> mLapInfoHeader;
};