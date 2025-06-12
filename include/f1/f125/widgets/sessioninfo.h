#pragma once

#include "imgui.h"
#include "implot.h"

#include "constants.h"
#include "f125/constants.h"
#include "f1telemetry.h"
#include "spdlog/spdlog.h"

#include <array>
#include <cstdint>
#include <ctime>
#include <memory>
#include <string>

class CSessionInfo25 : public ISessionInfo
{
public:
    CSessionInfo25()
    {
        SPDLOG_TRACE("CSessionInfo25()");
    }

    void SetFont(ImFont *font) override
    {
        mFont = font;
    }

    void SessionStarted() override
    {
        mSessionActive = true;
    }

    void SessionStopped() override
    {
        mSessionActive = false;
    }

    // TODO: include ip
    void SetSocketInfo(const int port) override
    {
        mPort = port;
    }

    void NewPacket() override
    {
        time_t now = time(0);
        mLastPacketTime = std::string(ctime(&now));
    }

    void ShowSessionStatus() const override
    {
        ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - 23);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - 17);
        ImGui::PushFont(mFont);
        ImGui::Text("(v25)");
        ImGui::SameLine();
        ImGui::Text("Listening port:");
        ImGui::SameLine();
        ImGui::Text(std::to_string(mPort).c_str()); // gross
        ImGui::SameLine();
        ImGui::Text(mLastPacketTime.c_str());
        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 120);
        ImGui::Text("Session:");
        ImGui::SameLine();
        ImGui::TextColored((mSessionActive ? F1::green : F1::red),
                           (mSessionActive ? "Active" : "Inactive")); // From constants.h

        ImGui::PopFont();
    }
};