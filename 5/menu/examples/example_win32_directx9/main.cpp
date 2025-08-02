// Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include "bgs.h"
#include "logotype.h"

#include "misc/fonts/Montserrat-Regular.h"

#include <vector>
#include <imgui_internal.h>
#include <examples/example_win32_directx9/Search.hpp>
#include "options.hpp"

ImFont* LexendDecaFont;
ImFont* IconFont;
ImFont* IconFontLogs;
ImFont* InterMedium;

static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

auto s = ImVec2{}, p = ImVec2{}, gs = ImVec2{ 1090, 1200 };

IDirect3DTexture9* bgs = nullptr;
IDirect3DTexture9* logo = nullptr;
IDirect3DTexture9* foto = nullptr;
IDirect3DTexture9* merfthefurry = nullptr;
IDirect3DTexture9* ghoul = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();

static int tab;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void legit()
{
    if (tab == 0)
    {
        ImGui::GetWindowDrawList()->AddText(InterMedium, 16, ImVec2(p.x + 315, p.y + 2), ImColor(255, 255, 255), "[ Legitbot ]");
    }

    ImGui::SetCursorPos(ImVec2(82, 61));
    ImGui::MenuChild("Globals", ImVec2(241, 329));
    {
       // ImGui::Spacing();
        ImGui::Spacing(); ImGui::Spacing();
        auto settings = &opt.ragebot[0];
        ImGui::Checkbox("Enable", &settings->enabled);
        ImGui::Checkbox("Attack team", &settings->enabled);
        ImGui::Checkbox("Smoke check", &settings->enabled);
        ImGui::Checkbox("Flash check", &settings->enabled);
        ImGui::Checkbox("Scope check", &settings->enabled);
        ImGui::Checkbox("Jump check", &settings->enabled);
        ImGui::Checkbox("Backtracking", &settings->enabled);
    }
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(330, 61));
    ImGui::MenuChild("Accuracy", ImVec2(241, 253));
    {
        ImGui::Spacing(); ImGui::Spacing();
        auto settings = &opt.ragebot[0];
        ImGui::SliderFloat("Smooth", &settings->fov, 1, 10, "%.3f");
        ImGui::Checkbox("RCS", &settings->enabled);
        ImGui::SliderFloat("RCS Pitch", &settings->fov, 1, 10, "%.3f");
        ImGui::SliderFloat("RCS Yaw", &settings->fov, 1, 10, "%.3f");
    }
    ImGui::EndChild();


    ImGui::SetCursorPos(ImVec2(576, 61));
    ImGui::MenuChild("Target", ImVec2(241, 456));
    {
        ImGui::Spacing(); ImGui::Spacing();
        auto settings = &opt.ragebot[0];
        ImGui::Checkbox("Recoil based FOV", &settings->enabled);
        ImGui::SliderFloat("FOV", &settings->fov, 1, 10, "%.3f");
        ImGui::SliderFloat("Shot delay", &settings->fov, 1, 10, "%.3f");
        ImGui::SliderFloat("Kill delay", &settings->fov, 1, 10, "%.3f");
    }
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(82, 414));
    ImGui::MenuChild("Current config", ImVec2(241, 102));
    {

    }
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(330, 338));
    ImGui::MenuChild("Extra", ImVec2(241, 178));
    {
        ImGui::Spacing(); ImGui::Spacing();
        auto settings = &opt.ragebot[0];
        ImGui::Checkbox("Auto pistol", &settings->enabled);
        ImGui::Checkbox("Silent aim", &settings->enabled);
        ImGui::Checkbox("Auto fire", &settings->enabled);
        ImGui::SliderFloat("Hitchance", &settings->fov, 1, 10, "%.3f");
    }
    ImGui::EndChild();
}

void render_tab()
{
    ImGui::PushFont(IconFont);
    {
        ImGui::SetCursorPos(ImVec2{ 19, 40 });
        if (ImGui::tab("Z", !tab)) tab = 0;

        ImGui::SetCursorPos(ImVec2{ 19, 85 });
        if (ImGui::tab("F", tab == 1)) tab = 1;
      
        ImGui::SetCursorPos(ImVec2{ 19, 130 });
        if (ImGui::tab("D", tab == 2)) tab = 2;
     
        ImGui::SetCursorPos(ImVec2{ 19, 175 });
        if (ImGui::tab("G", tab == 3)) tab = 3;
     
        ImGui::SetCursorPos(ImVec2{ 19, 220 });
        if (ImGui::tab("C", tab == 4)) tab = 4;

        ImGui::SetCursorPos(ImVec2{ 19, 265 });
        if (ImGui::tab("V", tab == 5)) tab = 5;

        ImGui::SetCursorPos(ImVec2{ 19, 310 });
        if (ImGui::tab("A", tab == 6)) tab = 6;
    }
    ImGui::PopFont();

    switch (tab)
    {
          case 0:     legit();       break;
    }
}

int main(int, char**)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX9 Example"), WS_POPUPWINDOW, 0, 0, 1920, 1080, NULL, NULL, wc.hInstance, NULL);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    if (bgs == nullptr)
        D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, bg_two, sizeof(bg_two), 1920, 1080, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &bgs);
    if (logo == nullptr)
        D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, logotype, sizeof(logotype), 90, 90, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &logo);
    if (foto == nullptr)
        D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, fototype, sizeof(fototype), 45, 45, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &foto);

    if (merfthefurry == nullptr)
        D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, merfthefurry_type, sizeof(merfthefurry_type), 18, 18, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &merfthefurry);

    if (ghoul == nullptr)
        D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, ghoul_type, sizeof(ghoul_type), 18, 18, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &ghoul);

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    ImFontConfig icons_config;
    IconFont = io.Fonts->AddFontFromMemoryTTF(Icons, sizeof(Icons), 27, NULL, io.Fonts->GetGlyphRangesCyrillic());
    IconFontLogs = io.Fonts->AddFontFromMemoryTTF(IconFontLog, sizeof(IconFontLog), 25, NULL, io.Fonts->GetGlyphRangesCyrillic());
    LexendDecaFont = io.Fonts->AddFontFromMemoryTTF(LexendDeca, sizeof(LexendDeca), 22, NULL, io.Fonts->GetGlyphRangesCyrillic());
    InterMedium = io.Fonts->AddFontFromMemoryTTF(Intermedium, sizeof(Intermedium), 17, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(-1, -1));
        ImGui::SetNextWindowSizeConstraints(ImVec2(w, h), ImVec2(w, h));
        ImGui::SetNextWindowPos({ 0,0 });
        ImGui::Begin("bg", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus);
        {
            auto Render = ImGui::GetWindowDrawList();
            ImVec2 p = ImGui::GetWindowPos();
            Render->AddImage(bgs, p, ImVec2(p.x + w, p.y + h));
        }
        ImGui::End();
        ImGui::PopStyleVar();

        {
            ImGui::SetNextWindowSize(ImVec2(gs));
            if (ImGui::Begin(("###"), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar))
            {
                s = ImVec2(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2);
                p = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y);

                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + 818, p.y + 21), ImColor(25, 25, 25));//bg upp
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + 818, p.y + 510), ImColor(15, 15, 15, 200));//bg
               
                //bg tabs
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x + 9, p.y + 30), ImVec2(p.x + 67, p.y + 501), ImColor(31, 31, 31));//bg
                ImGui::GetWindowDrawList()->AddRect(ImVec2(p.x + 9, p.y + 30), ImVec2(p.x + 67, p.y + 501), ImColor(21, 21, 21), true, 2, 2);

                ImGui::SetCursorPos(ImVec2(27, 465));
                ImGui::Image(logo, ImVec2(36, 37));//logo

                ImGui::GetWindowDrawList()->AddText(InterMedium , 16, ImVec2(p.x + 5, p.y + 2), ImColor(255, 255, 255), "Nixware.cc for Counter-Strike: Global Offensive");
             
                {
                    render_tab();
                }
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
