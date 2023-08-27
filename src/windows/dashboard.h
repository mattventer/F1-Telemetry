#pragma once

#include "imgui.h"

#include "../widgets/cardamage.h"
#include "../widgets/lapdata.h"
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
               const std::shared_ptr<CLapData> &lapData)
        : mTyreWearGraph(tyreWearGraph), mCarDamageGraph(carDamageGraph), mTyreTemps(tyreTemps), mLapData(lapData)
    {
        mWindowFlags |= ImGuiWindowFlags_NoTitleBar;
        mWindowFlags |= ImGuiWindowFlags_NoMove;
        mWindowFlags |= ImGuiWindowFlags_NoResize;
    }
    void ShowWindow(bool *p_open)
    {
        IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
        mCarDamageGraph->ShowGraph();
        ImGui::SameLine();
        mTyreTemps->ShowTyreInnerTemps();
        mTyreWearGraph->ShowGraph();
        ImGui::SameLine();
        mLapData->ShowDeltas();
    }

private:
    const std::shared_ptr<CTyreTemps> mTyreTemps;
    const std::shared_ptr<CTyreWearGraph> mTyreWearGraph;
    const std::shared_ptr<CCarDamageGraph> mCarDamageGraph;
    const std::shared_ptr<CLapData> mLapData;

    ImGuiWindowFlags mWindowFlags{0};
};