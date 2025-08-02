// Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include <d3d9.h>
#include <tchar.h>
#include <string>
#include <vector>
#define random_f (float a, float b) rand() % (b - a + 1) + a;
template<class T, class U>
static T clamp(const T& in, const U& low, const U& high)
{
    if (in <= low)
        return low;

    if (in >= high)
        return high;

    return in;
}
// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
// Main code
#define IM_USE using namespace ImGui;
static int tab = 0;
#include <math.h>
#include "Header.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "Elements.h"
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")
void CircularProgressBar(int x, int y, int r1, int r2, int s, int d, ImColor col, bool invert = false)
{
    for (int i = s; i < s + d; i++)
    {
        auto rad = i * 3.1415 / 180;
        if (!invert)
           ImGui::GetOverlayDrawList()->AddLine(ImVec2(x + cos(rad) * r1, y + sin(rad) * r1), ImVec2(x + cos(rad) * r2, y + sin(rad) * r2), col);
        else
            ImGui::GetOverlayDrawList()->AddLine(ImVec2(x - cos(rad) * r1, y - sin(rad) * r1), ImVec2(x - cos(rad) * r2, y - sin(rad) * r2), col);
    }
}

void FadedCircle(int x, int y, int radius, ImColor color) {
    auto step = color.Value.w / (radius * 0.5);

    for (int i = 5; i <= radius; i++) {
        ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), 15 + i, ImColor(color.Value.x, color.Value.y, color.Value.z, color.Value.w - step * i));
    }
}

#include <iostream>
#include <chrono>
#include <ctime> 
int rand_between(const int from, const int to) {
    if (to == from)
        return to;
    if (to < from)
        return rand_between(to, from);
    return from + rand() % (to - from + 1);
}
void CurPosx(int pos)
{
    ImGui::SetCursorPosX(pos);
}
void CurPosy(int pos)
{
    ImGui::SetCursorPosY(pos);
}
void CurPos(ImVec2 pos)
{
    ImGui::SetCursorPos(pos);
}

bool gui_key(const char* key_code)
{
    static bool return_me = false;
    ImGui::SameLine(ImGui::GetWindowSize().x - (ImGui::CalcTextSize(key_code).x + 12));
    ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    if (ImGui::Button(key_code, ImVec2(ImGui::CalcTextSize(key_code).x + 4, ImGui::CalcTextSize(key_code).y + 2)))return_me = true;
    ImGui::PopStyleColor(3);
    return return_me;
}
#include "imgui_internal.h"
void decoration(float alpha, ImVec2 p, ImVec2 s)
{
    auto draw = ImGui::GetWindowDrawList();
    //Background
    draw->AddRectFilled(ImVec2(p.x - 1, p.y - 1), ImVec2(p.x + s.x + 1, p.y + s.y + 1), ImColor(57, 71, 159, int(35 * alpha)), 4);
    draw->AddRectFilled(ImVec2(p.x - 2, p.y - 2), ImVec2(p.x + s.x + 2, p.y + s.y + 2), ImColor(57, 71, 159, int(25 * alpha)), 4);
    draw->AddRectFilled(ImVec2(p.x - 3, p.y - 3), ImVec2(p.x + s.x + 3, p.y + s.y + 3), ImColor(57, 71, 159, int(15 * alpha)), 4);

    draw->AddRectFilled(ImVec2(p.x + 1, p.y + 1), ImVec2(p.x + s.x, p.y + s.y), ImColor(18, 25, 35, int(255 * alpha)), 4);
    //Left panel main
    draw->AddRectFilled(ImVec2(p.x, p.y + 50), ImVec2(p.x + 50, p.y + s.y), ImColor(28, 37, 54, int(255 * alpha)), 4, ImDrawFlags_RoundCornersBottomLeft);
    //Upper panel main
    draw->AddRectFilled(ImVec2(p.x + 50, p.y), ImVec2(p.x + s.x, p.y + 50), ImColor(28, 37, 54, int(255 * alpha)), 4, ImDrawFlags_RoundCornersTopRight);
    //Logo
    draw->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + 50, p.y + 50), ImColor(7, 152, 245, int(255 * alpha)), 4, ImDrawFlags_RoundCornersTopLeft);
    draw->AddRectFilledMultiColor(ImVec2(p.x + 5, p.y + 5), ImVec2(p.x + 50, p.y + 50), ImColor(7, 152, 245, int(255 * alpha)), ImColor(57, 71, 159, int(255 * alpha)), ImColor(57, 71, 159, int(255 * alpha)), ImColor(57, 71, 159, int(255 * alpha)));
    draw->AddRectFilledMultiColor(ImVec2(p.x, p.y + 5), ImVec2(p.x + 5, p.y + 50), ImColor(7, 152, 245, int(255 * alpha)), ImColor(7, 152, 245, int(255 * alpha)), ImColor(57, 71, 159, int(255 * alpha)), ImColor(57, 71, 159, int(255 * alpha)));
    draw->AddRectFilledMultiColor(ImVec2(p.x + 5, p.y), ImVec2(p.x + 50, p.y + 5), ImColor(7, 152, 245, int(255 * alpha)), ImColor(57, 71, 159, int(255 * alpha)), ImColor(57, 71, 159, int(255 * alpha)), ImColor(7, 152, 245, int(255 * alpha)));
    draw->AddLine(ImVec2(p.x, p.y + 50), ImVec2(p.x + s.x, p.y + 50), ImColor(5, 25, 35, int(85 * alpha)));
    draw->AddLine(ImVec2(p.x + 50, p.y), ImVec2(p.x + 50, p.y + s.y), ImColor(5, 25, 35, int(85 * alpha)));
}
//MAIN COLOR: 112, 146, 190
int main(int, char**)
{
   
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX9 Example"), WS_POPUP, 0, 0, 1366, 768, NULL, NULL, wc.hInstance, NULL);

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
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);
    LPDIRECT3DTEXTURE9 lg = nullptr;
    if (lg == nullptr) {
        D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &logo, sizeof(logo), &lg);
    }
    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
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
    ImFont* name = io.Fonts->AddFontFromFileTTF("../../misc/fonts/Museo Sans Cyrl 700.ttf", 11.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    ImFont* isis = io.Fonts->AddFontFromFileTTF("../../misc/fonts/Museo Sans Cyrl 700.ttf", 24.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    ImFont* icon_font = io.Fonts->AddFontFromMemoryTTF((void*)Icons, sizeof(Icons), 24, &font_config, io.Fonts->GetGlyphRangesDefault());
    ImFont* weapon_icons = io.Fonts->AddFontFromMemoryTTF((void*)weapons_font, sizeof(weapons_font), 18, &font_config, io.Fonts->GetGlyphRangesDefault());
    ImFont* font_desc = io.Fonts->AddFontFromFileTTF("../../misc/fonts/MuseoSansCyrl_0.otf", 11.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    ImFont* font = io.Fonts->AddFontFromFileTTF("../../misc/fonts/MuseoSansCyrl_1.otf", 14.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    ImFont* GuiIcons = io.Fonts->AddFontFromFileTTF("../../misc/fonts/GuiIcons.ttf", 14.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
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
        // Rendering
        ImVec2 s, p;
        static int tab = 0;
        auto pad = ImGui::GetStyle().WindowPadding;
        //static float gui_alpha = ImClamp(gui_alpha + (3.15f * ImGui::GetIO().DeltaTime * (gui_open ? 1.f : -1.0f)), 0.f, 1.f);
        ImGui::SetNextWindowSize(ImVec2(760 + pad.x * 2, 540 + pad.y * 2));
        ImGui::Begin("Main", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        {
            auto alpha = ImGui::GetStyle().Alpha; auto draw = ImGui::GetWindowDrawList(); s = ImVec2(ImGui::GetWindowSize().x - pad.x * 2, ImGui::GetWindowSize().y - pad.y * 2); p = ImVec2(ImGui::GetWindowPos().x + pad.x, ImGui::GetWindowPos().y + pad.y);
            decoration(alpha, p, s);

            //tabs
            {
                ImGui::SetCursorPosY(50 + pad.y);
                ImGui::BeginGroup();
                {
                    
                    ImGui::PushFont(icon_font);
                    if (ImGui::tab("R", "Ragebot", tab == 0)) tab = 0;
                    if (ImGui::tab("A", "Antiaim", tab == 1)) tab = 1;
                    if (ImGui::tab("L", "Legitbot", tab == 2)) tab = 2;
                    if (ImGui::tab("P", "Players", tab == 3)) tab = 3;
                    if (ImGui::tab("W", "Visuals", tab == 4)) tab = 4;
                    if (ImGui::tab("I", "Skins", tab == 5)) tab = 5;
                    if (ImGui::tab("M", "Misc", tab == 6)) tab = 6;
                    if (ImGui::tab("C", "Configs", tab == 7)) tab = 7;
                    if (ImGui::tab("S", "Scripts", tab == 8)) tab = 8;
                    ImGui::PopFont();
                }
                ImGui::EndGroup();
            }
            switch (tab)
            {
            case 0:
            {
                ImGui::SetCursorPos(ImVec2(60 + pad.x, 75 + pad.y));
                ImGui::MenuChild("Accuracy", ImVec2(340, 475));
                {
                    static bool one, two = false; static int sl = 0;
                    ImGui::Checkbox("Checkbox", &one);
                    ImGui::Checkbox("Checkbox disabled", &two);
                    ImGui::SliderInt("Slider", &sl, 0, 100);
                }
                ImGui::EndMenuChild();
                ImGui::SetCursorPos(ImVec2(410 + pad.x, 75 + pad.y));
                ImGui::MenuChild("Selection", ImVec2(340, 475));
                {

                }
                ImGui::EndMenuChild();
            }break;
            }

        }
        ImGui::End();
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, 0);
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
