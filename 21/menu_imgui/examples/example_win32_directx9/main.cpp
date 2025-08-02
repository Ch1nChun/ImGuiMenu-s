#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"
#include "imgui_internal.h"
#include <d3d11.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <chrono>
#include <windows.h>
#include <Lmcons.h>
#include "utils.h"


// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ImFont* font;
float window_alpha = 0.0f;
bool menu_open = false;
const int toggle_key = VK_INSERT; // Key to toggle the menu

// Variables for checkbox, slider, and color picker
bool aimbot_checkbox_value = false;
bool prediction_checkbox_value = false;
bool shake_checkbox_value = false;
bool stickyaim_checkbox_value = false;
bool disableoutsidefov_checkbox_value = false;
bool teamcheck_checkbox_value = false;
bool knockcheck_checkbox_value = false;
bool celex_checkbox_value = false;
float slider_value = 1.0f;
float aimbotslider_value = 0.5f;
// ImVec4 main_color = ImVec4(210.0f / 255.0f, 170.0f / 255.0f, 200.0f / 255.0f, 1.0f); // Default menu color 210, 170, 200
int aimbot_key = 0; // Variable to store the aimbot keybind
int selected_item = 0; // Variable to store the selected item in the ListBox
std::vector<std::string> listbox_items = { "Item 1", "Item 2", "Item 3", "Item 4", "Item 5" }; // ListBox items

template<class T, class U>
static T clamp(const T& in, const U& low, const U& high)
{
    if (in <= low)
        return low;

    if (in >= high)
        return high;

    return in;
}

// Structure to handle tab animations
struct tab_anim {
    float hovered_text_anim;
    float active_text_anim;
};

// Function to get the current user's name
std::string GetUserNameString() {
    wchar_t username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserNameW(username, &username_len)) {
        char username_c[UNLEN + 1];
        wcstombs(username_c, username, UNLEN + 1); // Convert wide string to narrow string
        return std::string(username_c);
    }
    else {
        return "User";
    }
}

// Function to toggle the aimbot state
void ToggleAimbot() {
    aimbot_checkbox_value = !aimbot_checkbox_value;
}

// Main code
#define IM_USE using namespace ImGui;
static int tab = 0;
static int tab2 = 0;
#include <math.h>
#include "Header.h"
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

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

int main(int, char**)
{
    // Get the current user's name
    std::string user_name = GetUserNameString();

    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_POPUP, 0, 0, 1920, 1080, NULL, NULL, wc.hInstance, NULL);

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

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    ImFontConfig font_config;
    font_config.OversampleH = 1;
    font_config.OversampleV = 1;
    font_config.PixelSnapH = 1;

    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF,
        0x0400, 0x044F,
        0,
    };

    ImFont* icons = io.Fonts->AddFontFromMemoryTTF((void*)icon, sizeof(icon), 44, &font_config, ranges);

    // Load Verdana font from memory using the byte array from font_data.h
    font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 13.0f, &font_config, io.Fonts->GetGlyphRangesJapanese());

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.051f, 0.051f, 0.051f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
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

        if (GetAsyncKeyState(toggle_key) & 1)
        {
            menu_open = !menu_open;
            window_alpha = menu_open ? 1.0f : 0.0f;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        static float open_alpha = 0.0;

        if (window_alpha > 0.0f)
        {
            auto s = ImVec2{}, p = ImVec2{}, gs = ImVec2{ 600, 420 };
            ImGui::SetNextWindowSize(ImVec2(gs));
            ImGui::SetNextWindowBgAlpha(window_alpha);
            ImGui::Begin("##GUI", NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground);

            {
                {// draw

                    s = ImVec2(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2);
                    p = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y);
                    auto draw = ImGui::GetWindowDrawList();

                    draw->AddRectFilled(ImVec2(p.x, p.y + 0), ImVec2(p.x + s.x, p.y + s.y - 0), ImColor(8, 8, 8), 4); // bg

                    draw->AddRect(ImVec2(p.x + 1, p.y + 1), ImVec2(p.x + s.x - 1, p.y + s.y - 1), ImColor(27, 27, 27, 255), 4.5); // outline
                    draw->AddRect(ImVec2(p.x - -10, p.y + 35.3), ImVec2(p.x + s.x - 480, p.y + s.y - 36.85), ImColor(26, 26, 26, 255)); // subtabs outline

                    draw->AddLine(ImVec2(p.x, p.y + s.y - 27), ImVec2(p.x + s.x, p.y + s.y - 27), ImColor(27, 27, 27, 255)); // top separator
                    draw->AddLine(ImVec2(p.x, p.y + 25), ImVec2(p.x + s.x, p.y + 25), ImColor(27, 27, 27, 255)); // tab separator

                    int fade_line_count = 60;
                    float fade_stop = s.x;
                    float center_point = fade_stop / 2.0f;

                    for (int i = 0; i < fade_line_count; i++)
                    {
                        float alpha = 1.0f - (i * (1.0f / fade_line_count));

                        ImVec2 start_right = ImVec2(p.x + fade_stop - i * (center_point / fade_line_count), p.y + 25);
                        ImVec2 end_right = ImVec2(p.x + fade_stop - (i + 1) * (center_point / fade_line_count), p.y + 25);

                        ImColor fade_color(main_color.x, main_color.y, main_color.z, alpha);

                        draw->AddLine(start_right, end_right, fade_color);
                    }

                    ImGui::AlignTextToFramePadding();
                    ImGui::PushFont(font);
                    draw->AddText(ImVec2(p.x + 9.5, p.y + 7), ImColor(main_color), "name");

                    draw->AddText(ImVec2(p.x + 9.5, p.y + 384), ImColor(255, 255, 255, 100), "Build:");
                    draw->AddText(ImVec2(p.x + 41, p.y + 384), ImColor(main_color), ("Live"));

                    ImVec2 welcome_text_size = ImGui::CalcTextSize(("Welcome back, " + user_name).c_str());
                    draw->AddText(ImVec2(p.x + s.x - welcome_text_size.x - -77.5, p.y + 384), ImColor(main_color), ("sexy"));

                    ImGui::PopFont();
                }

                {//tabs
                    ImGui::PushFont(font);
                    ImGui::SetCursorPosX(112);
                    ImGui::SetCursorPosY(10);
                    ImGui::BeginGroup();
                    if (ImGui::tab("Aimbot", tab == 0)) tab = 0; ImGui::SameLine();
                    if (ImGui::tab("Visuals", tab == 1)) tab = 1; ImGui::SameLine();
                    if (ImGui::tab("Misc", tab == 2)) tab = 2; ImGui::SameLine();
                    ImGui::EndGroup();
                    ImGui::PopFont();
                }
                {//content
                    if (tab == 0)
                    {
                        ImGui::PushFont(font);
                        {//subtabs

                            ImGui::SetCursorPosY(54);
                            ImGui::SetCursorPosX(30);
                            if (ImGui::subtab("General", tab2 == 0)) tab2 = 0;

                            ImGui::SetCursorPosY(79);
                            ImGui::SetCursorPosX(30);
                            if (ImGui::subtab("Advanced", tab2 == 1)) tab2 = 1;



                            if (tab2 == 0) {// First Subtab

                                ImGui::SetCursorPosY(38);
                                ImGui::SetCursorPosX(122);
                                ImGui::MenuChild("Main Group", ImVec2(226, 337), false);
                                {

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Enabled", &aimbot_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Prediction", &prediction_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Shake", &shake_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Sticky Aim", &stickyaim_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Disable Outside FOV", &disableoutsidefov_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Teamcheck", &teamcheck_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Knockcheck", &knockcheck_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                //        ImGui::Text("");
                                 //       ImGui::SameLine();
                                 //       if (ImGui::Button("button")) {}

                                    ImGui::EndChild();
                                }

                                ImGui::SetCursorPosY(38);
                                ImGui::SetCursorPosX(358);
                                ImGui::MenuChild("Modifiers", ImVec2(224, 337), false);
                                {

                                    const char* items[] = { "Mouse", "Camera" };
                                    static int item_current = 0;
                                    ImGui::Combo("combo1", &item_current, items, IM_ARRAYSIZE(items)); // Unique ID: "combo1"
                                    ImGui::SameLine();

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("Smoothing", &slider_value, 0.0f, 2.0f);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("FOV", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("X Offset", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("Y Offset", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    const char* items1[] = { "(Default) Division", "Multiplication" };
                                    static int item_current1 = 0;
                                    ImGui::Combo("combo2", &item_current1, items1, IM_ARRAYSIZE(items1)); // Unique ID: "combo2"
                                    ImGui::SameLine();

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("Prediction X", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("Prediction Y", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    const char* items3[] = { "Head", "UpperTorso", "HumanoidRootPart", "LowerTorso" };
                                    static int item_current3 = 0;
                                    ImGui::Combo("combo3", &item_current3, items3, IM_ARRAYSIZE(items3)); // Unique ID: "combo3"
                                    ImGui::SameLine();

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("X Shake", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("Y Shake", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    // Uncomment and adjust if needed:
                                    // ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
                                    // KeybindSelector("Aimbot Keybind", &aimbot_key);

                                }
                                ImGui::EndChild();
                            }


                            else if (tab2 == 1) {// Second Subtab

                                ImGui::SetCursorPosY(38);
                                ImGui::SetCursorPosX(122);
                                ImGui::MenuChild("Main Group", ImVec2(226, 337), false);
                                {
                                }
                                ImGui::EndChild();

                                ImGui::SetCursorPosY(38);
                                ImGui::SetCursorPosX(358);
                                ImGui::MenuChild("Modifiers", ImVec2(224, 337), false);
                                {

                                    //   ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
                                    //   KeybindSelector("Aimbot Keybind", &aimbot_key);

                                }
                                ImGui::EndChild();
                            }
                        }
                        ImGui::PopFont();
                    }

                    if (tab == 1)
                    {
                        ImGui::PushFont(font);
                        {//subtabs

                            ImGui::SetCursorPosY(54);
                            ImGui::SetCursorPosX(30);
                            if (ImGui::subtab("General", tab2 == 0)) tab2 = 0;

                            ImGui::SetCursorPosY(79);
                            ImGui::SetCursorPosX(30);
                            if (ImGui::subtab("Advanced", tab2 == 1)) tab2 = 1;



                            if (tab2 == 0) {// First Subtab

                                ImGui::SetCursorPosY(38);
                                ImGui::SetCursorPosX(122);
                                ImGui::MenuChild("Main Group", ImVec2(226, 337), false);
                                {

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Enabled", &aimbot_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Prediction", &prediction_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Shake", &shake_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Sticky Aim", &stickyaim_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Disable Outside FOV", &disableoutsidefov_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Teamcheck", &teamcheck_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_CheckMark, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                                    ImGui::Checkbox("Knockcheck", &knockcheck_checkbox_value);
                                    ImGui::PopStyleColor(2);

                                    //        ImGui::Text("");
                                     //       ImGui::SameLine();
                                     //       if (ImGui::Button("button")) {}

                                    ImGui::EndChild();
                                }

                                ImGui::SetCursorPosY(38);
                                ImGui::SetCursorPosX(358);
                                ImGui::MenuChild("Modifiers", ImVec2(224, 337), false);
                                {

                                    const char* items[] = { "Mouse", "Camera" };
                                    static int item_current = 0;
                                    ImGui::Combo("combo1", &item_current, items, IM_ARRAYSIZE(items)); // Unique ID: "combo1"
                                    ImGui::SameLine();

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("Smoothing", &slider_value, 0.0f, 2.0f);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("FOV", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("X Offset", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("Y Offset", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    const char* items1[] = { "(Default) Division", "Multiplication" };
                                    static int item_current1 = 0;
                                    ImGui::Combo("combo2", &item_current1, items1, IM_ARRAYSIZE(items1)); // Unique ID: "combo2"
                                    ImGui::SameLine();

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("Prediction X", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("Prediction Y", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    const char* items3[] = { "Head", "UpperTorso", "HumanoidRootPart", "LowerTorso" };
                                    static int item_current3 = 0;
                                    ImGui::Combo("combo3", &item_current3, items3, IM_ARRAYSIZE(items3)); // Unique ID: "combo3"
                                    ImGui::SameLine();

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("X Shake", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, main_color);
                                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, main_color);
                                    ImGui::SliderFloat("Y Shake", &aimbotslider_value, 0.0f, 1.0f);
                                    ImGui::PopStyleColor(2);

                                    // Uncomment and adjust if needed:
                                    // ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
                                    // KeybindSelector("Aimbot Keybind", &aimbot_key);

                                }
                                ImGui::EndChild();
                            }


                            else if (tab2 == 1) {// Second Subtab

                                ImGui::SetCursorPosY(38);
                                ImGui::SetCursorPosX(122);
                                ImGui::MenuChild("Main Group", ImVec2(226, 337), false);
                                {
                                }
                                ImGui::EndChild();

                                ImGui::SetCursorPosY(38);
                                ImGui::SetCursorPosX(358);
                                ImGui::MenuChild("Modifiers", ImVec2(224, 337), false);
                                {

                                    //   ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
                                    //   KeybindSelector("Aimbot Keybind", &aimbot_key);

                                }
                                ImGui::EndChild();
                            }
                        }
                        ImGui::PopFont();
                    }

                    else if (tab == 2)
                    {
                        ImGui::PushFont(font);
                        {
                            ImGui::SetCursorPosY(38);
                            ImGui::SetCursorPosX(122);
                            ImGui::MenuChild("Main Group", ImVec2(226, 337), false);
                            {
                            }
                            ImGui::EndChild();

                            ImGui::SetCursorPosY(38);
                            ImGui::SetCursorPosX(358);
                            ImGui::MenuChild("Modifiers", ImVec2(226, 337), false);
                            {
                            }
                            ImGui::EndChild();
                        }
                        ImGui::PopFont();
                    }
                }
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
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
    // Create the D3D device and device context
    D3D_FEATURE_LEVEL feature_level;
    const D3D_FEATURE_LEVEL feature_levels[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT result = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, feature_levels, 2, D3D11_SDK_VERSION, &g_pd3dDevice, &feature_level, &g_pd3dDeviceContext);
    if (FAILED(result))
    {
        MessageBox(NULL, _T("Failed to create D3D11 device!"), _T("Error"), MB_OK | MB_ICONERROR);
        return false;
    }

    // Create the swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
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

    IDXGIFactory* dxgi_factory = NULL;
    IDXGIDevice* dxgi_device = NULL;
    IDXGIAdapter* dxgi_adapter = NULL;

    result = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device);
    if (FAILED(result))
    {
        MessageBox(NULL, _T("Failed to get DXGIDevice!"), _T("Error"), MB_OK | MB_ICONERROR);
        return false;
    }

    result = dxgi_device->GetAdapter(&dxgi_adapter);
    if (FAILED(result))
    {
        MessageBox(NULL, _T("Failed to get DXGIAdapter!"), _T("Error"), MB_OK | MB_ICONERROR);
        dxgi_device->Release();
        return false;
    }

    result = dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgi_factory);
    if (FAILED(result))
    {
        MessageBox(NULL, _T("Failed to get DXGIFactory!"), _T("Error"), MB_OK | MB_ICONERROR);
        dxgi_adapter->Release();
        dxgi_device->Release();
        return false;
    }

    result = dxgi_factory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
    if (FAILED(result))
    {
        MessageBox(NULL, _T("Failed to create swap chain!"), _T("Error"), MB_OK | MB_ICONERROR);
        dxgi_factory->Release();
        dxgi_adapter->Release();
        dxgi_device->Release();
        return false;
    }

    dxgi_factory->Release();
    dxgi_adapter->Release();
    dxgi_device->Release();

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* p_back_buffer;
    HRESULT result = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&p_back_buffer);
    if (FAILED(result))
    {
        MessageBox(NULL, _T("Failed to get back buffer!"), _T("Error"), MB_OK | MB_ICONERROR);
        return;
    }
    result = g_pd3dDevice->CreateRenderTargetView(p_back_buffer, NULL, &g_mainRenderTargetView);
    p_back_buffer->Release();
    if (FAILED(result))
    {
        MessageBox(NULL, _T("Failed to create render target view!"), _T("Error"), MB_OK | MB_ICONERROR);
    }
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
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
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
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
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
