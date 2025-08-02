// Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include <d3d9.h>
#include <tchar.h>
#include <vector>
#include <string>
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

ImFont* font;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
// Main code
#define IM_USE using namespace ImGui;
static int tab = 0;
static int tab2 = 0;
#include <math.h>
#include "Header.h"
#include <d3d9.h>
#include <d3dx9.h>
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

namespace keybinds
{
    static float open_alpha = 0;
    static bool toggled = true;
    static float spacing;
    std::vector<std::pair<std::string, std::string>> vec = {
        {"Double tap", ""},
        {"Override damage", ""},
        {"Slow walk", ""},
    };
    void BindsList()
    {
        auto d = ImGui::GetWindowDrawList();
        auto p = ImGui::GetWindowPos();
        auto s = ImGui::GetWindowSize();
        auto style = ImGui::GetStyle();

        for (int i = 0; i < int(vec.size()); i++)
        {
            auto keybind = vec.at(i);
            d->AddText(font, font->FontSize, ImVec2(p.x + 5, p.y + 20 + (spacing * i)), ImColor(255, 255, 255, int(160 * open_alpha)), keybind.first.c_str());

            auto textSize = font->CalcTextSizeA(font->FontSize, FLT_MAX, NULL, keybind.second.c_str());
            d->AddText(font, font->FontSize, ImVec2(p.x + s.x - 5 - textSize.x, p.y + 20 + (spacing * i)), ImColor(255, 255, 255, int(160 * open_alpha)), keybind.second.c_str());
        }
    }

    void Decoration()
    {
        auto d = ImGui::GetWindowDrawList();
        auto p = ImGui::GetWindowPos();
        auto s = ImGui::GetWindowSize();

        d->AddRectFilled(ImVec2(p.x, p.y + 16), ImVec2(p.x + s.x, p.y + 25 + (spacing * vec.size())), ImColor(25, 25, 25, int(255 * open_alpha)));
        d->AddRectFilled(p, ImVec2(p.x + s.x, p.y + 16), ImColor(33, 33, 33, int(255 * open_alpha)));
        //d->AddRectFilled(ImVec2(p.x, p.y + 16), ImVec2(p.x + s.x, p.y + 15), ImColor(89, 113, 162, int(255 * open_alpha)));
        d->AddLine(ImVec2(p.x, p.y + 16), ImVec2(p.x + s.x, p.y + 16), ImColor(89, 113, 162, int(255 * open_alpha))); // line

        auto textSize = font->CalcTextSizeA(font->FontSize, FLT_MAX, NULL, "Keybinds");
        d->AddText(font, font->FontSize, ImVec2(p.x + (s.x - textSize.x) / 2, p.y + (15 - textSize.y) / 2), ImColor(255, 255, 255, int(160 * open_alpha)), "Keybinds");
    }

    void Render()
    {
        spacing = font->FontSize + 2;
        if (GetAsyncKeyState(VK_F7) & 1)
            toggled = !toggled;

        open_alpha = ImClamp(open_alpha + (2.f * ImGui::GetIO().DeltaTime * (toggled ? 0.5f : -1.5f)), 0.f, 1.f);
        if (open_alpha > 0.01f) {
            ImGui::SetNextWindowSize(ImVec2(150, 25 + (spacing * vec.size())));
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 150, ImGui::GetIO().DisplaySize.y / 2), ImGuiCond_Once);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, open_alpha);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Keybinds", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);
            {
                Decoration();
                BindsList();
            }
            ImGui::End();
            ImGui::PopStyleVar(2);
        }
    }
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
    //ImGui::StyleColorsClassic();

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
    ImFont* name = io.Fonts->AddFontFromFileTTF("../../misc/fonts/Museo Sans Cyrl 700.ttf", 19.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    ImFont* icons = io.Fonts->AddFontFromMemoryTTF((void*)icon, sizeof(icon), 44, &font_config, ranges);
    ImFont* weapon_icons = io.Fonts->AddFontFromFileTTF("../../misc/fonts/icomoon.ttf", 38.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    font = io.Fonts->AddFontFromFileTTF("../../misc/fonts/Museo Sans Cyrl 700.ttf", 13.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.51f, 0.55f, 0.57f, 1.00f);

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
        static bool one = true;
        static bool two = false;
        static int sl = 0;

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        keybinds::Render();
        auto s = ImVec2{}, p = ImVec2{}, gs = ImVec2{620, 460};
        ImGui::SetNextWindowSize(ImVec2(gs));
        ImGui::Begin("##GUI", NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground);
        {
            {//draw
                s = ImVec2(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2); p = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y); auto draw = ImGui::GetWindowDrawList();
                draw->AddRectFilled(p, ImVec2(p.x + s.x, p.y + s.y), ImColor(33, 33, 33)); //tabs
                draw->AddRectFilled(ImVec2(p.x, p.y + 25), ImVec2(p.x + s.x, p.y + s.y - 25), ImColor(25, 25 ,25));

                draw->AddLine(ImVec2(p.x, p.y + 25), ImVec2(p.x + s.x, p.y + 25), ImColor(89, 113, 162)); // tab seperator

                draw->AddLine(ImVec2(p.x, p.y + s.y - 25), ImVec2(p.x + s.x, p.y +s.y - 25), ImColor(89, 113, 162)); // bottom seperator
            }
            {//tabs
                ImGui::PushFont(font);
                ImGui::SetCursorPosX(20);
                ImGui::SetCursorPosY(10);
                ImGui::BeginGroup();
                if (ImGui::tab("Legit", tab == 0))tab = 0; ImGui::SameLine();
                if (ImGui::tab("Rage", tab == 1))tab = 1; ImGui::SameLine();
                if (ImGui::tab("Anti Aim", tab == 2))tab = 2; ImGui::SameLine();
                if (ImGui::tab("Visuals", tab == 3))tab = 3; ImGui::SameLine();
                if (ImGui::tab("Misc", tab == 4))tab = 4; ImGui::SameLine();
                if (ImGui::tab("Skins", tab == 5))tab = 5; ImGui::SameLine();
                if (ImGui::tab("Config", tab == 6))tab = 6;
                ImGui::EndGroup();
                ImGui::PopFont();
            }
            {//content
                if (tab == 1)
                {
                    ImGui::PushFont(font);
                    {//left upper
                        ImGui::SetCursorPosY(35);
                        ImGui::BeginGroup();
                        ImGui::SetCursorPosX(15);
                        ImGui::MenuChild("General", ImVec2(300, 205), false);
                        {
                            
                        }
                        ImGui::EndChild();
                        ImGui::EndGroup();
                    }
                    {//left bottom
                        ImGui::SetCursorPosY(240);
                        ImGui::BeginGroup();
                        ImGui::SetCursorPosX(15);
                        ImGui::MenuChild("Other", ImVec2(300, 180), false);
                        {

                        }
                        ImGui::EndChild();
                        ImGui::EndGroup();
                    }
                    {//right
                        ImGui::SetCursorPosY(35);
                        ImGui::BeginGroup();
                        ImGui::SetCursorPosX(320);
                        ImGui::MenuChild("Settings", ImVec2(285, 385), false);
                        {
                            
                        }
                        ImGui::EndChild();
                        ImGui::EndGroup();
                        ImGui::PopFont();
                    }
                }
            }
        }
        ImGui::End();
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
