#pragma warning(disable:4244)
#pragma warning(disable:4700)
#pragma warning(disable:4189)
#pragma warning(disable:4100)
#pragma warning(disable:4389)

#include "imgui\imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui\imgui_internal.h"
#include <d3d9.h>

#define DIRECTINPUT_VERSION 0x0800
#define SPACE u8"	   "

#include <dinput.h>
#include <string>
#include <tchar.h>

#include "faprolight.hpp"
#include "faproreg.hpp"
#include "faprosolid.hpp"
#include "sffont.hpp"

#include "imgui_rotate.hpp"
#include "hashes.h"
#include "customgui.h"

static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

ImFont* SFCustom;
const int SFCustomSz = 12;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void lua() {

	static int selectedItem = 0;
	static const char* items[] = { "First", "Second", "Third" };
	bool selected[3] = { false, false, false };

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 25 , 25 });
	ImGui::BeginChild("##childfree", ImVec2(400, 510), true);
	{
		ImGui::Text("FIRST CHILD##22");
		ImGui::Separator();

		ImGui::ListBox("Lua List Box", &selectedItem, items, IM_ARRAYSIZE(items));

		switch (selectedItem)
		{
		case 0: ImGui::Text("1"); break;
		case 1: ImGui::Text("2"); break;
		case 2: ImGui::Text("3"); break;
		default:  ImGui::Text("Fuck off!");
			break;
		}
	}
	ImGui::EndChild();
	ImGui::PopStyleVar(1);
}

void aimbot() {

	static bool goddamn = false, fuckfuck = false;
	static int selectedItem = 0, someint = 0;
	static const char* items[] = { "First", "Second", "Third", "IDK", "Super", "Huh?" };
	static char textexamp[1024] = "";

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 25 , 25 });
	ImGui::BeginChild("##childfree", ImVec2(400, 510), true);
	{
		ImGui::Text("FIRST CHILD");
		ImGui::Separator();

		CGUI::Checkbox("Check", &goddamn);
		CGUI::Checkbox("Checkiro!", &fuckfuck);

		ImGui::Text("Some Text Here!");

		CGUI::Button("Some button IDK 2");
		CGUI::Button("Buttonchik");

		//CGUI::TextWFont("dick pic 4K", SFCustom);

		ImGui::PushItemWidth(150);
		CGUI::Combo("New combo", &selectedItem, items, IM_ARRAYSIZE(items));
	}
	ImGui::EndChild();
	ImGui::PopStyleVar(1);

	ImGui::SameLine();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 25 , 25 });
	ImGui::BeginChild("##childfree2", ImVec2(400, 510), true);
	{
		ImGui::Text("SECOND CHILD");
		ImGui::Separator();

		ImGui::Text("Some text bruh..."); CGUI::ShowHelpMarker("Help", true, -5.f); // Distance of "helper" can be changed

		CGUI::InputInt("Input Int", &selectedItem);
		CGUI::Button("Button");

		CGUI::SliderInt("selectedItem", &selectedItem, 0, 25, "%0f");
		CGUI::SliderInt("fuck fuc k", &someint, 0, 45, "%0f"); CGUI::ShowHelpMarker("SLIDER LOL");

	}
	ImGui::EndChild();
	ImGui::PopStyleVar(1);
}

int main(int, char**)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX9 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

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



	ImFontConfig font_cfg{};
	font_cfg.FontDataOwnedByAtlas = false;
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(sfpro_compressed_data, sfpro_compressed_size, 16, &font_cfg, io.Fonts->GetGlyphRangesDefault());

	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;

	io.Fonts->AddFontFromMemoryTTF(&faprosolid, sizeof faprosolid, 10, &icons_config, icon_ranges),
		io.Fonts->AddFontFromMemoryTTF(&faproreg, sizeof faproreg, 10, &icons_config, icon_ranges);
	io.Fonts->AddFontFromMemoryTTF(&faprolight, sizeof faprolight, 10, &icons_config, icon_ranges);

	io.IniFilename = NULL;

	SFCustom = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(sfpro_compressed_data, sfpro_compressed_size, SFCustomSz, NULL, io.Fonts->GetGlyphRangesDefault());

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

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

		static int animateid[2048];

		for (int i; i <= 2048; i++)
		{
			animateid[i] = 0;
		}

		ImVec2 pos = ImVec2();
		ImDrawList* draw_list;
		ImDrawList* overlay_draw_list;

		static bool draw_resizer = false;
		static bool start_alpha_animate = false;
		static int resizer_alpha = 0;
		static int background_alpha = 0;
		static bool resize_state = false;
		static float resize_volume = 0;

		static const char* current_tab_name = "Aimbot";

		static int tab = 0;

		// FLAGS
		auto window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus;

		// ATRIBUTES
		ImGui::SetNextWindowSize({ 80 + resize_volume, 650 });

		// STYLES
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0);

		// COLORS
		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0 / 255.f,0 / 255.f,0 / 255.f,0 / 255.f });
		ImGui::PushStyleColor(ImGuiCol_Text, { 255 / 255.f,255 / 255.f,255 / 255.f,255.f / 255.f });
		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, { 35.f / 255.f,35.f / 255.f,35.f / 255.f,0 / 255.f });
		ImGui::PushStyleColor(ImGuiCol_PopupBg, { 35.f / 255.f,35.f / 255.f,35.f / 255.f,0 / 255.f });


		ImGui::Begin("##sidebar", nullptr, window_flags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		{
			pos = ImGui::GetWindowPos();
			draw_list = ImGui::GetWindowDrawList();
			overlay_draw_list = ImGui::GetOverlayDrawList();

			draw_list->AddRectFilled(pos, { pos.x + 70 + resize_volume, pos.y + 650 }, ImColor(20, 20, 25), 8, ImDrawCornerFlags_Left);

			if (draw_resizer)
			{
				if (resizer_alpha < 250)
					resizer_alpha += 15;
			}
			else
			{
				if (resizer_alpha > 0)
					resizer_alpha -= 15;
			}

			if (resize_state)
			{
				if (resize_volume < 110)
					resize_volume += 15;
			}
			else
			{
				if (resize_volume > 0)
					resize_volume -= 15;
			}

			ImColor active = ImColor(255, 255, 255);

			CGUI::DrawLogo(ImVec2(pos.x + ((70 + resize_volume) / 2), pos.y + 30), 15, true);

			if (resize_state)
				ImGui::SetCursorPos({ 15, 50 }),
				CGUI::TextWFont("NAVIGATION", SFCustom, ImGui::GetColorU32(ImGuiCol_TextDisabled));

			ImGui::SetCursorPos({ 15, 70 });
			ImGui::BeginGroup();
			{

				if (CGUI::Tab("Aimbot", ICON_FA_CROSSHAIRS, tab == 0, resize_state, active, &animateid[1], &animateid[2]))
					tab = 0, current_tab_name = "AIMBOT";

				if (CGUI::Tab("Triggerbot", ICON_FA_BULLSEYE, tab == 1, resize_state, active, &animateid[5], &animateid[6]))
					tab = 1, current_tab_name = "TRIGGERBOT";

				if (CGUI::Tab("ESP", ICON_FA_USER, tab == 2, resize_state, active, &animateid[7], &animateid[8]))
					tab = 2, current_tab_name = "ESP";

				if (CGUI::Tab("Chams & Glow", ICON_FA_STREET_VIEW, tab == 3, resize_state, active, &animateid[9], &animateid[10]))
					tab = 3, current_tab_name = "CHAMS & GLOW";

				if (CGUI::Tab("Visuals", ICON_FA_MAP_MARKED, tab == 4, resize_state, active, &animateid[11], &animateid[12]))
					tab = 4, current_tab_name = "VISUALS";

				if (CGUI::Tab("Skinchanger", ICON_FA_STAR, tab == 5, resize_state, active, &animateid[13], &animateid[14]))
					tab = 5, current_tab_name = "SKINCHANGER";

				if (CGUI::Tab("Misc", ICON_FA_BARS, tab == 6, resize_state, active, &animateid[15], &animateid[16]))
					tab = 6, current_tab_name = "MISC";

				if (CGUI::Tab("Settings", ICON_FA_COGS, tab == 7, resize_state, active, &animateid[17], &animateid[18]))
					tab = 7, current_tab_name = "SETTINGS";

				if (CGUI::Tab("Lua Scripts", ICON_FA_FILE, tab == 8, resize_state, active, &animateid[19], &animateid[20]))
					tab = 8, current_tab_name = "LUA SCRIPTS";

				if (CGUI::Tab("Console", ICON_FA_CODE, tab == 9, resize_state, active, &animateid[21], &animateid[22]))
					tab = 9, current_tab_name = "CONSOLE";
			}
			ImGui::EndGroup();

			if (resize_state)
				ImGui::SetCursorPos({ 15, 480 }),
				CGUI::TextWFont("FOLDERS", SFCustom, ImGui::GetColorU32(ImGuiCol_TextDisabled));

			ImGui::SetCursorPos({ 15, 500 });
			ImGui::BeginGroup();
			{

				if (CGUI::Tab("Favorites", u8"\uf07c", tab == 10, resize_state, active, &animateid[23], &animateid[24], ImColor(222, 140, 255)))
					tab = 10, current_tab_name = "Favorites";
			}
			ImGui::EndGroup();



			draw_list->AddRectFilled({ pos.x + 20, pos.y + 600 }, { pos.x + 50, pos.y + 630 }, ImGui::GetColorU32(ImGuiCol_FrameBg), 4.f);
			if (resize_state)
				ImGui::SetCursorPos({ 60, 600 }),
				ImGui::Text("colebyte2"),
				ImGui::SetCursorPos({ 60, 615 }),
				CGUI::TextWFont("ADMINISTRATOR", SFCustom, ImColor(
					71, 148, 255
				));

			ImGui::SetCursorPos({ 61 + resize_volume, 10 });
			if (ImGui::InvisibleButton("><", ImVec2(40, 650)))
			{
				resize_state = !resize_state;
				draw_resizer = false;
			}

			if (ImGui::GetMousePos().x - pos.x > 55 + resize_volume && ImGui::GetMousePos().y - pos.y > 5 && ImGui::GetMousePos().y - pos.y < 650 && ImGui::GetMousePos().x - pos.x < 85 + resize_volume)
				draw_resizer = true;
			else
				draw_resizer = false;

			if (resize_state)
			{
				if (background_alpha < 150)
					background_alpha += 15;
			}
			else
			{
				if (background_alpha > 0)
					background_alpha -= 15;
			}

			overlay_draw_list->AddRectFilled({ pos.x + 70 + resize_volume,pos.y }, { pos.x + 1020, pos.y + 650 }, ImColor(20, 20, 20, background_alpha), ImGui::GetStyle().WindowRounding, ImDrawCornerFlags_Right);

			overlay_draw_list->AddLine({ pos.x + 70 + resize_volume,pos.y }, { pos.x + 70 + resize_volume,pos.y + 650 }, ImColor(64, 139, 255, resizer_alpha), 1);
			overlay_draw_list->AddCircleFilled({ pos.x + 70 + resize_volume,pos.y + 75 }, 9, ImColor(64, 139, 255, resizer_alpha), 32);

			if (!resize_state)
				overlay_draw_list->AddText({ pos.x + 67 + resize_volume,pos.y + 65 }, ImColor(255, 255, 255, resizer_alpha), ICON_FA_CHEVRON_RIGHT);
			else
				overlay_draw_list->AddText({ pos.x + 67 + resize_volume,pos.y + 65 }, ImColor(255, 255, 255, resizer_alpha), ICON_FA_CHEVRON_LEFT);

			if (ImGui::GetMousePos().x - pos.x > 65 + resize_volume && ImGui::GetMousePos().y - pos.y > 0 && ImGui::GetMousePos().y - pos.y < 600 && ImGui::GetMousePos().x - pos.x < 1020)
			{
				if (resize_state && io.MouseClicked[0])
					resize_state = !resize_state;
			}

			static int lin = 255;
			if (resize_state)
			{
				if (lin >= 0)
					lin -= 8.5 * (60 / io.Framerate);

				overlay_draw_list->AddRectFilledMultiColor(
					{ pos.x + 0/*45*/, pos.y },
					{ pos.x + 70 + resize_volume, pos.y + 600 },
					ImColor(20, 20, 25, lin * 0.45),
					ImColor(20, 20, 25, lin),
					ImColor(20, 20, 25, lin),
					ImColor(20, 20, 25, lin * 0.45)
				);
			}
			else lin = 255;

		}
		ImGui::End();

		// STYLES
		ImGui::PopStyleVar(6);

		// COLORS
		ImGui::PopStyleColor(4);

		// NEXT WINDOW SETTINGS

		// ATRIBUTES
		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowSize({ 1020, 650 });

		// STYLES
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 90 , 80 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0);

		ImGui::Begin("##MAINWND", nullptr, window_flags | ImGuiWindowFlags_FalconTriangle);
		{
			ImVec2 w_pos = ImGui::GetWindowPos();
			ImVec2 start = ImVec2(ImGui::GetWindowPos().x + resize_volume + 70, ImGui::GetWindowPos().y);
			ImVec2 end = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y);

			//Upper Rect
			draw_list->AddRect(start, end, ImColor(75, 82, 92), ImGui::GetStyle().WindowRounding, ImDrawCornerFlags_Right);
			draw_list->AddRectFilled({ start.x, start.y }, { ImGui::GetWindowSize().x + ImGui::GetWindowPos().x , start.y + 60 }, ImColor(35, 36, 41)/*ImColor(34, 36, 41)*/);
			draw_list->AddRect(start, { ImGui::GetWindowSize().x + ImGui::GetWindowPos().x, start.y + 61 }, ImColor(75, 82, 92));
			draw_list->AddRectFilledMultiColor({ start.x + 1, start.y + 61 }, { ImGui::GetWindowSize().x + ImGui::GetWindowPos().x, start.y + 100 }, ImColor(35, 36, 41), ImColor(35, 36, 41), ImColor(35, 36, 41, 0), ImColor(35, 36, 41, 0));

			static int subtab = 0;

			ImGui::SetCursorPos({ 69, 2 });
			if (CGUI::SubTab("menu.subtab.aimbot", subtab == 0))
				subtab = 0;
			ImGui::SameLine();
			if (CGUI::SubTab("menu.subtab.triggerbot", subtab == 1))
				subtab = 1;

			ImGui::SetCursorPos({ 90 , 80 });
			ImGui::BeginGroup();
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 25 , 25 });
				ImGui::BeginChild("##aimbot.weapons", ImVec2(290, 545), true);
				{
					static bool check;
					CGUI::Checkbox("check", &check);

					static int slider;
					CGUI::SliderInt("slider", &slider, 0, 100, "%.0f");

					CGUI::Button("Button", { -1, 25.0f });

					if (CGUI::BeginCombo("Flags", "...", NULL))
					{
						ImGui::Selectable(("Armor"), true, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::Selectable(("Defusing"), false, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::Selectable(("Defuse Kit"), false, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleVar(1);
			}
			ImGui::EndGroup();
		}
		ImGui::End();

		// STYLES
		ImGui::PopStyleVar(6);

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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
