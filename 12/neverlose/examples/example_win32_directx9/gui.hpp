#pragma once

#define  IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include "color_t.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

using namespace std;

class c_gui {

public:

    float m_anim = 0.f;
    int m_tab = 0;

    int m_rage_subtab = 0;
    vector < const char* > rage_subtabs = { "General", "Anti aim", "Subtab" };

    color_t accent_color = { 0.3f, 0.49f, 1.f, 1.f };

    color_t text = { 1.f, 1.f, 1.f, 1.f };
    color_t text_disabled = { 0.51f, 0.52f, 0.56f, 1.f };

    color_t border = { 1.f, 1.f, 1.f, 0.03f };

    color_t frame_inactive = { 0.023f, 0.039f, 0.07f, 1.f };
    color_t frame_active = { 0.043f, 0.07f, 0.137f, 1.f };

    color_t button = { 0.031f, 0.035f, 0.058f, 1.f };
    color_t button_hovered = { 0.050f, 0.054f, 0.078f, 1.f };
    color_t button_active = { 0.07f, 0.074f, 0.098f, 1.f };

    color_t group_box_bg = { 0.019f, 0.035f, 0.062f, 1.f };

    void render_circle_for_horizontal_bar( ImVec2 pos, ImColor color, float alpha );

    inline void group_title( const char* name ) {

        SetCursorPosX( GetCursorPosX( ) + 10 );
        PushStyleColor( ImGuiCol_Text, ImVec4( 1.f, 1.f, 1.f, 0.5f ) );
        Text( name );
        PopStyleColor( );
    }

    void group_box( const char* name, ImVec2 size_arg );
    void end_group_box( );

    bool tab( const char* icon, const char* label, bool selected );
    bool subtab( const char* label, bool selected, int size, ImDrawFlags flags );

};

inline c_gui gui;
