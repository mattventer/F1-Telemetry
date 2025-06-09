#pragma once

#include "imgui.h"
#include "implot.h"

#include "constants.h"
#include "f123/session.h"
#include "spdlog/spdlog.h"

#include <array>
#include <cstdint>
#include <ctime>
#include <memory>
#include <string>

namespace
{
    static const float topPadding = 28.0f;
}

class CSessionInfo
{
public:
    CSessionInfo()
    {
        SPDLOG_TRACE("CSessionInfo()");
    }

    void SetFont(ImFont *font)
    {
        mFont = font;
    }

    void SessionStarted()
    {
        mSessionActive = true;
    }

    void SessionStopped()
    {
        mSessionActive = false;
    }

    // TODO: include ip
    void SetSocketInfo(const int port)
    {
        mPort = port;
    }

    void NewPacket(const uint16_t year)
    {
        time_t now = time(0);
        mLastPacketTime = std::string(ctime(&now));
        mPacketVersion = year;
    }

    void ShowSessionStatus() const
    {
        ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - 23);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - 17);
        ImGui::PushFont(mFont);
        ImGui::Text("Listening port:");
        ImGui::SameLine();
        ImGui::Text(std::to_string(mPort).c_str()); // gross
        ImGui::SameLine();
        ImGui::Text(mLastPacketTime.c_str());
        ImGui::SameLine();

        uint8_t offset = (mPacketVersion == 0) ? 120 : 175;
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - offset);
        ImGui::Text("Session:");
        ImGui::SameLine();
        ImGui::TextColored((mSessionActive ? green : red),
                           (mSessionActive ? "Active" : "Inactive")); // From constants.h

        // Packet version (if set)
        if (mPacketVersion != 0)
        {
            ImGui::SameLine();
            ImGui::Text("(");
            ImGui::SameLine();
            ImGui::Text(std::to_string(mPacketVersion).c_str());
            ImGui::SameLine();
            ImGui::Text(")");
        }

        ImGui::PopFont();
    }

private:
    bool mSessionActive{false};
    std::string mLastPacketTime{"Never"};
    uint16_t mPacketVersion{0};
    int mPort{-1};

    // Style
    ImFont *mFont;
};