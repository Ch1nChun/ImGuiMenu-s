#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "imgui_edited.hpp"

#include "imgui_freetype.h"
#include <d3d11.h>
#include <tchar.h>

#include "font.h"
#include "texture.h"

#include <vector>
#include <string>


bool info_bar = true;

const char* cheat_name = "RAGNAREK";
const char* game_status = "Counter-Strike: 2";
const char* developer = "Past Owl";
const char* role = "Sr.DEV";
const char* fps = "210fps";
const char* ping = "45ms";
const char* world_time = "12:20am";


#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")

static int notify_select = 0;
const char* notify_items[2]{ "Circle", "Line" };

namespace texture
{
    ID3D11ShaderResourceView* esp_preview = nullptr;
}

namespace esp_preview
{
    bool money = true;
    bool nickname = true;
    bool weapon = true;
    bool zoom = true;

    bool c4 = true;
    bool HP_line = true;
    bool hit = true;
    bool box = true;
    bool bomb = true;

    static float box_color[4] = { 37 / 255.f, 37 / 255.f, 47 / 255.f, 1.f };
    static float nick_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float money_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float zoom_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float c4_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float bomb_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float hp_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float hp_line_color[4] = { 112 / 255.f, 109 / 255.f, 214 / 255.f, 1.f };
    static float weapon_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };
    static float hit_color[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };

    int hp = 85;
}

int page = 0;

static float tab_alpha = 0.f; /* */ static float tab_add; /* */ static int active_tab = 0;
bool enable_preview = true;

static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace ImGui;

DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

#include <imgui.h>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

void CricleProgress(const char* name, float progress, float max, float radius, const ImVec2& size)
{
    const float tickness = 3.f;
    static float position = 0.f;

    position = progress / max * 6.28f;

    ImGui::GetForegroundDrawList()->PathClear();
    ImGui::GetForegroundDrawList()->PathArcTo(GetCursorScreenPos() + size, radius, 0.f, 2.f * IM_PI, 120.f);
    ImGui::GetForegroundDrawList()->PathStroke(ImGui::GetColorU32(c::elements::background_widget), 0, tickness);

    ImGui::GetForegroundDrawList()->PathClear();
    ImGui::GetForegroundDrawList()->PathArcTo(GetCursorScreenPos() + size, radius, IM_PI * 1.5f, IM_PI * 1.5f + position, 120.f);
    ImGui::GetForegroundDrawList()->PathStroke(ImGui::GetColorU32(c::accent), 0, tickness);

    int procent = progress / (int)max * 100;

    std::string procent_str = std::to_string(procent) + "%";

}

struct Notification {
    int id;
    std::string message;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
};

class NotificationSystem {
public:
    NotificationSystem() : notificationIdCounter(0) {}

    void AddNotification(const std::string& message, int durationMs) {
        auto now = std::chrono::steady_clock::now();
        auto endTime = now + std::chrono::milliseconds(durationMs);
        notifications.push_back({ notificationIdCounter++, message, now, endTime });
    }

    void DrawNotifications() {
        auto now = std::chrono::steady_clock::now();

        std::sort(notifications.begin(), notifications.end(),
            [now](const Notification& a, const Notification& b) -> bool {
                float durationA = std::chrono::duration_cast<std::chrono::milliseconds>(a.endTime - a.startTime).count();
                float elapsedA = std::chrono::duration_cast<std::chrono::milliseconds>(now - a.startTime).count();
                float percentageA = (durationA - elapsedA) / durationA;

                float durationB = std::chrono::duration_cast<std::chrono::milliseconds>(b.endTime - b.startTime).count();
                float elapsedB = std::chrono::duration_cast<std::chrono::milliseconds>(now - b.startTime).count();
                float percentageB = (durationB - elapsedB) / durationB;

                return percentageA < percentageB;
            }
        );

        int currentNotificationPosition = 0;

        for (auto& notification : notifications) {
            if (now < notification.endTime) {
                float duration = std::chrono::duration_cast<std::chrono::milliseconds>(notification.endTime - notification.startTime).count();
                float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - notification.startTime).count();
                float percentage = (duration - elapsed) / duration * 100.0f;

                ShowNotification(currentNotificationPosition, notification.message, percentage);
                currentNotificationPosition++;
            }
        }

        notifications.erase(std::remove_if(notifications.begin(), notifications.end(),
            [now](const Notification& notification) { return now >= notification.endTime; }),
            notifications.end());
    }

private:
    std::vector<Notification> notifications;
    int notificationIdCounter;

    void ShowNotification(int position, const std::string& message, float percentage) {

        float duePercentage = 100.0f - percentage;
        float alpha = percentage > 10.0f ? 1.0f : percentage / 10.0f;
        GetStyle().WindowPadding = ImVec2(15, 10);

        ImGui::SetNextWindowPos(ImVec2(duePercentage < 15.f ? duePercentage : 15.f, 15 + position * 90));

        ImGui::Begin(("##NOTIFY" + std::to_string(position)).c_str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImVec2 pos = ImGui::GetWindowPos(), spacing = ImGui::GetStyle().ItemSpacing, region = ImGui::GetContentRegionMax();

        GetBackgroundDrawList()->AddRectFilledMultiColor(pos, pos + region, ImGui::GetColorU32(c::background::filling, alpha), ImGui::GetColorU32(c::accent, 0.01f), ImGui::GetColorU32(c::accent, 0.01f), ImGui::GetColorU32(c::background::filling, alpha), c::elements::rounding);
        GetBackgroundDrawList()->AddRectFilled(pos, pos + region, ImGui::GetColorU32(c::background::filling, 0.4f), c::elements::rounding);
        GetBackgroundDrawList()->AddRect(pos, pos + region, ImGui::GetColorU32(c::background::stroke, alpha), c::elements::rounding);

        if (notify_select == 0)
        CricleProgress("##NOTIFY", percentage, 100, 7.f, ImVec2(GetContentRegionMax().x - 40, 11));
        if (notify_select == 1)
        GetBackgroundDrawList()->AddRectFilled(pos + ImVec2(0, region.y - 3), pos + ImVec2(region.x * (duePercentage / 100.0f), region.y), ImGui::GetColorU32(c::accent, alpha), c::elements::rounding);

        PushFont(font::lexend_bold);
        ImGui::TextColored(ImColor(GetColorU32(c::accent, alpha)), "%s", "[Notification]");
        ImGui::TextColored(ImColor(GetColorU32(c::elements::text_active, alpha)), "%s", message.c_str());
        ImGui::Dummy(ImVec2(CalcTextSize(message.c_str()).x + 15, 5));
        PopFont();

        ImGui::End();
    }
};

NotificationSystem notificationSystem;

int main(int, char**)
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_POPUP, 0, 0, 1920, 1080, nullptr, nullptr, wc.hInstance, nullptr);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; 

    ImFontConfig cfg;
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;

    font::lexend_general_bold = io.Fonts->AddFontFromMemoryTTF(lexend_bold, sizeof(lexend_bold), 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::lexend_bold = io.Fonts->AddFontFromMemoryTTF(lexend_regular, sizeof(lexend_regular), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::lexend_regular = io.Fonts->AddFontFromMemoryTTF(lexend_regular, sizeof(lexend_regular), 14.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::icomoon = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 20.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    font::icomoon_widget = io.Fonts->AddFontFromMemoryTTF(icomoon_widget, sizeof(icomoon_widget), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::icomoon_widget2 = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 16.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };
    if (texture::esp_preview == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, esp_preview1, sizeof(esp_preview1), &info, pump, &texture::esp_preview, 0);

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    bool done = false;
    while (!done)
    {

        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done) break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();

        NewFrame();
        {
            ImGuiStyle* style = &ImGui::GetStyle();

            notificationSystem.DrawNotifications();

            style->WindowPadding = ImVec2(0, 0);
            style->ItemSpacing = ImVec2(10, 10);
            style->WindowBorderSize = 0;
            style->ScrollbarSize = 8.f;

            static float color[4] = { 124 / 255.f, 103 / 255.f, 255 / 255.f, 1.f };
            c::accent = { color[0], color[1], color[2], 1.f };

            SetNextWindowSize(c::background::size);

            Begin("IMGUI!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus);
            {
                const ImVec2& pos = GetWindowPos();
                const ImVec2& region = GetContentRegionMax();
                const ImVec2& spacing = style->ItemSpacing;

                GetBackgroundDrawList()->AddRectFilled(pos, pos + c::background::size, GetColorU32(c::background::filling), c::background::rounding);
                GetBackgroundDrawList()->AddRectFilled(pos, pos + ImVec2(200.f, c::background::size.y), GetColorU32(c::tab::border), c::background::rounding, ImDrawFlags_RoundCornersLeft);
                GetBackgroundDrawList()->AddLine(pos + ImVec2(200.f, 0.f), pos + ImVec2(200.f, c::background::size.y), GetColorU32(c::background::stroke), 1.f);

                GetBackgroundDrawList()->AddRect(pos, pos + c::background::size, GetColorU32(c::background::stroke), c::background::rounding);

                SetCursorPos(ImVec2(10, 10));
                BeginGroup();

                std::vector<std::vector<std::string>> tab_columns = {
                    { "c", "b", "f", "o", "e" },
                    { "Aimbot", "Visuals", "Skins", "Configs", "Settings" },
                    { "Shooting is more accur...", "Changing the look and...", "Changing characters, w...", "Function customization...", "Changing the paramet..." },
                    { "Have you switched to the Aimbot tab? You're just crazy!", "Have you switched to the Visuals tab, do you want to get banned?", "You switched to the skins tab, why do you need self-deception??", "You switched over.. And yes, to hell with it, come up with a script yourself.", "You switched over.. And yes, to hell with it, come up with a script yourself." }
                };

                for (int i = 0; i < tab_columns[0].size(); ++i) {

                    if (edited::Tab(page == i, tab_columns[0][i].c_str(), tab_columns[1][i].c_str(), tab_columns[2][i].c_str(), ImVec2(180, 50))) {
                        notificationSystem.AddNotification(tab_columns[3][i], 1000);
                        page = i;
                    }

                }
                    
                EndGroup();

                tab_alpha = ImLerp(tab_alpha, (page == active_tab) ? 1.f : 0.f, 15.f * ImGui::GetIO().DeltaTime);
                if (tab_alpha < 0.01f && tab_add < 0.01f) active_tab = page;

                ImGui::SetCursorPos(ImVec2(200, 100 - (tab_alpha * 100)));

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * style->Alpha);
                {

                    if (active_tab == 0)
                    {
                        edited::BeginChild("##Container0", ImVec2((c::background::size.x - 200) / 2, c::background::size.y), NULL);
                        {

                            ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Weapons");

                            static int select = 0;
                            const char* items[3]{ "One", "Two", "Three" };
                            edited::Combo("Select Weapon", "Select a weapon to customize", &select, items, IM_ARRAYSIZE(items), 3);

                            ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "General");

                            static bool enabled = true;
                            edited::Checkbox("Enabled", "Activate ragebot [AWP]", &enabled);

                            if (IsItemClicked() && IsItemHovered()) notificationSystem.AddNotification("You clicked on the check box, the notification is here!", 4000);

                            static bool pick = true;
                            edited::Checkbox("Peek Assist", "Returns you to the previous point after peek", &pick);

                            if (IsItemClicked() && IsItemHovered()) notificationSystem.AddNotification("I'm going to be a hell of a long time, no offense.", 10500);

                            static bool shots = true;
                            edited::Checkbox("Hide Shots", "Doesn't turn your head forward after a shot is fired", &shots);

                            static bool dtap = true;
                            edited::Checkbox("Double Tap", "Fires 2 rounds per round on some weapons", &dtap);

                            static int field = 90;
                            edited::SliderInt("Field of View", "The range of the ragebot", &field, -180, 180, "%d\xC2\xB0");

                            ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Accuracy");

                            static bool scope = true;
                            edited::Checkbox("Auto Scope", "Automatically scope when a target is seen", &scope);

                            static bool stope = true;
                            edited::Checkbox("Auto Stope", "Automatically stops when a target is seen", &stope);

                        }
                        edited::EndChild();

                        SameLine(0, 0);

                        edited::BeginChild("##Container1", ImVec2((c::background::size.x - 200) / 2, c::background::size.y), NULL);
                        {
                            ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Selection");

                            static bool multi_num[5] = { false, true, true, false, true };
                            const char* multi_items[5] = { "Head", "Chest", "Stromatch", "Body", "Legs" };
                            edited::MultiCombo("Hitboxes", "What body parts will the ragebot shoot at?", multi_num, multi_items, 5);

                            static bool multi_num1[5] = { true, true, false, false, false };
                            const char* multi_items1[5] = { "Head", "Chest", "Stromatch", "Body", "Legs" };
                            edited::MultiCombo("Multipoints", "Point of impact calculation in hitboxes", multi_num1, multi_items1, 5);

                            static float hit1, hit2;
                            edited::RangeSliderFloat("Hit Chance", "The chance of hitting the enemy", &hit1, &hit2, 0, 100, "%.1f, %.1f");

                            static float dmg1, dmg2;
                            edited::RangeSliderFloat("Min. Damage", "Minimum damage to enemy hit", &dmg1, &dmg2, 0, 100, "%.0fHP, %.0fHP");

                            static bool penetrate = true;
                            edited::Checkbox("Penetrate Walls", "Shoot through walls", &penetrate);

                            ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Safety");

                            static int body_select = 0;
                            const char* body_items[3]{ "Default", "Prefer", "Force" };
                            edited::Combo("Body Aim", "Will shoot more often at the body", &body_select, body_items, IM_ARRAYSIZE(body_items), 3);

                            static int safe_select = 0;
                            const char* safe_items[3]{ "Default", "Prefer", "Force" };
                            edited::Combo("Safe Points", "Will only shoot at points the cheat is sure of.", &safe_select, safe_items, IM_ARRAYSIZE(safe_items), 3);

                            static int ensure_select = 0;
                            const char* ensure_items[3]{ "Default", "Prefer", "Force" };
                            edited::Combo("Ensure Hitbox Safety", "I don't know what the fuck that function is.", &ensure_select, ensure_items, IM_ARRAYSIZE(ensure_items), 3);

                            ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Keybinds");

                            static int key = 0;
                            if (edited::Keybind("Aimbot Active", "Activate all parts of these parameters.", &key))
                            {
                             notificationSystem.AddNotification("You have successfully bound a key!", 2500);
                            }

                            static int key1 = 0;
                            if (edited::Keybind("Penetrate Active", "Activates this function by holding or pressing a key.", &key1))
                            {
                                notificationSystem.AddNotification("You have successfully bound a key!", 2500);
                            }

                        }
                        edited::EndChild();
                    }
                    else if (active_tab == 1)
                    {
                        edited::BeginChild("##Container0", ImVec2((c::background::size.x - 200) / 2, c::background::size.y), NULL);
                        {
                            ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Select");

                            static int select = 0;
                            const char* items[1]{ "Enemy" };
                            edited::Combo("Type", "Select type", &select, items, IM_ARRAYSIZE(items), 1);

                            ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "General");

                            static bool enable = true;
                            edited::Checkbox("Enabled", "Active visuals", &enable);

                            static int select1 = 0;
                            const char* items1[2]{ "Health", "Armor" };
                            edited::Combo("Select Weapon", "Select a weapon to customize", &select1, items1, IM_ARRAYSIZE(items1), 2);

                            static float health_color[4] = { 0.f, 0.f, 1.f, 0.5f };
                            edited::ColorEdit4("Health Color", "The color of the 'Health' element", health_color, picker_flags);

                            static float armor_color[4] = { 0.f, 0.f, 1.f, 0.5f };
                            edited::ColorEdit4("Armor Color", "The color of the ‘Armor’ element", armor_color, picker_flags);

                            static int line_ticknes = 50;
                            edited::SliderInt("Line Thickness", "The range of the ragebot", &line_ticknes, 0, 100, "%d%%");

                            static int x_pos0 = (1920 / 2), x_pos1 = (1080 / 2);
                            edited::QuandInt("Quand Integer", "ESP Preview exact position.", &x_pos0, &x_pos1, 0, 1920, 0, 1080);

                        }
                        edited::EndChild();

                        SameLine(0, 0);

                        edited::BeginChild("##Container1", ImVec2((c::background::size.x - 200) / 2, c::background::size.y), NULL);
                        {

                            ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Chams");

                            static int select1 = 0;
                            const char* items1[2]{ "Visibility", "Invisibility" };
                            edited::Combo("Style", "Select a style to customize", &select1, items1, IM_ARRAYSIZE(items1), 2);

                            static bool enabled = true;
                            edited::Checkbox("Enabled", "Activate ‘Visibility’", &enabled);

                            static int select2 = 0;
                            const char* items2[2]{ "Flat", "Blyat" };
                            edited::Combo("Material", "Select material for ‘Visibility’", &select1, items2, IM_ARRAYSIZE(items2), 2);

                            static float color[4] = { 0.f, 0.f, 1.f, 0.5f };
                            edited::ColorEdit4("Color", "Color to material", color, picker_flags);

                            ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Glow");

                            static bool enabled_glow = true;
                            edited::Checkbox("Enabled ", "Activate glow", &enabled_glow);

                            static float color_glow[4] = { 0.9f, 0.3f, 0.5f, 0.5f };
                            edited::ColorEdit4("Color ", "Select a color for the glow", color_glow, picker_flags);

                            static int ticknes = 50;
                            edited::SliderInt("Thickness", "Luminescence thickness", &ticknes, 0, 100, "%d%%");

                            static char text[65] = { "" };
                            InputTextEx("v", "Enter your text here", text, 65, ImVec2(GetContentRegionMax().x - style->WindowPadding.x, 40), NULL);

                        }
                        edited::EndChild();
                    }
                    else if (active_tab == 2)
                    {
                        edited::BeginChild("##Container0", ImVec2((c::background::size.x - 200) / 2, c::background::size.y), NULL);
                        {
                            ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Create");

                            static char name[65] = { "" };
                            InputTextEx("v", "Config name", name, 65, ImVec2(GetContentRegionMax().x - style->WindowPadding.x, 40), NULL);

                            static char desc[65] = { "" };
                            InputTextEx("v", "Config description", desc, 65, ImVec2(GetContentRegionMax().x - style->WindowPadding.x, 40), NULL);

                        }
                        edited::EndChild();

                        SameLine(0, 0);

                        edited::BeginChild("##Container1", ImVec2((c::background::size.x - 200) / 2, c::background::size.y), NULL);
                        {

                        }
                        edited::EndChild();
                    }
                    else if (active_tab == 4)
                    {

                        edited::BeginChild("##Container0", ImVec2((c::background::size.x - 200) / 2, c::background::size.y), NULL);
                        {

                            edited::ColorEdit4("Accented Color", "Setting the main color of the menu.", color, picker_flags);

                            edited::Checkbox("ESP Preview", "Activate ESP Preview for settings", &enable_preview);

                            edited::Combo("Notify design", "Setting up notification styling", &notify_select, notify_items, IM_ARRAYSIZE(notify_items), 2);

                        }
                        edited::EndChild();

                    }

                    ImGui::PopStyleVar();

                }
                End();
            }

            PushFont(font::lexend_bold);

            const float cheat_name_size = ImGui::CalcTextSize(cheat_name).x;
            const float developer_size = ImGui::CalcTextSize(developer).x;
            const float role_size = ImGui::CalcTextSize(role).x;
            const float ping_size = ImGui::CalcTextSize(ping).x;
            const float fps_size = ImGui::CalcTextSize(fps).x;
            const float world_time_size = ImGui::CalcTextSize(world_time).x;
            const float bar_size = ImGui::CalcTextSize("|").x;

            const float ibar_size = cheat_name_size + bar_size + developer_size + bar_size + role_size + bar_size + ping_size + fps_size + developer_size + bar_size + world_time_size * 2 + bar_size * 3;

            ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - (ibar_size + 15), 15));
            ImGui::SetNextWindowSize(ImVec2(ibar_size, 45));

            ImGui::Begin("info-bar", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
            {
                const ImVec2& pos = ImGui::GetWindowPos(), spacing = style->ItemSpacing, region = ImGui::GetContentRegionMax();

                GetBackgroundDrawList()->AddRectFilled(pos, pos + region, ImGui::GetColorU32(c::background::filling), c::elements::rounding);
                GetBackgroundDrawList()->AddRect(pos, pos + region, ImGui::GetColorU32(c::background::stroke), c::elements::rounding);

                const char* info_set[6] = { cheat_name, developer, role, ping, fps, world_time };

                ImGui::SetCursorPos(ImVec2(spacing.x, (45 - ImGui::CalcTextSize(developer).y) / 2));
                ImGui::BeginGroup();
                {
                    for (int i = 0; i < ARRAYSIZE(info_set); i++) {
                        ImGui::TextColored(i < 1 ? ImColor(ImGui::GetColorU32(c::accent)) : ImColor(ImGui::GetColorU32(c::elements::text)), info_set[i]);
                        ImGui::SameLine();
                        ImGui::TextColored(ImColor(ImGui::GetColorU32(c::elements::text)), "|");
                        ImGui::SameLine();
                    }
                }
                ImGui::EndGroup();
            }
            ImGui::End();

            PopFont();


            if (enable_preview) {
                ImGui::SetNextWindowSize(ImVec2(350, 490));

                ImGui::Begin("ESP PREVIEW", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
                {
                    const ImVec2& pos = ImGui::GetWindowPos(), spacing = style->ItemSpacing, region = ImGui::GetContentRegionMax();

                    GetBackgroundDrawList()->AddRectFilled(pos, pos + region, ImGui::GetColorU32(c::background::filling), c::elements::rounding);
                    GetBackgroundDrawList()->AddRect(pos, pos + region, ImGui::GetColorU32(c::background::stroke), c::elements::rounding);

                    GetBackgroundDrawList()->AddRectFilledMultiColor(pos + ImVec2(0, region.y / 2), pos + ImVec2(2, region.y), ImGui::GetColorU32(c::accent), ImGui::GetColorU32(c::accent), ImGui::GetColorU32(c::background::filling), ImGui::GetColorU32(c::background::filling), c::elements::rounding);
                    GetBackgroundDrawList()->AddRectFilledMultiColor(pos + ImVec2(0, 0), pos + ImVec2(2, region.y / 2), ImGui::GetColorU32(c::background::filling), ImGui::GetColorU32(c::background::filling), ImGui::GetColorU32(c::accent), ImGui::GetColorU32(c::accent), c::elements::rounding);

                    GetBackgroundDrawList()->AddRectFilledMultiColor(pos + ImVec2(region.x - 2, region.y / 2), pos + ImVec2(region.x, region.y), ImGui::GetColorU32(c::accent), ImGui::GetColorU32(c::accent), ImGui::GetColorU32(c::background::filling), ImGui::GetColorU32(c::background::filling), c::elements::rounding);
                    GetBackgroundDrawList()->AddRectFilledMultiColor(pos + ImVec2(region.x - 2, 0), pos + ImVec2(region.x, region.y / 2), ImGui::GetColorU32(c::background::filling), ImGui::GetColorU32(c::background::filling), ImGui::GetColorU32(c::accent), ImGui::GetColorU32(c::accent), c::elements::rounding);

                    GetWindowDrawList()->AddText(pos + ImVec2(15, 15), GetColorU32(c::elements::text_active), "ESP PREVIEW");

                    SetCursorPosY(55);
                    edited::esp_preview(texture::esp_preview,
                        &esp_preview::nickname, esp_preview::nick_color,
                        &esp_preview::weapon, esp_preview::weapon_color,
                        &esp_preview::hp, esp_preview::hp_color,
                        &esp_preview::zoom, esp_preview::zoom_color,
                        &esp_preview::bomb, esp_preview::bomb_color,
                        &esp_preview::c4, esp_preview::c4_color,
                        &esp_preview::money, esp_preview::money_color,
                        &esp_preview::hit, esp_preview::hit_color,
                        &esp_preview::box, esp_preview::box_color,
                        &esp_preview::HP_line, esp_preview::hp_line_color);
                     
                    ImGui::SetCursorPos(ImVec2(20, 415));
                    ImGui::BeginGroup();
                    {
                        edited::checkboxP("Nickname", &esp_preview::nickname, ImVec2(CalcTextSize("Nickname").x + 5, 23));
                        ImGui::SameLine();
                        edited::checkboxP("Weapon", &esp_preview::weapon, ImVec2(CalcTextSize("Weapon").x + 5, 23));
                        ImGui::SameLine();
                        edited::checkboxP("Zoom", &esp_preview::zoom, ImVec2(CalcTextSize("Zoom").x + 5, 23));
                        ImGui::SameLine();
                        edited::checkboxP("Bomb", &esp_preview::bomb, ImVec2(CalcTextSize("Bomb").x + 5, 23));
                        ImGui::SameLine();
                        edited::checkboxP("HP", &esp_preview::HP_line, ImVec2(CalcTextSize("HP").x + 5, 23));

                        edited::checkboxP("Money", &esp_preview::money, ImVec2(CalcTextSize("Money").x + 5, 23));
                        ImGui::SameLine();
                        edited::checkboxP("Hit", &esp_preview::hit, ImVec2(CalcTextSize("Hit").x + 5, 23));
                        ImGui::SameLine();
                        edited::checkboxP("Box", &esp_preview::box, ImVec2(CalcTextSize("Box").x + 5, 23));
                        ImGui::SameLine();
                        edited::checkboxP("C4", &esp_preview::c4, ImVec2(CalcTextSize("C4").x + 5, 23));

                    }
                    ImGui::EndGroup();

                }
                ImGui::End();
            }

        }
        Render();

        const float clear_color_with_alpha[4] = { 0.f };

        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

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

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK) return false;

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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
