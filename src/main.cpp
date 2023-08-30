#include <csignal>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <memory>
#include <thread>
#include <winsock2.h>
#include <windows.h>

#include "f123constants.h"
#include "packets/cardamage.h"
#include "packets/carsetup.h"
#include "packets/carstatus.h"
#include "packets/cartelemetry.h"
#include "packets/lapdata.h"
#include "packets/event.h"
#include "packets/header.h"
#include "packets/participants.h"
#include "packets/session.h"
#include "udpserver.h"
#include "windows/dashboard.h"
#include "widgets/cardamage.h"
#include "widgets/lapdeltas.h"
#include "widgets/lapinfoheader.h"
#include "widgets/sessionhistory.h"
#include "widgets/sessioninfo.h"
#include "widgets/tyrewear.h"

#include "imgui.h"
#include "implot.h"
#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_win32.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>

#define ImTextureID ImU64

using namespace F123;

const int sUdpPort = 20777;

// Components
std::unique_ptr<CUdpClient> client;
std::thread listener;

// Graphs
const auto sSessionInfo = std::make_shared<CSessionInfo>();
const auto sSessionHistory = std::make_shared<CSessionHistory>();
const auto sTyreTemps = std::make_shared<CTyreTemps>();
const auto sTyreWearGraph = std::make_shared<CTyreWearGraph>();
const auto sCarDamageGraph = std::make_shared<CCarDamageGraph>();
const auto sLapDeltas = std::make_shared<CLapDeltas>();
const auto sLapInfoHeader = std::make_shared<CLapInfoHeader>();

// Windows
const auto sDashboard = std::make_shared<CDashboard>(sTyreWearGraph, sTyreTemps, sCarDamageGraph, sLapDeltas, sLapInfoHeader);

struct FrameContext
{
    ID3D12CommandAllocator *CommandAllocator;
    UINT64 FenceValue;
};

// Data
static int const NUM_FRAMES_IN_FLIGHT = 3;
static FrameContext g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
static UINT g_frameIndex = 0;

static int const NUM_BACK_BUFFERS = 3;
static ID3D12Device *g_pd3dDevice = nullptr;
static ID3D12DescriptorHeap *g_pd3dRtvDescHeap = nullptr;
static ID3D12DescriptorHeap *g_pd3dSrvDescHeap = nullptr;
static ID3D12CommandQueue *g_pd3dCommandQueue = nullptr;
static ID3D12GraphicsCommandList *g_pd3dCommandList = nullptr;
static ID3D12Fence *g_fence = nullptr;
static HANDLE g_fenceEvent = nullptr;
static UINT64 g_fenceLastSignaledValue = 0;
static IDXGISwapChain3 *g_pSwapChain = nullptr;
static HANDLE g_hSwapChainWaitableObject = nullptr;
static ID3D12Resource *g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static D3D12_CPU_DESCRIPTOR_HANDLE g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext *WaitForNextFrameResources();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void signal_callback_handler(int signal)
{
    client->stop();
}

void ParsePacket(char *buffer, int n)
{
    if (n < 0)
    {
        std::cout << "Empty Packet" << std::endl;
        return;
    }
    SPacketHeader header;
    header.get(buffer);
    if (header.packetFormat != 2023)
    {
        std::cerr << "Incorrect packet format. Expected 2022, received " << header.packetFormat << std::endl;
        return;
    }

    // Update last received time
    sSessionInfo->NewPacket();

    // Parse based on packet id
    const EPacketId id = sPacketIds[header.packetId];
    const int playerIdx = header.playerCarIndex;

    switch (id)
    {
    case EPacketId::Participants:
    {
        SPacketParticipantsData packetParticipantsData;
        packetParticipantsData.get(buffer);
        break;
    }
    case EPacketId::CarSetups:
    {
        SPacketCarSetupData carSetup;
        carSetup.get(buffer);
        break;
    }
    case EPacketId::CarStatus:
    {
        SPacketCarStatusData carStatus;
        carStatus.get(buffer);
        try
        {
            auto actual = static_cast<EActualTyreCompound>(carStatus.carStatusData[playerIdx].actualTyreCompound);
            auto visual = static_cast<EVisualTyreCompound>(carStatus.carStatusData[playerIdx].visualTyreCompound);
            sTyreTemps->SetTyreCompound(actual, visual);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
        break;
    }
    case EPacketId::CarTelemetry:
    {
        SPacketCarTelemetryData carTelemetry;
        carTelemetry.get(buffer);

        std::array<uint8_t, 4> tyreInnerTemps;
        // Get data
        for (int i = 0; i < 4; ++i)
        {
            tyreInnerTemps[i] = carTelemetry.carTelemetryData[playerIdx].tyresInnerTemperature[i];
        }
        sTyreTemps->SetTyreInnerTemps(tyreInnerTemps);
        break;
    }
    case EPacketId::CarDamage:
    {
        SPacketCarDamageData damage;
        damage.get(buffer);

        std::array<float, 4> tyreWear;
        // Get data
        for (int i = 0; i < 4; ++i)
        {
            tyreWear[i] = damage.carDamageData[playerIdx].tyresWear[i];
        }
        // TODO: move CTyreWear into CCarDamage
        sTyreWearGraph->SetTyreWear(tyreWear);
        sCarDamageGraph->SetCarDamage(damage.carDamageData[playerIdx]);
        break;
    }
    case EPacketId::LapData:
    {
        SPacketLapData lapData;
        lapData.get(buffer);
        const auto myRacePosition = lapData.lapData[playerIdx].carPosition;
        SLapData carBehind;
        if (myRacePosition < 21)
        {
            for (int i = 0; i < 22; ++i)
            {
                if (lapData.lapData[i].carPosition == myRacePosition + 1)
                {
                    carBehind = lapData.lapData[i];
                    break;
                }
            }
        }

        sSessionInfo->SessionStarted();
        sLapDeltas->SetLapData(lapData.lapData[playerIdx], carBehind);
        sLapInfoHeader->SetCurrentLap(lapData.lapData[playerIdx].currentLapNum);
        break;
    }
    case EPacketId::Session:
    {
        sSessionInfo->SessionStarted();
        SPacketSessionData sessionData;
        sessionData.get(buffer);

        auto trackId = static_cast<ETrackId>(sessionData.trackId);
        auto sessionType = static_cast<ESessionType>(sessionData.sessionType);
        // Banking on this always coming before LapData
        if (!sSessionHistory->SessionActive())
        {
            sSessionHistory->StartSession(trackId, sessionType);
        }
        // TODO: fix, always 0
        // sLapInfoHeader->SetPitLapWindow(sessionData.pitStopWindowIdealLap, sessionData.pitStopWindowLatestLap, sessionData.pitStopRejoinPosition);
        break;
    }
    case EPacketId::SessionHistory:
    {
        SPacketSessionHistoryData sessionHistory;
        sessionHistory.get(buffer);

        if (sessionHistory.carIdx == playerIdx)
        {
            sSessionHistory->SetSessionHistoryData(sessionHistory);
        }
        break;
    }
    case EPacketId::Event:
    {
        SPacketEventData event;
        switch (event.getCode(buffer))
        {
        case EEventCode::SessionStarted:
            sSessionInfo->SessionStarted();
            sLapDeltas->ResetLapData();
            break;
        case EEventCode::SessionEnded:
            sSessionInfo->SessionStopped();
            sSessionHistory->StopSession();
            sLapInfoHeader->SetPitLapWindow(0, 0, 0);
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

int main()
{
    // Signal handler
    signal(SIGINT, signal_callback_handler);
    client = std::make_unique<CUdpClient>(sUdpPort);

    listener = std::thread(&CUdpClient::start, *client, ParsePacket);

    sSessionInfo->SetSocketInfo(sUdpPort);

    // Create application window
    // ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = {sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"F123 Telemetry", nullptr};
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"F123 Telemetry", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
                        DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
                        g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
                        g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

    // Load fonts
    auto defaultFont = io.Fonts->AddFontDefault();
    auto mainTabsFont = io.Fonts->AddFontFromFileTTF("misc/fonts/ABeeZee-Regular.ttf", 20.0f);

    // Session history tab fonts
    auto tableHeaderFont = io.Fonts->AddFontFromFileTTF("misc/fonts/TitilliumWeb-Bold.ttf", 30.0f);
    auto raceHeaderFont = io.Fonts->AddFontFromFileTTF("misc/fonts/din1451altG.ttf", 18.0f);
    auto sessionHeaderFont = io.Fonts->AddFontFromFileTTF("misc/fonts/din1451altG.ttf", 17.0f);
    auto generalTableFont = io.Fonts->AddFontFromFileTTF("misc/fonts/din1451altG.ttf", 16.5f);
    sSessionHistory->SetFonts(tableHeaderFont, raceHeaderFont, sessionHeaderFont, generalTableFont);

    auto sessionInfoFont = io.Fonts->AddFontFromFileTTF("misc/fonts/ABeeZee-Regular.ttf", 16.0f);
    sSessionInfo->SetFont(sessionInfoFont);

    // Our state
    static bool show_history_window = true;
    static bool show_demo_windows = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
            {
                client->stop();
                listener.join();
                done = true;
            }
        }
        if (done)
        {
            break;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Full screen
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

        ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_NoTitleBar;
        mainWindowFlags |= ImGuiWindowFlags_NoMove;
        mainWindowFlags |= ImGuiWindowFlags_NoResize;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushFont(mainTabsFont);

        if (!ImGui::Begin("Main", &done, mainWindowFlags))
        {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return -1;
        }

        // Always show at bottom
        sSessionInfo->ShowSessionStatus();
        auto ySpaceConsumed = 62.0f; // TODO: Magic Y-offset number
        ImGui::SetCursorPos(ImGui::GetCursorStartPos());

        if (ImGui::BeginTabBar("MainTabs"))
        {
            auto spaceAvail = ImGui::GetContentRegionMax();
            spaceAvail.y -= ySpaceConsumed;
            ImGui::SetNextWindowSize(spaceAvail);

            if (ImGui::BeginTabItem("Live"))
            {
                sDashboard->ShowWindow(spaceAvail);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("History"))
            {
                sSessionHistory->ShowSessionHistory(&show_history_window);
                ImGui::EndTabItem();
            }
            if (show_demo_windows && ImGui::BeginTabItem("ImGui Demo"))
            {
                ImGui::ShowDemoWindow();
                ImGui::EndTabItem();
            }
            if (show_demo_windows && ImGui::BeginTabItem("ImPlot Demo"))
            {
                ImPlot::ShowDemoWindow();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::PopFont();

        ImGui::PopStyleVar();
        ImGui::End();

        // Rendering
        ImGui::Render();

        FrameContext *frameCtx = WaitForNextFrameResources();
        UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
        frameCtx->CommandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        g_pd3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
        g_pd3dCommandList->ResourceBarrier(1, &barrier);

        // Render Dear ImGui graphics
        const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w};
        g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, nullptr);
        g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
        g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        g_pd3dCommandList->ResourceBarrier(1, &barrier);
        g_pd3dCommandList->Close();

        g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList *const *)&g_pd3dCommandList);

        g_pSwapChain->Present(1, 0); // Present with vsync

        UINT64 fenceValue = g_fenceLastSignaledValue + 1;
        g_pd3dCommandQueue->Signal(g_fence, fenceValue);
        g_fenceLastSignaledValue = fenceValue;
        frameCtx->FenceValue = fenceValue;
    }

    client->stop();
    listener.join();

    WaitForLastSubmittedFrame();

    // Cleanup
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC1 sd;
    {
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = NUM_BACK_BUFFERS;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;
    }

    // Create device
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if (D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
        return false;

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = NUM_BACK_BUFFERS;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
            return false;

        SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        {
            g_mainRenderTargetDescriptor[i] = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
            return false;
    }

    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
            return false;
    }

    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
            return false;

    if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, nullptr, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
        g_pd3dCommandList->Close() != S_OK)
        return false;

    if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
        return false;

    g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (g_fenceEvent == nullptr)
        return false;

    {
        IDXGIFactory4 *dxgiFactory = nullptr;
        IDXGISwapChain1 *swapChain1 = nullptr;
        if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
            return false;
        if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, nullptr, nullptr, &swapChain1) != S_OK)
            return false;
        if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
            return false;
        swapChain1->Release();
        dxgiFactory->Release();
        g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
        g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
    }

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain)
    {
        g_pSwapChain->SetFullscreenState(false, nullptr);
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (g_hSwapChainWaitableObject != nullptr)
    {
        CloseHandle(g_hSwapChainWaitableObject);
    }
    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_frameContext[i].CommandAllocator)
        {
            g_frameContext[i].CommandAllocator->Release();
            g_frameContext[i].CommandAllocator = nullptr;
        }
    if (g_pd3dCommandQueue)
    {
        g_pd3dCommandQueue->Release();
        g_pd3dCommandQueue = nullptr;
    }
    if (g_pd3dCommandList)
    {
        g_pd3dCommandList->Release();
        g_pd3dCommandList = nullptr;
    }
    if (g_pd3dRtvDescHeap)
    {
        g_pd3dRtvDescHeap->Release();
        g_pd3dRtvDescHeap = nullptr;
    }
    if (g_pd3dSrvDescHeap)
    {
        g_pd3dSrvDescHeap->Release();
        g_pd3dSrvDescHeap = nullptr;
    }
    if (g_fence)
    {
        g_fence->Release();
        g_fence = nullptr;
    }
    if (g_fenceEvent)
    {
        CloseHandle(g_fenceEvent);
        g_fenceEvent = nullptr;
    }
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}

void CreateRenderTarget()
{
    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
    {
        ID3D12Resource *pBackBuffer = nullptr;
        g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, g_mainRenderTargetDescriptor[i]);
        g_mainRenderTargetResource[i] = pBackBuffer;
    }
}

void CleanupRenderTarget()
{
    WaitForLastSubmittedFrame();

    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        if (g_mainRenderTargetResource[i])
        {
            g_mainRenderTargetResource[i]->Release();
            g_mainRenderTargetResource[i] = nullptr;
        }
}

void WaitForLastSubmittedFrame()
{
    FrameContext *frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue == 0)
        return; // No fence was signaled

    frameCtx->FenceValue = 0;
    if (g_fence->GetCompletedValue() >= fenceValue)
        return;

    g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
    WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext *WaitForNextFrameResources()
{
    UINT nextFrameIndex = g_frameIndex + 1;
    g_frameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = {g_hSwapChainWaitableObject, nullptr};
    DWORD numWaitableObjects = 1;

    FrameContext *frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue != 0) // means no fence was signaled
    {
        frameCtx->FenceValue = 0;
        g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
        waitableObjects[1] = g_fenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

    return frameCtx;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            WaitForLastSubmittedFrame();
            CleanupRenderTarget();
            HRESULT result = g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
            assert(SUCCEEDED(result) && "Failed to resize swapchain.");
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}