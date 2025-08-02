// dear imgui: standalone example application for DirectX 9
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include "imgui_internal.h"
#include "byte.h"
#include <d3dx9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

float dpi_scale = 1.f;

ImFont* info = nullptr;
ImFont* info_big = nullptr;
ImFont* gilroy = nullptr;
ImFont* gilroy_mini = nullptr;
ImFont* iconfont = nullptr;
IDirect3DTexture9* logggo = nullptr;
ImVec2 pos;
ImDrawList* draw;


static int sliderint = 0;

static bool checkbox = false;



// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};
// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


static int tabs = 1;
static int subtabs = 0;
static int sett = 0;

// Main code

void decorations()
{

    pos = ImGui::GetWindowPos();
    draw = ImGui::GetWindowDrawList();


    draw->AddRectFilled(pos, ImVec2(pos.x + 805, pos.y + 480), ImColor(235, 235, 240), 12);
    
}

void tabs_()
{
    
    ImGui::BeginChild("##tabs", ImVec2(50, 475));
    ImGui::SetCursorPos(ImVec2(27, 7)); ImGui::PushFont(iconfont); ImGui::Text("K"); ImGui::PopFont();
    ImGui::SetCursorPos(ImVec2(17, 75));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 16));
    ImGui::BeginGroup();
    {
        if (ImGui::tab("A", 0 == tabs)) tabs = 0;
        if (ImGui::tab("B", 1 == tabs)) tabs = 1;
        if (ImGui::tab("C", 2 == tabs)) tabs = 2;
        if (ImGui::tab("D", 3 == tabs)) tabs = 3;
        if (ImGui::tab("E", 4 == tabs)) tabs = 4;
        ImGui::SetCursorPosY(450);
        if (ImGui::settingsbutton("L")) sett = 1;
    }
    ImGui::EndGroup();
    ImGui::PopStyleVar();
    ImGui::EndChild();
}

void subtabs_()
{
    if (tabs == 1)
    {
        ImGui::SetCursorPos(ImVec2(50, 0));
        ImGui::BeginChild("Visuals", ImVec2(160, 475));
        {
            ImGui::SetCursorPos(ImVec2(15, 50));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 10));
            ImGui::BeginGroup();
            {
                ImGui::TextColored(ImColor(195, 195, 200), "ENEMY");
                ImGui::Spacing(0);
                if (ImGui::sub("ESP", 0 == subtabs)) subtabs = 0;
                if (ImGui::sub("Chams", 1 == subtabs)) subtabs = 1;
                if (ImGui::sub("Other", 2 == subtabs)) subtabs = 2;
                ImGui::Spacing(7);

                ImGui::TextColored(ImColor(195, 195, 200), "TEAM");
                ImGui::Spacing();
                if (ImGui::sub("ESP##1", 3 == subtabs)) subtabs = 3;
                if (ImGui::sub("Chams##1", 4 == subtabs)) subtabs = 4;
                if (ImGui::sub("Other##1", 5 == subtabs)) subtabs = 5;

                ImGui::Spacing(7);

                ImGui::TextColored(ImColor(195, 195, 200), "WORLD");
                ImGui::Spacing();
                if (ImGui::sub("ESP##2", 6 == subtabs)) subtabs = 6;
                if (ImGui::sub("Chams##2", 7 == subtabs)) subtabs = 7;
                if (ImGui::sub("Other##2", 8 == subtabs)) subtabs = 8;
            }
            ImGui::EndGroup();
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
    }
    
}

void function()
{

    if (tabs == 1)
    {
        if (subtabs == 0)
        {
            ImGui::SetCursorPos(ImVec2(210, 0));
            ImGui::BeginChild("General", ImVec2(280, 475));
            {
                ImGui::SetCursorPos(ImVec2(15, 50));
                ImGui::BeginGroup();
                {
                    ImGui::Checkbox("Checkbox", &checkbox);
                    ImGui::SliderInt("SliderInt", &sliderint, 0, 100);
                    ImGui::Button("Button", ImVec2(220, 30));
                }
                ImGui::EndGroup();
            }
            ImGui::EndChild();
            ImGui::SameLine(0, 0);
            ImGui::BeginChild("Preview", ImVec2(315, 475));
            {

            }
            ImGui::EndChild();
        }
    }

}

int main(int, char**)
{
    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("ћеню аче)"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGuiStyle* style = &ImGui::GetStyle();

    ImGui::StyleColorsDark();

    style->Alpha = 1.f;
    style->WindowRounding = 5;
    style->FramePadding = ImVec2(4, 3);
    style->WindowPadding = ImVec2(0, 0);
    style->ItemInnerSpacing = ImVec2(4, 4);
    style->ItemSpacing = ImVec2(8, 0);
    style->FrameRounding = 12;
    style->ScrollbarSize = 2.f;
    style->ScrollbarRounding = 12.f;
    style->PopupRounding = 4.f;
    style->Rounding = 11.f;


    ImVec4* colors = ImGui::GetStyle().Colors;

    colors[ImGuiCol_ChildBg] = ImColor(26, 30, 35, 0);
    colors[ImGuiCol_Border] = ImVec4(255, 255, 255, 0);
    colors[ImGuiCol_FrameBg] = ImColor(18, 19, 23, 255);
    colors[ImGuiCol_FrameBgActive] = ImColor(25, 25, 33, 255);
    colors[ImGuiCol_FrameBgHovered] = ImColor(25, 25, 33, 255);
    colors[ImGuiCol_Header] = ImColor(141, 142, 144, 255);
    colors[ImGuiCol_HeaderActive] = ImColor(141, 142, 144, 255);
    colors[ImGuiCol_HeaderHovered] = ImColor(141, 142, 144, 255);
    colors[ImGuiCol_PopupBg] = ImColor(141, 142, 144, 255);
    colors[ImGuiCol_Button] = ImColor(160, 30, 30, 255);
    colors[ImGuiCol_ButtonHovered] = ImColor(190, 45, 35, 255);
    colors[ImGuiCol_ButtonActive] = ImColor(220, 60, 40, 255);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(110 / 255.f, 122 / 255.f, 200 / 255.f, 1.f);
    colors[ImGuiCol_SliderGrab] = ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f);
    colors[ImGuiCol_CheckMark] = ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f);

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsClassic();

    ImFontConfig font_config;
    font_config.OversampleH = 1; //or 2 is the same
    font_config.OversampleV = 1;
    font_config.PixelSnapH = 1;

    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x044F, // Cyrillic
        0,
    };

    gilroy_mini = io.Fonts->AddFontFromMemoryTTF((void*)gilroyfont, sizeof(gilroyfont), 13, &font_config, ranges);
    iconfont = io.Fonts->AddFontFromMemoryTTF((void*)icon, sizeof(icon), 30, &font_config, ranges);
    gilroy = io.Fonts->AddFontFromMemoryTTF((void*)gilroyfont, sizeof(gilroyfont), 17, &font_config, ranges);
    

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);


    
    // Our state
    bool show_demo_window = true, loader_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);
    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {

        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_HorizontalScrollbar);
            {
                static int x = 805 * dpi_scale, y = 575 * dpi_scale;
                pos = ImGui::GetWindowPos();
                draw = ImGui::GetWindowDrawList();

                ImGui::SetWindowSize(ImVec2(ImFloor(x * dpi_scale), ImFloor(y * dpi_scale)));

                decorations();
                tabs_();
                subtabs_();
                function();

            }
            ImGui::End();
        }

        // Rendering
        ImGui::EndFrame();
        
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * 255.0f), (int)(clear_color.y * 255.0f), (int)(clear_color.z * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
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
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

