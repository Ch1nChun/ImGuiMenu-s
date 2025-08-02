#include "ui.hpp"

void ui::handle_anims( ) {
    anim = ImLerp( anim, anim_dst, 21.f * ImGui::GetIO( ).DeltaTime );
    if ( abs( anim - anim_dst ) < 0.01f ) {
        anim_dst = 1.f;
        cur_tab = next_tab;
    }

    content_anim = ImLerp( content_anim, content_anim_dst, 21.f * ImGui::GetIO( ).DeltaTime );
    if ( abs( content_anim - content_anim_dst ) < 0.01f ) {
        content_anim_dst = 1.f;
        tabs[cur_tab].cur_subtab = tabs[cur_tab].next_subtab;
    }
}

void ui::render_page( ) {
	if ( tabs[cur_tab].pages.size( ) == 0 || tabs[cur_tab].pages.size( ) <= tabs[cur_tab].cur_subtab )
        return;

    tabs[cur_tab].pages[tabs[cur_tab].cur_subtab]( );
}

void ui::add_page( int tab_idx, std::function< void( ) > c ) {
	tabs[tab_idx].pages.emplace_back( c );
}

int rotation_start_index;
void ui::rotate_start( ) {
    rotation_start_index = ImGui::GetWindowDrawList( )->VtxBuffer.Size;
}

ImVec2 ui::rotation_center( ) {
    ImVec2 l( FLT_MAX, FLT_MAX ), u( -FLT_MAX, -FLT_MAX );

    const auto& buf = ImGui::GetWindowDrawList( )->VtxBuffer;
    for ( int i = rotation_start_index; i < buf.Size; i++ )
        l = ImMin( l, buf[i].pos ), u = ImMax( u, buf[i].pos );

    return ImVec2( ( l.x + u.x ) / 2, ( l.y + u.y ) / 2 );
}

void ui::rotate_end( float rad, ImVec2 center ) {
    float s = sin( rad ), c = cos( rad );
    center = ImRotate( center, s, c ) - center;

    auto& buf = ImGui::GetWindowDrawList( )->VtxBuffer;
    for ( int i = rotation_start_index; i < buf.Size; i++ )
        buf[i].pos = ImRotate( buf[i].pos, s, c ) - center;
}

void ui::tab( int num ) {
    auto icon = tabs[num].m_icon;
    auto name = tabs[num].m_name;

    bool selected = ui::next_tab == num;

    auto window = ImGui::GetCurrentWindow( );
    auto id = window->GetID( name );

    ImVec2 icon_size = CalcTextSize( icon, 0, 1 );
    ImVec2 name_size = font( 2 + selected )->CalcTextSizeA( font( 2 + selected )->FontSize, FLT_MAX, 0.f, name, FindRenderedTextEnd( name ) );
    
    ImVec2 size{ window->Size.x, 38 };
    ImRect bb{ window->DC.CursorPos, window->DC.CursorPos + size };
    ItemAdd( bb, id );
	ItemSize( bb );

    bool hovered, held;
    bool pressed = ButtonBehavior( bb, id, &hovered, &held );

    if ( pressed && !selected ) {
        ui::next_tab = num;
        ui::anim_dst = 0.f;
    }

    struct s {
		float cur;
		float hover;
        float rad;
        ImColor col;
	}; auto& obj = anim_obj( name, 0, s{ 0.f, 0.f, 0.f, 0 } );
    
	obj.cur = animate( obj.cur, 0.f, 1.f, selected );
	obj.hover = animate( obj.hover, 0.f, 1.f, hovered );
    obj.rad = animate( obj.rad, IM_PI / 2, IM_PI * 1.5f, selected );
    obj.col = col_anim( col_anim( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_Text, 0.75f ), obj.hover ), GetColorU32( ImGuiCol_Text ), obj.cur );

    window->DrawList->AddText( { bb.Min.x + 15, bb.GetCenter( ).y - icon_size.y / 2 }, obj.col, icon, FindRenderedTextEnd( icon ) );
    window->DrawList->AddText( font( 2 + selected ), font( 2 + selected )->FontSize, { bb.Min.x + 40, bb.GetCenter( ).y - name_size.y / 2 }, obj.col, name, FindRenderedTextEnd( name ) );

    if ( tabs[num].m_subtabs.size( ) > 0 ) {
        ui::rotate_start( );
        RenderArrow( window->DrawList, { bb.Max.x - 25, bb.GetCenter( ).y - 8 }, GetColorU32( ImGuiCol_Text ), ImGuiDir_Down, 12 );
        ui::rotate_end( obj.rad, ui::rotation_center( ) );

        if ( obj.cur > 0.01f ) {
            PushStyleVar( ImGuiStyleVar_Alpha, obj.cur );
            PushStyleVar( ImGuiStyleVar_WindowPadding, { 40, 0 } );
            BeginChild( std::string( name ).append( "##subtabs" ).c_str( ), { size.x, 1 }, 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse ); {
                SetWindowSize({ size.x, GetCurrentWindow( )->ContentSize.y * obj.cur });

                PushStyleVar( ImGuiStyleVar_ItemSpacing, { 10, 10 } );
                for ( int i = 0; i < tabs[num].m_subtabs.size( ); ++i ) {
                    ui::subtab( i, num );
                }
                Dummy({ 0, 0 });
                PopStyleVar( );
            } EndChild( );
            PopStyleVar( 2 );
        }
    }

    window->DrawList->AddLine( { bb.Min.x + 40, window->DC.CursorPos.y }, { bb.Max.x, window->DC.CursorPos.y }, GetColorU32( ImGuiCol_Border, 0.5f ) );
}

void ui::subtab( int num, int tab_num ) {
    auto name = tabs[tab_num].m_subtabs[num];
    auto name_size = ImGui::CalcTextSize( name, 0, 1 );

    bool selected = tabs[tab_num].next_subtab == num;

    auto window = ImGui::GetCurrentWindow( );
    auto id = window->GetID( name );
    auto g = *GImGui;

    ImRect bb{ window->DC.CursorPos, window->DC.CursorPos + name_size };
    ItemAdd( bb, id );
    ItemSize( bb );

    bool hovered, held;
    bool pressed = ButtonBehavior( bb, id, &hovered, &held );

    if ( pressed && !selected ) {
        tabs[tab_num].next_subtab = num;
        content_anim_dst = 0.f;
    }

    struct s {
        float anim;
        float hover;
        ImColor col;
    }; auto& obj = anim_obj( name, 0, s{ 0.f, 0.f, 0 } );

    obj.anim = animate( obj.anim, 0.f, 1.f, selected );
    obj.hover = animate( obj.hover, 0.f, 1.f, hovered );
    obj.col = col_anim( col_anim( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_Text ), obj.hover ), GetColorU32( ImGuiCol_Scheme ), obj.anim );

    window->DrawList->AddText( bb.Min, obj.col, name, FindRenderedTextEnd( name ) );
}

void ui::begin_child( const char* name, ImVec2 size ) {
    ImGui::BeginChild( std::string( name ).append( "decor" ).c_str( ), CalcItemSize( size, ( ImGui::GetWindowWidth( ) - GImGui->Style.WindowPadding.x * 2 ) / 2 - GImGui->Style.ItemSpacing.x / 2, 0 ), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground );
	ImGui::GetWindowDrawList( )->AddRectFilled( ImGui::GetWindowPos( ), ImGui::GetWindowPos( ) + ImGui::GetWindowSize( ), ImGui::GetColorU32( ImGuiCol_ChildBg ), GImGui->Style.ChildRounding );

    if ( ImGui::CalcTextSize( name, 0, 1 ).x > 0 ) {
        ImGui::GetWindowDrawList( )->AddRectFilled( ImGui::GetWindowPos( ), { ImGui::GetWindowPos( ).x + ImGui::GetWindowWidth( ), ImGui::GetWindowPos( ).y + 33 }, ImGui::GetColorU32( ImGuiCol_Header ), GImGui->Style.ChildRounding, ImDrawFlags_RoundCornersTop );
        ImGui::GetWindowDrawList( )->AddLine( { ImGui::GetWindowPos( ).x, ImGui::GetWindowPos( ).y + 33 }, { ImGui::GetWindowPos( ).x + ImGui::GetWindowWidth( ), ImGui::GetWindowPos( ).y + 33 }, ImGui::GetColorU32( ImGuiCol_BorderShadow ) );
        ImGui::GetWindowDrawList( )->AddText( font( 1 ), font( 1 )->FontSize, { ImGui::GetWindowPos( ).x + 13, ImGui::GetWindowPos( ).y + 33 / 2 - font( 1 )->CalcTextSizeA( font( 1 )->FontSize, FLT_MAX, 0.f, name, ImGui::FindRenderedTextEnd( name ) ).y / 2 }, ImGui::GetColorU32( ImGuiCol_Text ), name, ImGui::FindRenderedTextEnd( name ) );
        ImGui::SetCursorPosY( 34 );
    }

    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 13, 15 } );
    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, { 8, 14 } );
    ImGui::BeginChild( name, { -1, size.y == 0 ? size.y : -1 }, 0, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoBackground );
}

void ui::end_child( ) {
    ImGui::EndChild( );
    ImGui::PopStyleVar( 2 );
    ImGui::EndChild( );
}

bool ui::color( const char* name, float col[4], bool alpha ) {
	auto window = GetCurrentWindow( );
    auto id		= window->GetID( name );

    auto draw = window->DrawList;
	auto pos  = window->DC.CursorPos;

	ImVec2 size{ 16, 16 };
	auto label_size = CalcTextSize( name, 0, 1 );

    ImRect bb( pos, { pos.x + CalcItemWidth( ) - size.x, pos.y + size.y } );
    ItemAdd( bb, id );
    ItemSize( bb );

	draw->AddText( { bb.Min.x, bb.GetCenter( ).y - label_size.y / 2 }, GetColorU32( ImGuiCol_Text ), name, FindRenderedTextEnd( name ) );

	bool value_changed = false;
	if ( col ) {
        pos = window->DC.CursorPos;
        window->DC.CursorPos = ImVec2{ bb.Max.x, bb.Min.y };

		value_changed = ColorEdit4( std::string( name ).append( "##col" ).c_str( ), col, alpha ? ALPHA : NO_ALPHA );

        window->DC.CursorPos = pos;
    }

	return value_changed;
}