// Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <vector>
#include <string>

#include "globals.h"
#include "custom.h"
#include "bytearray.h"

#include <d3dx9tex.h>
#pragma comment(lib, "d3dx9.lib")

// Defines
#define ALPHA (ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar)
#define NO_ALPHA (ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar)

// Fonts
ImFont* tab_title;
ImFont* font_icon;
ImFont* poppins;

// Byte
IDirect3DTexture9* bgs = nullptr;
IDirect3DTexture9* logo = nullptr;
IDirect3DTexture9* logotwo = nullptr;
IDirect3DTexture9* foto_user = nullptr;

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static int m_tabs;
static bool toggled = true;
static float open_alpha = 0;

namespace render_ui
{
    void rage_tab()
    {
        ImGui::SetCursorPos(ImVec2(169, 38));
        ImGui::MenuChild("Override config", ImVec2(320, 57));
        {
            const char* type[] = { "Auto Snipers (G3SG1, SCAR-20, SC...)", "Types", "Select" };
            static int combo;
 
            ImGui::Combo(" ", &combo, type, IM_ARRAYSIZE(type));
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(169, 134));
        ImGui::MenuChild("General", ImVec2(320, 240));
        {
            static bool active = true;
            static bool inactive = false;

            ImGui::Spacing();
            ImGui::Checkbox("Enable Particles", &particles);

            static int procent = 60;
            ImGui::SliderInt("Fov", &procent, 0, 100, "%d%%");

            ImGui::Checkbox("Autofire", &inactive);
            ImGui::Checkbox("Autowall", &active);
            ImGui::Checkbox("Silent aim", &active);
            ImGui::Checkbox("Quick peek", &active);
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(169, 413));
        ImGui::MenuChild("Exploits", ImVec2(320, 107));
        {
            static bool active = true;
            static bool inactive = false;

            ImGui::Spacing();
            ImGui::Checkbox("Lag Peek", &active);
            ImGui::Checkbox("Hide Shots", &active);
            ImGui::Checkbox("Double Tab", &active);
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(505, 38));
        ImGui::MenuChild("Accuracy", ImVec2(320, 286));
        {
            static bool active = true;
            static bool inactive = false;

            const char* type[] = { "Type", "Types", "Select" };
            static int combo;

            ImGui::Spacing();
            ImGui::Checkbox("Automatic stop", &active);

            ImGui::Combo("Combo", &combo, type, IM_ARRAYSIZE(type));

            ImGui::Checkbox("Hitchance", &active);

            static int procentw = 60;
            ImGui::SliderInt("Hitchance value", &procentw, 0, 100, "%d%%");

            static int procente = 20;
            ImGui::SliderInt("Damage value", &procente, 0, 100, "%d%%");

            static int procent = 56;
            ImGui::SliderInt("Damage override value", &procent, 0, 100, "%d%%");
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(505, 363));
        ImGui::MenuChild("Misc", ImVec2(320, 157));
        {
            static bool active = true;
            static bool inactive = false;

            ImGui::Spacing();
            ImGui::Checkbox("Prefer a pont", &inactive);
            ImGui::Checkbox("Prefer body aim", &active);
            ImGui::Checkbox("ignore limbs when moving", &active);
        }
        ImGui::EndChild();

    }

    void Particles()
    {
        if(!particles)
           return;

        ImVec2 screen_size = { (float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN) };

        static ImVec2 partile_pos[100];
        static ImVec2 partile_target_pos[100];
        static float partile_speed[100];
        static float partile_radius[100];


        for (int i = 1; i < 50; i++)
        {
            if (partile_pos[i].x == 0 || partile_pos[i].y == 0)
            {
                partile_pos[i].x = rand() % (int)screen_size.x + 1;
                partile_pos[i].y = 15.f;
                partile_speed[i] = 1 + rand() % 25;
                partile_radius[i] = rand() % 4;

                partile_target_pos[i].x = rand() % (int)screen_size.x;
                partile_target_pos[i].y = screen_size.y * 2;
            }

            partile_pos[i] = ImLerp(partile_pos[i], partile_target_pos[i], ImGui::GetIO().DeltaTime * (partile_speed[i] / 60));

            if (partile_pos[i].y > screen_size.y)
            {
                partile_pos[i].x = 0;
                partile_pos[i].y = 0;
            }

            ImGui::GetWindowDrawList()->AddCircleFilled(partile_pos[i], partile_radius[i], ImColor(174, 139, 148, 255 / 2));
        }

    }

    void RenderTab()
    {
        auto draw = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetWindowPos();

        draw->AddText(poppins, 17, ImVec2(pos.x + 13, pos.y + 81), ImColor(105, 105, 105, int(255 * ImGui::GetStyle().Alpha)), "Aimbot");  // right bg
        draw->AddText(poppins, 17, ImVec2(pos.x + 13, pos.y + 210), ImColor(105, 105, 105, int(255 * ImGui::GetStyle().Alpha)), "Visuals");  // right bg
        draw->AddText(poppins, 17, ImVec2(pos.x + 13, pos.y + 348), ImColor(105, 105, 105, int(255 * ImGui::GetStyle().Alpha)), "Miscellaneous");  // right bg

        ImGui::SetCursorPos(ImVec2(13, 99));
        if (ImGui::Rendertab("r", "Legit Bot", !m_tabs)) m_tabs = 0;

        ImGui::SetCursorPos(ImVec2(13, 136));
        if (ImGui::Rendertab("e", "Rage Bot", m_tabs == 1)) m_tabs = 1;

        ImGui::SetCursorPos(ImVec2(13, 174));
        if (ImGui::Rendertab("a", "Anti-Aims", m_tabs == 2)) m_tabs = 2;

        ImGui::SetCursorPos(ImVec2(13, 228));
        if (ImGui::Rendertab("x", "Players", m_tabs == 3)) m_tabs = 3;

        ImGui::SetCursorPos(ImVec2(13, 266));
        if (ImGui::Rendertab("w", "World", m_tabs == 4)) m_tabs = 4;

        ImGui::SetCursorPos(ImVec2(13, 304));
        if (ImGui::Rendertab("v", "View", m_tabs == 5)) m_tabs = 5;

        ImGui::SetCursorPos(ImVec2(13, 369));
        if (ImGui::Rendertab("z", "Main", m_tabs == 6)) m_tabs = 6;

        ImGui::SetCursorPos(ImVec2(13, 407));
        if (ImGui::Rendertab("s", "Inventory", m_tabs == 7)) m_tabs = 7;

        ImGui::SetCursorPos(ImVec2(13, 445));
        if (ImGui::Rendertab("c", "Configs", m_tabs == 8)) m_tabs = 8;

        switch (m_tabs)
        {
        case 0: rage_tab(); break; // RAGEBOT
        case 1: break; // LEGITBOT
        case 2: break; // PLAYERS
        case 3: break; // VISUALS
        case 4: break; // MISC
        case 5: break; // SETTINGS
        }
    }

    void user_info()
    {
        auto draw = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetWindowPos();

        draw->AddRectFilled(ImVec2(pos.x + 9, pos.y + 486), ImVec2(pos.x + 152, pos.y + 523), ImColor(41, 41, 41, int(255 * ImGui::GetStyle().Alpha)), 5.f, ImDrawCornerFlags_All); // right bg
        draw->AddRect(ImVec2(pos.x + 9, pos.y + 486), ImVec2(pos.x + 152, pos.y + 523), ImColor(50, 50, 50, int(255 * ImGui::GetStyle().Alpha)), 5.f, ImDrawCornerFlags_All, 1);  // right bg

        //парсим с сайта
        draw->AddColoredText(ImVec2(pos.x + 49, pos.y + 488), ImColor(105, 105, 105, int(255 * ImGui::GetStyle().Alpha)), ImColor(255, 255, 255, int(255 * ImGui::GetStyle().Alpha)), "(bouy)");
        draw->AddColoredText(ImVec2(pos.x + 49, pos.y + 503), ImColor(105, 105, 105, int(255 * ImGui::GetStyle().Alpha)), ImColor(255, 255, 255, int(255 * ImGui::GetStyle().Alpha)), "Till: (25.09.2023)");

        ImGui::SetCursorPos(ImVec2(15, 490));
        ImGui::Image(foto_user, ImVec2(28, 28));//logo 2
    }

    void Decoration()
    {
        auto draw = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetWindowPos();

        draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 161, pos.y + 535), ImColor(32, 32, 32, int(255 * ImGui::GetStyle().Alpha)), 10.f, ImDrawCornerFlags_Left); // left bg
        draw->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + 161, pos.y + 535), ImColor(50, 50, 50, int(255 * ImGui::GetStyle().Alpha)), 10.f, ImDrawCornerFlags_Left, 1);  // left bg

        draw->AddRectFilled(ImVec2(pos.x + 160, pos.y), ImVec2(pos.x + 838, pos.y + 535), ImColor(26, 26, 26, int(255 * ImGui::GetStyle().Alpha)), 10.f, ImDrawCornerFlags_Right); // right bg
        draw->AddRect(ImVec2(pos.x + 160, pos.y), ImVec2(pos.x + 838, pos.y + 535), ImColor(50, 50, 50, int(255 * ImGui::GetStyle().Alpha)), 10.f, ImDrawCornerFlags_Right, 1);  // right bg

        // на будующие делаю 2 лого не ебу какое вам нужно
        ImGui::SetCursorPos(ImVec2(23, 30));
        ImGui::Image(logo, ImVec2(120, 25.1f));//logo 1

        ImGui::SetCursorPos(ImVec2(36, - 7));
        ImGui::Image(logotwo, ImVec2(85, 98));//logo 2
    }

    void Render()
    {
 
        if (GetAsyncKeyState(VK_INSERT) & 1)
            toggled = !toggled;

        open_alpha = ImClamp(open_alpha + (2.f * ImGui::GetIO().DeltaTime * (toggled ? 1.5f : -1.5f)), 0.f, 1.f);
        if (open_alpha > 0.01f)
        {
            ImGui::SetNextWindowSize(ImVec2(838 * dpi_scale, 535 * dpi_scale));
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, open_alpha);
            ImGui::Begin("Dear ImGui", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);
            {
                Decoration();
                RenderTab();
                user_info();

                Particles();
            }
            ImGui::End();
            ImGui::PopStyleVar();
        }
    }

};

// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX9 Example"), WS_POPUPWINDOW, 0, 0, 1920, 1080, NULL, NULL, wc.hInstance, NULL);

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

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    auto& style = ImGui::GetStyle();
    auto& color = style.Colors;

    style.FramePadding = ImVec2(1, 0);
    style.FrameRounding = 3;
    style.WindowBorderSize = 0;
    style.ScrollbarRounding = 3;
    style.ScrollbarSize = 5;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    ImFontConfig font_config;
    font_config.PixelSnapH = false;
    font_config.OversampleH = 5;
    font_config.OversampleV = 5;
    font_config.RasterizerMultiply = 1.2f;

    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0xE000, 0xE226, // icons
        0,
    };

    //Font
    font_config.GlyphRanges = ranges;

    io.Fonts->AddFontFromMemoryTTF(poppin_font, sizeof(poppin_font), 16, &font_config, ranges);
    tab_title = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 19.0f, &font_config, ranges);
    font_icon = io.Fonts->AddFontFromMemoryTTF(icon_font, sizeof(icon_font), 25.0f, &font_config, ranges);
    poppins = io.Fonts->AddFontFromMemoryTTF(poppin_font, sizeof(poppin_font), 25.0f, &font_config, ranges);

    //Byte
    if (bgs == nullptr)
        D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, bg_one, sizeof(bg_one), 1920, 1080, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &bgs);

    if (logo == nullptr)
        D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, logo_one, sizeof(logo_one), 120, 25.1, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &logo);

    if (logotwo == nullptr)
        D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, logo_two, sizeof(logo_two), 85, 98, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &logotwo);

    if (foto_user == nullptr)
        D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, user, sizeof(user), 28, 28, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &foto_user);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
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

        render_ui::Render();  //Render

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*clear_color.w*255.0f), (int)(clear_color.y*clear_color.w*255.0f), (int)(clear_color.z*clear_color.w*255.0f), (int)(clear_color.w*255.0f));
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
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
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
