#include "custom.hpp"

#include <string>

using namespace ImGui;

int rotation_start_index;
void custom::ImRotateStart( )
{
    rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

ImVec2 custom::ImRotationCenter( )
{
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

    const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
}
void custom::ImRotateEnd( float rad, ImVec2 center )
{
    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

bool custom::tab( const char* icon, const char* label, bool selected ) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow( );

    ImGuiID id = window->GetID( label );

    ImVec2 p = window->DC.CursorPos;
    ImVec2 size( { window->Size.x, 24 } );
    ImRect bb( p, p + size );

    ItemSize( bb );
    ItemAdd( bb, id );

    bool hovered, held;
    bool pressed = ButtonBehavior( bb, id, &hovered, &held );

    float anim = ImTricks::Animations::FastFloatLerp( std::string( label ).append( "tab.anim" ), selected, 0.f, 1.f, 0.04f );
    auto col = ImTricks::Animations::FastColorLerp( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_Text ), anim );

    if ( pressed )
        content_anim = 0.f;

    static float line_pos = 0.f;
    line_pos = ImLerp( line_pos, selected ? bb.Min.y - window->Pos.y : line_pos, 0.04f );
    window->DrawList->AddRectFilled( { bb.Max.x - 2, window->Pos.y + line_pos }, { bb.Max.x, window->Pos.y + line_pos + size.y }, GetColorU32( ImGuiCol_Scheme, anim ), 2, ImDrawFlags_RoundCornersLeft );

    PushStyleColor( ImGuiCol_Text, col.Value );
    PushFont( GetIO( ).Fonts->Fonts[1] );
    RenderText( { bb.Min.x + 19, bb.GetCenter( ).y - CalcTextSize( icon ).y / 2 }, icon );
    PopFont( );
    RenderText( { bb.Min.x + 50, bb.GetCenter( ).y - CalcTextSize( label ).y / 2 }, label );
    PopStyleColor( );

    return pressed;
}

bool custom::subtab( const char* label, bool selected ) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow( );

    ImGuiID id = window->GetID( label );

    ImVec2 p = window->DC.CursorPos;
    ImVec2 size( { CalcTextSize( label, 0, true ).x + 10, window->Size.y } );
    ImRect bb( p, { p.x + size.x, p.y + size.y } );

    ItemSize( bb );
    ItemAdd( bb, id );

    bool hovered, held;
    bool pressed = ButtonBehavior( bb, id, &hovered, &held );

    float anim = ImTricks::Animations::FastFloatLerp( std::string( label ).append( "subtab.anim" ), selected, 0.f, 1.f, 0.04f );
    auto col = ImTricks::Animations::FastColorLerp( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_Scheme ), anim );

    window->DrawList->AddRectFilled( { bb.Min.x, bb.Max.y - 2 }, bb.Max, GetColorU32( ImGuiCol_Scheme, anim ), 2, ImDrawFlags_RoundCornersTop );

    PushStyleColor( ImGuiCol_Text, col.Value );
    RenderText( bb.GetCenter( ) - CalcTextSize( label, 0, true ) / 2, label );
    PopStyleColor( );

    return pressed;
}

void custom::begin_child( const char* name, ImVec2 size ) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow( );

    ImVec2 p = window->DC.CursorPos;

    BeginChild( std::string( name ).append( "main" ).c_str( ), size, true, ImGuiWindowFlags_NoScrollbar );
    GetWindowDrawList( )->AddText( GetIO( ).Fonts->Fonts[2], GetIO( ).Fonts->Fonts[0]->FontSize, { p.x + 15, p.y + 15 }, GetColorU32( ImGuiCol_Text ), name );
    SetCursorPosY( 50 );
    BeginChild( name, { size.x, size.y == 0 ? size.y : size.y - 50 }, false, ImGuiWindowFlags_NoBackground );
    SetCursorPosX( 15 );

    BeginGroup( );

    PushStyleVar( ImGuiStyleVar_ItemSpacing, { 10, 12 } );
}

void custom::end_child( ) {
    PopStyleVar( );
    EndGroup( );
    EndChild( );
    EndChild( );
}

bool custom::collapse_button( bool collapsed ) {
    BeginChild( "123", { 17, 17 } );

    ImGuiWindow* window = GetCurrentWindow( );

    ImGuiID id = window->GetID( "collapse_button" );

    ImVec2 p = window->DC.CursorPos;
    ImVec2 size( GetWindowSize( ) );
    ImRect bb( p, p + size );

    ItemSize( bb );
    ItemAdd( bb, id );

    bool hovered, held;
    bool pressed = ButtonBehavior( bb, id, &hovered, &held );

    window->DrawList->AddCircleFilled( bb.GetCenter( ), bb.GetWidth( ) / 2, GetColorU32( ImGuiCol_ChildBg ) );
    window->DrawList->AddCircle( bb.GetCenter( ), bb.GetWidth( ) / 2, GetColorU32( ImGuiCol_Border ) );

    float anim = ImTricks::Animations::FastFloatLerp( "collapse_button.anim", collapsed, 0.f, 1.f, 0.04f );

    ImRotateStart( );
    RenderArrow( window->DrawList, bb.GetCenter( ) - ImVec2( 2 + 2 * !anim, 6 ), GetColorU32( ImGuiCol_TextDisabled ), ImGuiDir_Down, 9.f );
    ImRotateEnd( 3.14f * anim );
    
    EndChild( );

    return pressed;
}
