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
int tab = 0;
int subtab = 0;
#include <math.h>
#include "Header.h"
#include "bgs.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "Elements.h"
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")
#include <map>
#include <deque>
#include <iostream>
#include <chrono>
#include <ctime> 
#include "imgui_internal.h"

enum AnimationTypes { STATIC, DYNAMIC, INTERP };

std::string pChar(std::string first_, std::string second_) {
    return (first_ + second_);
}

float Animate(const char* label, const char* second_label, bool if_, float Maximal_, float Speed_, int type) {

    auto ID = ImGui::GetID(pChar(label, second_label).c_str());

    static std::map<ImGuiID, float> pValue;

    auto this_e = pValue.find(ID);

    if (this_e == pValue.end()) {
        pValue.insert({ ID, 0.f });
        this_e = pValue.find(ID);
    }

    switch (type) {

    case DYNAMIC: {
        if (if_) //do
            this_e->second += abs(Maximal_ - this_e->second) / Speed_;
        else
            this_e->second -= (0 + this_e->second) / Speed_;
    }
                break;

    case INTERP: {
        if (if_) //do
            this_e->second += (Maximal_ - this_e->second) / Speed_;
        else
            this_e->second -= (0 + this_e->second) / Speed_;
    }
               break;

    case STATIC: {
        if (if_) //do
            this_e->second += Speed_;
        else
            this_e->second -= Speed_;
    }
               break;
    }

    {
        if (this_e->second < 0.f)
            this_e->second = 0.f;
        else if (this_e->second > Maximal_)
            this_e->second = Maximal_;
    }

    return this_e->second;

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
    if (ImGui::GetIO().Maintexture == nullptr) {
        D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &main_texture, sizeof(main_texture), &ImGui::GetIO().Maintexture);
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
    ImFont* name = io.Fonts->AddFontFromFileTTF("../../misc/fonts/MuseoSansCyrl_1.otf", 22.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    ImFont* isis = io.Fonts->AddFontFromFileTTF("../../misc/fonts/MuseoSansCyrl_1.otf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    ImFont* icon_font = io.Fonts->AddFontFromMemoryTTF((void*)Icons, sizeof(Icons), 18, &font_config, io.Fonts->GetGlyphRangesDefault());
    ImFont* icon_font_big = io.Fonts->AddFontFromMemoryTTF((void*)Icons, sizeof(Icons), 32, &font_config, io.Fonts->GetGlyphRangesDefault());
    ImFont* weapon_icons = io.Fonts->AddFontFromMemoryTTF((void*)weapons_font, sizeof(weapons_font), 28, &font_config, io.Fonts->GetGlyphRangesDefault());
    ImFont* font_desc = io.Fonts->AddFontFromFileTTF("../../misc/fonts/MuseoSansCyrl_1.otf", 11.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    ImFont* font = io.Fonts->AddFontFromFileTTF("../../misc/fonts/MuseoSansCyrl_2.otf", 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ImFont* mainfont = io.Fonts->AddFontFromFileTTF("../../misc/fonts/MuseoSansCyrl_1.otf", 14.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ImFont* GuiIcons = io.Fonts->AddFontFromFileTTF("../../misc/fonts/Icons.ttf", 25.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.36f, 0.35f, 0.37f, 1.00f);
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

        int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(-1, -1));
        ImGui::SetNextWindowSizeConstraints(ImVec2(w, h), ImVec2(w, h));
        ImGui::SetNextWindowPos({ 0,0 });
        ImGui::Begin("bg", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_::ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
        {

            auto Render = ImGui::GetWindowDrawList();
            ImVec2 p = ImGui::GetWindowPos();
            
        }
        ImGui::End();
        ImGui::PopStyleVar();
        static bool keybinds[5], spectating[5]; 
        int alpha = Animate("GUI", "Main alpha", GetKeyState(VK_INSERT), 270, 7.5f, INTERP);
        alpha = ImClamp(alpha, 0, 255);
        ImGui::GetIO().Font = font;
        ImGui::GetIO().FontIcons = icon_font;
        ImGui::GetIO().FontIconsBig = icon_font_big;
        ImGui::GetIO().FontHeader = name;
        ImGui::GetIO().FontName = isis;
        ImGui::GetIO().FontDesc = font_desc;
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha / 255.f);
        ImGui::SetNextWindowSize(ImVec2(690, 470));
        ImGui::MainBegin("##GUI", nullptr, ImGuiWindowFlags_NoDecoration);
        {
            ImGui::GetIO().OriginalWindowPos = ImGui::GetWindowPos();
            ImGui::SetCursorPos({0, 40});
            ImGui::BeginGroup();//SUBTABS
            {
                if (ImGui::subtab("Q", "Main", subtab == 0))subtab = 0;
                ImGui::SetCursorPosY(80);
                if (ImGui::subtab("F", "Weapons", subtab == 1))subtab = 1;
                ImGui::SetCursorPosY(120);
                if (ImGui::subtab("B", "Anti-aim", subtab == 2))subtab = 2;
                ImGui::SetCursorPosY(160);
                if (ImGui::subtab("D", "Misc", subtab == 3))subtab = 3;
            }
            ImGui::EndGroup();

            ImGui::SetCursorPos({ 25, 430 });
            ImGui::BeginGroup();//TABS
            {
                if (ImGui::tab("A", "RAGE", tab == 0))tab = 0; ImGui::SameLine(0, 2);
                if (ImGui::tab("H", "LEGIT", tab == 1))tab = 1; ImGui::SameLine(0, 2);
                if (ImGui::tab("K", "VISUALS", tab == 2))tab = 2; ImGui::SameLine(0, 2);
                if (ImGui::tab("C", "SKINS", tab == 3))tab = 3; ImGui::SameLine(0, 2);
                if (ImGui::tab("Q", "MISC", tab == 4))tab = 4; ImGui::SameLine(0, 2);
                if (ImGui::tab("I", "CONFIGS", tab == 5))tab = 5; ImGui::SameLine(0, 8);
                if (ImGui::tab("O", "LUA", tab == 6))tab = 6; ImGui::SameLine(0, 2);
            }
            ImGui::EndGroup();
            
            ImGui::SetCursorPos({ 128, 40 });
            ImGui::BeginChild("##content", ImVec2(560, 370), true, ImGuiWindowFlags_NoDecoration);
            {
                ImGui::PushFont(mainfont);
                ImGui::SetCursorPos({ 10, 42 });
                ImGui::MenuChild("GENERAL", ImVec2(266, 122));
                {
                    if (tab == 0) {
                    static bool one, two;
                    ImGui::Checkbox("Enable", &one);
                    ImGui::Checkbox("Enable ", &two);
                    const char* texts[2] = { "Value", "value" }; static int combo_value = 0;
                    ImGui::Combo("Combobox", &combo_value, texts, 2);
                    }
                    else
                    {
                        for (int i = 0; i < 5; i++)
                        {
                            ImGui::Checkbox(std::string("Spect " + std::to_string(i)).c_str(), &spectating[i]);
                        }
                    }
                }
                ImGui::EndMenuChild();
                ImGui::SameLine(0, 8);
                ImGui::MenuChild("EXPLOITS", ImVec2(266, 108));
                {
                    char texts[24] = "TEXT"; static int combo_value = 0;
                    ImGui::InputText("Textfield", texts, IM_ARRAYSIZE(texts));
                    static int slider_value = 1;
                    ImGui::SliderInt("Slider", &slider_value, 1, 100, "%d%%");
                }
                ImGui::EndMenuChild();

                ImGui::SetCursorPos({ 10, 206 });
                ImGui::MenuChild("OTHER", ImVec2(266, 98));
                {
                    static float color[4] = {1.f, 1.f, 1.f, 1.f};
                    ImGui::SetCursorPosX(15); ImGui::Text("Colorpicker"); ImGui::SameLine();
                    ImGui::ColorEdit4("##coloredit", color);

                    ImGui::SetCursorPosX(15); ImGui::Text("Binder"); ImGui::SameLine();
                    ImGui::KeybindButton("Shift", "##eeeee", ImVec2(56, 23));
                }
                ImGui::EndMenuChild();
                ImGui::SetCursorPos({ 284, 192 });
                ImGui::MenuChild("ACCURACY", ImVec2(266, 140));
                {
                    if (tab == 0) {
                        static int v = 0;
                        if (ImGui::RadioButton("Default", v == 0)) v = 0;
                        if (ImGui::RadioButton("Skating", v == 1)) v = 1;
                        if (ImGui::RadioButton("Walking", v == 2)) v = 2;
                        if (ImGui::RadioButton("Opposite", v == 3)) v = 3;
                    }
                    else
                    {
                        for (int i = 0; i < 5; i++)
                        {
                            ImGui::Checkbox(std::string("Key " + std::to_string(i)).c_str(), &keybinds[i]);
                        }
                    }
                }
                ImGui::EndMenuChild();
                ImGui::PopFont();
            }
            ImGui::EndChild();
        }   
        ImGui::End();
        ImGui::PopStyleVar();

        std::vector<std::string> keysss;
        ImGui::Begin("##KEYS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_UnsavedDocument | ImGuiWindowFlags_KeybindWindow);
        {
            const char* keys[5] = {"Double-tap", "Hide-Shots", "Body-aim", "Safe-points", "Auto-peek"};
            for (int i = 0; i < 5; i++)
            {
                if (keybinds[i])
                    keysss.push_back(keys[i]);
            }
            static int current_size; int max_size; max_size = 30 + 20 * keysss.size();
            if (current_size < max_size)
                current_size++;
            else if (current_size > max_size)
                current_size--;
            ImGui::SetWindowSize(ImVec2(160, current_size));
            int spacing = 0;
            ImGui::SetCursorPosY(35);
            ImGui::BeginGroup();
            ImGui::PushFont(mainfont);
            for (int i = 0; i < keysss.size(); i++)
            {
                ImGui::Text(keysss.at(i).c_str());
            }
            ImGui::PopFont();
            ImGui::EndGroup();
        }
        ImGui::End();

        std::vector<std::string> spects;
        ImGui::Begin("##SPEC", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_UnsavedDocument);
        {
            const char* names[5] = { "Jonh", "Oscar", "Kate", "Insultik", "Zavodnoy krokodil" };
            for (int i = 0; i < 5; i++)
            {
                if (spectating[i])
                    spects.push_back(names[i]);
            }
            static int current_size; int max_size; max_size = 30 + 20 * spects.size();
            if (current_size < max_size)
                current_size++;
            else if (current_size > max_size)
                current_size--;
            ImGui::SetWindowSize(ImVec2(160, current_size));
            int spacing = 0;
            ImGui::SetCursorPosY(35);
            ImGui::BeginGroup();
            ImGui::PushFont(mainfont);
            for (int i = 0; i < spects.size(); i++)
            {
                ImGui::Text(spects.at(i).c_str());
            }
            ImGui::PopFont();
            ImGui::EndGroup();
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
