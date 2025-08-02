#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/backend/imgui_impl_dx9.h"
#include "../imgui/backend/imgui_impl_win32.h"
#include "blur/impl.hpp"
#include "animations.hpp"
#include "hashes.hpp"
#include "image.hpp"
#include "bytes.hpp"

#include <string>
#include <vector>
#include <functional>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

using namespace ImGui;

#define font( n ) ImGui::GetIO( ).Fonts->Fonts[ n ]

#define ALPHA    ( ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )
#define NO_ALPHA ( ImGuiColorEditFlags_NoTooltip    | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )

struct tab_t {
	const char* m_icon;
	const char* m_name;

	int cur_subtab = 0;
	int next_subtab = 0;
	std::vector< const char* > m_subtabs;
	std::vector< std::function< void( ) > > pages;

	tab_t( const char* icon, const char* name, std::vector< const char* > subtabs ) {
		m_icon = icon;
		m_name = name;
		m_subtabs = subtabs;
	}
};

namespace ui {
	inline ImVec2 size{ 670, 478 };

	inline float anim = 0.f;
	inline float anim_dst = 1.f;
	inline float content_anim = 0.f;
	inline float content_anim_dst = 1.f;
	void handle_anims( );

	void render_page( );
	void add_page( int tab_idx, std::function< void( ) > c );

	void rotate_start( );
    ImVec2 rotation_center( );
    void rotate_end( float rad, ImVec2 center );

	inline int cur_tab = 0;
	inline int next_tab = 0;
	inline std::vector< tab_t > tabs = {
		{ ICON_FA_SKULL, "Ragebot", { "Aimbot", "Exploits", "Anti-aim", "Misc##1" } },
		{ ICON_FA_CROSSHAIRS, "Legitbot", { "Aimbot##1", "Triggerbot" } },
		{ ICON_FA_EYE, "Visuals", { "Enemies", "Allies", "World" } },
		{ ICON_FA_GEAR, "Misc", { "Subtab", "Subtab 1" } },
		{ ICON_FA_BRUSH, "Skins", { } },
		{ ICON_FA_CODE, "Scripts", { } },
		{ ICON_FA_FILE, "Configs", { } }
	};
	void tab( int num );
	void subtab( int num, int tab_num );

	void begin_child( const char* name, ImVec2 size );
	void end_child( );

	bool color( const char* name, float col[4], bool alpha = true );
}