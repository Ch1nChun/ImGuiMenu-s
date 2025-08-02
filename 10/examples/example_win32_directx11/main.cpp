// Dear ImGui: standalone example application for DirectX 11

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp
#include <windows.h>
#include <mmsystem.h>
#include "menu.h"
#include "Icons.h"
#include "regular.h"
#include <imgui_internal.h>
#include <map>
#include "intro.h"
#include "Gizzard.h"
#include <d3d11.h>
#include <D3DX11.h>
#include <chrono>
#pragma comment (lib, "d3dx11.lib")
#pragma comment(lib, "winmm.lib")
//#pragma comment(lib, "dwmapi.lib")

// Data
static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

ID3D11ShaderResourceView* Gizzard = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool watermark;
bool keybinds;
bool spectatorlist;
int alphaWidgets = 215;
int active_tab = 1;
int combo_test;
int event_logs;

bool const1 = false;
bool const2_ready = false;
bool const2 = false;
bool isIntro = true;
bool isMusic = false;
float anim_y = 0.f;
float gizzard_y = 0;
auto startTime = std::chrono::steady_clock::now();
float rotation = 0.0f;

const char* combo_ctr[] = { "Head", "Body", "Nonagon" };

//CODED BY MON4IK
int rotation_start_index;
void ImRotateStart()
{
    rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

ImVec2 ImRotationCenter()
{
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

    const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
}

ImVec2 operator-(const ImVec2& l, const ImVec2& r) { return{ l.x - r.x, l.y - r.y }; }

void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
{
    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

void EventLogs()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(350, io.DisplaySize.y));
    ImGui::Begin("intro by mon4ik", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
    {
        auto window = ImGui::GetCurrentWindow();
        auto draw = window->DrawList;
        auto pos = ImGui::GetWindowPos();
        auto size = ImGui::GetWindowSize();

        for (int i = 0; i < event_logs; i++)
        {
            ImGui::PushFont(menu.smallFont);
            ImGui::TextColored(ImVec4(1.0, 0.5, 0.5, 1.0), "[GILA]"); ImGui::SameLine();
            ImGui::Text("hit King Gizzard | hitbox: head | bc: 1 | damage: 100", i);
            ImGui::Spacing();
            ImGui::PopFont();
        }
    }
    ImGui::End();
}


void Intro(const char* label)
{
    ImGuiIO& io = ImGui::GetIO();
    if (!isMusic)
    {
        PlaySoundA((LPCSTR)introGizzard, NULL, SND_MEMORY | SND_ASYNC);
        isMusic = true;
    }
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    ImGui::Begin("intro by mon4ik", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
    {
        auto window = ImGui::GetCurrentWindow();
        auto draw = window->DrawList;
        auto pos = ImGui::GetWindowPos();
        auto size = ImGui::GetWindowSize();
        auto style = ImGui::GetStyle();

        const ImGuiID id = window->GetID(label);
        static std::map<ImGuiID, int> alpha;
        auto it_alpha = alpha.find(id);

        static std::map<ImGuiID, int> timer;
        auto it_timer = timer.find(id);

        if (it_alpha == alpha.end() && !const2)
        {
            alpha.insert({ id, 0 });
            it_alpha = alpha.find(id);
        }

        if (it_timer == timer.end() && !const2)
        {
            timer.insert({ id, 0 });
            it_timer = timer.find(id);
        }

        if (it_alpha->second < 255 && !const2)
            it_alpha->second += 3;

        if (it_alpha->second >= 255)
            const2_ready = true;

        if (const2_ready) {
            if (it_timer->second < 255)
                it_timer->second += 3;
        }

        if (it_timer->second > 245) {
            const2_ready = false; const2 = true;
        }

        if (const2)
        {
            if (it_alpha->second >= 0)
                it_alpha->second -= 5;

            anim_y = ImLerp(anim_y, IM_PI * 45.f, ImGui::GetIO().DeltaTime * 2.7f);
            gizzard_y = ImLerp(gizzard_y, IM_PI * -15.f, ImGui::GetIO().DeltaTime * 2.7f);

            if (it_alpha->second <= 1)
                isIntro = false;
        }

        if (!const2) {
            anim_y = ImLerp(anim_y, IM_PI * 85.f, ImGui::GetIO().DeltaTime * 2.3f);

        }

        ImVec2 textSize = ImGui::CalcTextSize("GILA MONSTER");
        ImVec2 textPos(io.DisplaySize.x / 2.22f - textSize.x, io.DisplaySize.y / 2 - textSize.y / 2 + 45 - anim_y);
        draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImColor(0, 0, 0, it_alpha->second), 0);
        draw->AddText(menu.introFont, 64.f, ImVec2(textPos.x, textPos.y), ImColor(it_alpha->second, 113, 113, it_alpha->second), "GILA MONSTER"); //Name

        gizzard_y = ImLerp(gizzard_y, IM_PI * 35.f, ImGui::GetIO().DeltaTime * 2.3f);

        ImVec2 size_image(740.0f, 430.f);
        ImVec2 center_pos = ImVec2(pos.x + io.DisplaySize.x / 2.25f - size_image.x + 463, io.DisplaySize.y / 2 - size_image.y + 245 - 35 + gizzard_y);

        ImGui::SetCursorPos(center_pos);
        ImRotateStart();
        ImGui::Image(Gizzard, size_image, ImVec2(0, 0), ImVec2(1, 1), ImColor(255,255,255, it_alpha->second));
        ImRotateEnd(0.001f * ::GetTickCount());
    }
    ImGui::End();
}

void Widgets()
{
    if (watermark)
    {
        ImGui::SetNextWindowSize(ImVec2(230, 30));
        ImGui::Begin("watermark by mon4ik", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
        {
            auto backgroud_list = ImGui::GetBackgroundDrawList();
            auto draw_list = ImGui::GetWindowDrawList();
            ImVec2 xy = ImGui::GetWindowPos();

            backgroud_list->AddShadowRect(xy, ImVec2(xy.x + 230, xy.y + 30), ImColor(26, 28, 96), 100.f, ImVec2(0, 0)); //Glow effect
            draw_list->AddText(menu.nameTable, 15.f, ImVec2(xy.x + 10, xy.y + 8), ImColor(133, 133, 133, 255), "GILA MONSTER | 134 ms | 138 fps"); //Name
        }
        ImGui::End();
    }

    if (spectatorlist)
    {
        ImGui::SetNextWindowSize(ImVec2(140, 30));
        ImGui::Begin("spectator list by mon4ik", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
        {
            auto backgroud_list = ImGui::GetBackgroundDrawList();
            auto draw_list = ImGui::GetWindowDrawList();
            ImVec2 xy = ImGui::GetWindowPos();

            backgroud_list->AddShadowRect(xy, ImVec2(xy.x + 140, xy.y + 30), ImColor(26, 28, 96), 100.f, ImVec2(0, 0)); //Glow effect
            draw_list->AddText(menu.nameTable, 15.f, ImVec2(xy.x + 28, xy.y + 8), ImColor(133, 133, 133, 255), "Spectator list"); //Name
        }
        ImGui::End();
    }

    if (keybinds)
    {
        ImGui::SetNextWindowSize(ImVec2(140, 30));
        ImGui::Begin("keybinds by mon4ik", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
        {
            auto backgroud_list = ImGui::GetBackgroundDrawList();
            auto draw_list = ImGui::GetWindowDrawList();
            ImVec2 xy = ImGui::GetWindowPos();

            backgroud_list->AddShadowRect(xy, ImVec2(xy.x + 140, xy.y + 30), ImColor(26, 28, 96), 100.f, ImVec2(0, 0)); //Glow effect
            draw_list->AddText(menu.nameTable, 15.f, ImVec2(xy.x + 40, xy.y + 8), ImColor(133, 133, 133, 255), "Keybinds"); //Name
        }
        ImGui::End();
    }
}

void Tab(const char* icon, const char* label, int _id) {
    ImVec2 pos = ImGui::GetCursorPos();
    ImGuiWindow* window = ImGui::GetCurrentWindow(); 
    const ImGuiID id = window->GetID(label);

    static std::map<ImGuiID, int> color_alpha;
    auto it_alpha = color_alpha.find(id);

    static std::map<ImGuiID, int> color_alpha2;
    auto it_alpha2 = color_alpha2.find(id);

    if (it_alpha == color_alpha.end())
    {
        color_alpha.insert({ id, 0 });
        it_alpha = color_alpha.find(id);
    }

    if (it_alpha2 == color_alpha2.end())
    {
        color_alpha2.insert({ id, 0 });
        it_alpha2 = color_alpha2.find(id);
    }

    ImGui::SetCursorPos(pos);
    if (ImGui::InvisibleButton(label, ImVec2(pos.x + 127, ImGui::CalcTextSize(icon).y + ImGui::CalcTextSize(label).y)))
        active_tab = _id;
    if (active_tab == _id)
    {
        if (it_alpha->second < 163)
            it_alpha->second += 5;

        if (it_alpha2->second < 255)
            it_alpha2->second += 5;

        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window->Pos.x + pos.x, window->Pos.y + pos.y), ImVec2(window->Pos.x + pos.x + 3, window->Pos.y + pos.y + 28), ImColor(139, 130, 159, it_alpha2->second), 5, ImDrawFlags_RoundCornersRight);
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window->Pos.x + pos.x + 7, window->Pos.y + pos.y), ImVec2(window->Pos.x + pos.x + 135, window->Pos.y + pos.y + 28), ImColor(20, 23, 22, it_alpha->second), 5);
        ImGui::SetCursorPos(pos);
        ImGui::PushFont(menu.Icons);
        ImGui::SetCursorPos(ImVec2(pos.x + 10, pos.y + ImGui::CalcTextSize(icon).y / 4 - 1));
        ImGui::TextColored(ImColor(255, 255, 255), icon);
        ImGui::SetCursorPos(ImVec2(pos.x + 35, pos.y + ImGui::CalcTextSize(icon).y / 4 + 1));
        ImGui::PopFont();
        ImGui::PushFont(menu.leftTable);
        ImGui::TextColored(ImColor(255, 255, 255), label);
        ImGui::PopFont();
    }
    else if (ImGui::IsItemHovered())
    {
        if (it_alpha->second < 133)
            it_alpha->second += 5;

        if (it_alpha2->second < 111)
            it_alpha2->second += 5;

        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window->Pos.x + pos.x, window->Pos.y + pos.y), ImVec2(window->Pos.x + pos.x + 3, window->Pos.y + pos.y + 28), ImColor(139, 130, 159, it_alpha2->second), 5, ImDrawFlags_RoundCornersRight);
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window->Pos.x + pos.x + 7, window->Pos.y + pos.y), ImVec2(window->Pos.x + pos.x + 135, window->Pos.y + pos.y + 28), ImColor(20, 23, 22, it_alpha->second), 5);
        ImGui::SetCursorPos(pos);
        ImGui::PushFont(menu.Icons);
        ImGui::SetCursorPos(ImVec2(pos.x + 10, pos.y + ImGui::CalcTextSize(icon).y / 4 - 1));
        ImGui::TextColored(ImColor(255, 255, 255), icon);
        ImGui::SetCursorPos(ImVec2(pos.x + 35, pos.y + ImGui::CalcTextSize(icon).y / 4 + 1));
        ImGui::PopFont();

        ImGui::PushFont(menu.leftTable);
        ImGui::TextColored(ImColor(255, 255, 255), label);
        ImGui::PopFont();
    }
    else
    {
        if (it_alpha->second > 0)
            it_alpha->second -= 5;

        if (it_alpha2->second > 0)
            it_alpha2->second -= 5;

        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window->Pos.x + pos.x, window->Pos.y + pos.y), ImVec2(window->Pos.x + pos.x + 3, window->Pos.y + pos.y + 28), ImColor(139, 130, 159, it_alpha2->second), 5, ImDrawFlags_RoundCornersRight);
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window->Pos.x + pos.x + 7, window->Pos.y + pos.y), ImVec2(window->Pos.x + pos.x + 135, window->Pos.y + pos.y + 28), ImColor(20, 23, 22, it_alpha->second), 5);

        ImGui::SetCursorPos(pos);
        ImGui::PushFont(menu.Icons);
        ImGui::SetCursorPos(ImVec2(pos.x + 10, pos.y + ImGui::CalcTextSize(icon).y / 4 - 1));
        ImGui::TextColored(ImColor(132, 136, 139), icon);
        ImGui::SetCursorPos(ImVec2(pos.x + 35, pos.y + ImGui::CalcTextSize(icon).y / 4 + 1));
        ImGui::PopFont();

        ImGui::PushFont(menu.leftTable);
        ImGui::TextColored(ImColor(132, 136, 139), label);
        ImGui::PopFont();
    }
}

void RenderTabs() {
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    window->DrawList->AddLine(ImVec2(window->Pos.x, window->Pos.y + 15), ImVec2(window->Pos.x + 8, window->Pos.y + 15), ImColor(26, 29, 28, 255), 1.5f);
    window->DrawList->AddText(menu.smallFont, 12.f, ImVec2(window->Pos.x + 13, window->Pos.y + 9), ImColor(133, 133, 133, 255), "RAGEBOT"); //Name
    window->DrawList->AddLine(ImVec2(window->Pos.x + 65, window->Pos.y + 15), ImVec2(window->Pos.x + 136, window->Pos.y + 15), ImColor(26, 29, 28, 255), 1.5f);

    ImGui::SetCursorPos(ImVec2(0, 30));
    ImGui::BeginGroup(); {
        Tab("1", "Rage", 1);
        ImGui::Spacing(); ImGui::Spacing();
        Tab("3", u8"Anti-Aim", 3);
    }
    ImGui::EndGroup();

    window->DrawList->AddLine(ImVec2(window->Pos.x, window->Pos.y + 105), ImVec2(window->Pos.x + 8, window->Pos.y + 105), ImColor(26, 29, 28, 255), 1.5f);
    window->DrawList->AddText(menu.smallFont, 12.f, ImVec2(window->Pos.x + 13, window->Pos.y + 99), ImColor(133, 133, 133, 255), "VISUALS"); //Name
    window->DrawList->AddLine(ImVec2(window->Pos.x + 62, window->Pos.y + 105), ImVec2(window->Pos.x + 136, window->Pos.y + 105), ImColor(26, 29, 28, 255), 1.5f);

    ImGui::SetCursorPos(ImVec2(0, 110));
    ImGui::BeginGroup(); {
        ImGui::Spacing(); ImGui::Spacing();
        Tab("2", "Visuals", 2);
        ImGui::Spacing(); ImGui::Spacing();
        Tab("4", "Players", 4);
    }
    ImGui::EndGroup();

    window->DrawList->AddLine(ImVec2(window->Pos.x, window->Pos.y + 195), ImVec2(window->Pos.x + 8, window->Pos.y + 195), ImColor(26, 29, 28, 255), 1.5f);
    window->DrawList->AddText(menu.smallFont, 12.f, ImVec2(window->Pos.x + 13, window->Pos.y + 189), ImColor(133, 133, 133, 255), "MISC"); //Name
    window->DrawList->AddLine(ImVec2(window->Pos.x + 45, window->Pos.y + 195), ImVec2(window->Pos.x + 136, window->Pos.y + 195), ImColor(26, 29, 28, 255), 1.5f);

    ImGui::SetCursorPos(ImVec2(0, 210));
    ImGui::BeginGroup(); {
        Tab("5", "Misc", 5);
        ImGui::Spacing(); ImGui::Spacing();
        Tab("6", "Configs", 6);
    }
    ImGui::EndGroup();
}

// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"gila monster", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWMAXIMIZED); //SW_SHOWDEFAULT
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();

    menu.nameCheat = io.Fonts->AddFontFromMemoryTTF(&lexend_bold, sizeof lexend_bold, 18.f);
    menu.nameCheat2 = io.Fonts->AddFontFromMemoryTTF(&lexend_bold, sizeof lexend_bold, 19.f);
    menu.nameTable = io.Fonts->AddFontFromMemoryTTF(&lexend_bold, sizeof lexend_bold, 15.f);
    menu.generalTable = io.Fonts->AddFontFromMemoryTTF(&lexend_bold, sizeof lexend_bold, 14.f);
    menu.leftTable = io.Fonts->AddFontFromMemoryTTF(&lexend_bold, sizeof lexend_bold, 16.f);
    menu.smallFont = io.Fonts->AddFontFromMemoryTTF(&lexend_bold, sizeof lexend_bold, 12.f);
    menu.introFont = io.Fonts->AddFontFromMemoryTTF(&lexend_bold, sizeof lexend_bold, 64.f);
    menu.Icons = io.Fonts->AddFontFromMemoryTTF(&IconFont, sizeof IconFont, 19, nullptr, io.Fonts->GetGlyphRangesCyrillic());

    D3DX11_IMAGE_LOAD_INFO iInfo;
    ID3DX11ThreadPump* threadPump{ nullptr };

    D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, LogoGizzard, sizeof(LogoGizzard), &iInfo, threadPump, &Gizzard, 0);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

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
                done = true;
        }
        if (done)
            break;

        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (isIntro)
            Intro("Intro");
        else {
            EventLogs();
            ImGui::SetNextWindowSize(ImVec2(620, 471));
            ImGui::Begin("menu by mon4ik", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
            {
                auto backgroud_list = ImGui::GetBackgroundDrawList();
                auto draw_list = ImGui::GetWindowDrawList();
                ImVec2 xy = ImGui::GetWindowPos();

                backgroud_list->AddShadowRect(xy, ImVec2(xy.x + 620, xy.y + 471), ImColor(26, 28, 96), 100.f, ImVec2(0, 0)); //Glow effect
                backgroud_list->AddRectFilled(xy, ImVec2(xy.x + 620, xy.y + 471), ImColor(17, 19, 19), 5, ImDrawFlags_RoundCornersAll); //BG color
                draw_list->AddRectFilled(xy, ImVec2(xy.x + 140, xy.y + 471), ImColor(12, 15, 14), 5, ImDrawFlags_RoundCornersLeft); //Left panel
                draw_list->AddLine(ImVec2(xy.x + 140, xy.y), ImVec2(xy.x + 140, xy.y + 472), ImColor(26, 29, 28), 1.5f); //Left panel rect
                draw_list->AddLine(ImVec2(xy.x + 140, xy.y + 38), ImVec2(xy.x + 620, xy.y + 38), ImColor(26, 29, 28), 1.5f); //Up panel rect
                RenderTabs();

                draw_list->AddText(menu.nameCheat, 18, ImVec2(xy.x + 156, xy.y + 11), ImColor(255, 255, 255, 255), "GILA"); //Name cheat
                draw_list->AddText(menu.nameCheat2, 19, ImVec2(xy.x + 199, xy.y + 10), ImColor(102, 101, 134, 255), "MONSTER"); //Name cheat

                ImGui::SetCursorPos(ImVec2(148, 50));
                ImGui::BeginChild("GENERAL", ImVec2(229, 402));
                {
                    ImGui::PushFont(menu.generalTable);
                    ImGui::SetCursorPosY(45.f);
                    ImGui::Checkbox("Watermark", &watermark);
                    ImGui::Checkbox("Keybinds", &keybinds);
                    ImGui::Checkbox("Spectator list", &spectatorlist);
                    if (ImGui::Button("Log event", ImVec2(219, 25)))
                        event_logs++;
                    ImGui::PopFont();
                }
                ImGui::EndChild();

                ImGui::SetCursorPos(ImVec2(384, 50));
                ImGui::BeginChild("WEAPONS", ImVec2(229, 402));
                {
                    ImGui::PushFont(menu.generalTable);
                    ImGui::SetCursorPosY(45.f);
                    ImGui::SliderInt("Test slider", &alphaWidgets, 215, 255);
                    ImGui::Combo("Test combo", &combo_test, combo_ctr, 2); ImGui::Spacing();
                    ImGui::SliderInt("Test slider", &alphaWidgets, 215, 255);
                    ImGui::PopFont();
                }
                ImGui::EndChild();
            }
            ImGui::End();

            Widgets();
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
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
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
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
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
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
