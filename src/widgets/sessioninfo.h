#pragma once

#include "imgui.h"
#include "implot.h"

#include "constants.h"
#include "packets/session.h"

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

    void NewPacket()
    {
        time_t now = time(0);
        mLastPacketTime = std::string(ctime(&now));
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

        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 120);
        ImGui::Text("Session:");
        ImGui::SameLine();
        ImGui::TextColored((mSessionActive ? green : red),
                           (mSessionActive ? "Active" : "Inactive")); // From constants.h
        ImGui::PopFont();
    }

private:
    bool mSessionActive{false};
    std::string mLastPacketTime{"Never"};
    int mPort{-1};

    // Style
    ImFont *mFont;
};