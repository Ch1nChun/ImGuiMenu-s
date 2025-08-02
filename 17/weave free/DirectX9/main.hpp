#pragma once
#include "ui/ui.hpp"

inline static LPDIRECT3D9           g_pD3D = nullptr;
inline static LPDIRECT3DDEVICE9     g_pd3dDevice = nullptr;
inline static bool                  g_DeviceLost = false;
inline static UINT                  g_ResizeWidth = 0, g_ResizeHeight = 0;
inline static D3DPRESENT_PARAMETERS g_d3dpp = {};

inline LPDIRECT3DTEXTURE9 bg;
inline LPDIRECT3DTEXTURE9 logo;
inline LPDIRECT3DTEXTURE9 avatar;

class c_main {
private:

public:
    void initialize( ImGuiIO& io, LPDIRECT3DDEVICE9 device ) {
        ImGui::StyleColorsDark( );

        auto config = ImFontConfig( );
        config.FontDataOwnedByAtlas = false;

        static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        auto icons_config = ImFontConfig( );
	    icons_config.MergeMode = true;
	    icons_config.PixelSnapH = true;
        icons_config.FontDataOwnedByAtlas = false;

        io.Fonts->AddFontFromMemoryTTF( font_medium_bin, sizeof( font_medium_bin ), 14, &config );
        io.Fonts->AddFontFromMemoryTTF( font_awesome_bin, sizeof( font_awesome_bin ), 13, &icons_config, icon_ranges );
        io.Fonts->AddFontFromMemoryTTF( font_bold_bin, sizeof( font_bold_bin ), 14, &config );
        io.Fonts->AddFontFromMemoryTTF( font_medium_bin, sizeof( font_medium_bin ), 17, &config );
        io.Fonts->AddFontFromMemoryTTF( font_bold_bin, sizeof( font_bold_bin ), 17, &config );

        D3DXCreateTextureFromFileInMemory( device, imageb, sizeof( imageb ), &bg );
        D3DXCreateTextureFromFileInMemory( device, logob, sizeof( logob ), &logo );
        D3DXCreateTextureFromFileInMemory( device, avatarb, sizeof( avatarb ), &avatar );

        ui::add_page( 0, [ ]( ){
            static int ints[8];
            static char buf[32];
            static bool bools[8];
            static float color[4] = { 1.f, 1.f, 1.f, 1.f };

            ImGui::BeginGroup( ); {
                ui::begin_child( "General", { ( ImGui::GetWindowWidth( ) - GImGui->Style.WindowPadding.x * 2 ) / 2 - GImGui->Style.ItemSpacing.x / 2, ImGui::GetWindowHeight( ) - GImGui->Style.WindowPadding.y * 2 } ); {
                    ImGui::Checkbox( "Enabled", &bools[0] );
                    ImGui::Checkbox( "Disabled", &bools[1] );
                    ImGui::Combo( "Combobox", &ints[0], "Value\0Value 1\0Value 2\0Value 3\0Value 4" );
                    ImGui::InputText( "Textfield", buf, sizeof( buf ) );
                    ImGui::SliderInt( "Slider", &ints[1], 0, 100, "(%d%%)" );
                }
                ui::end_child( );
            }
            ImGui::EndGroup( );

            ImGui::SameLine( );

            ImGui::BeginGroup( ); {
                ui::begin_child( "Weapon", { ( ImGui::GetWindowWidth( ) - GImGui->Style.WindowPadding.x * 2 ) / 2 - GImGui->Style.ItemSpacing.x / 2, ( ImGui::GetWindowHeight( ) - GImGui->Style.WindowPadding.y * 2 ) / 2 - GImGui->Style.ItemSpacing.y / 2 } ); {
                    ui::color( "Colorpicker", color );
                    ImGui::Hotkey( "Binder", &ints[2] );
                    ImGui::Button( "Button", { ImGui::CalcItemWidth( ), 32 } );
                }
                ui::end_child( );

                ui::begin_child( "Miscellaneous", { ( ImGui::GetWindowWidth( ) - GImGui->Style.WindowPadding.x * 2 ) / 2 - GImGui->Style.ItemSpacing.x / 2, ( ImGui::GetWindowHeight( ) - GImGui->Style.WindowPadding.y * 2 ) / 2 - GImGui->Style.ItemSpacing.y / 2 } ); {
                    static bool var[16];
                    for ( int i = 0; i < sizeof( var ) / sizeof( var[0] ); ++i )
                        ImGui::Checkbox( std::string( "Checkbox " ).append( std::to_string( i ) ).c_str( ), &var[i] );
                }
                ui::end_child( );
            }
            ImGui::EndGroup( );
        } );

        imgui_blur::set_device( device );
    }

    void render( ) {
        ImGui::GetBackgroundDrawList( )->AddImage( bg, { 0, 0 }, ImGui::GetIO( ).DisplaySize );

        ImGui::SetNextWindowSize( ui::size );
        ImGui::Begin( "weave", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground ); {
            ImGui::BeginChild( "nav", { 176, ImGui::GetWindowHeight( ) }, 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar ); {
                ImGui::BeginChild( "brand", { ImGui::GetWindowWidth( ), 42 }, 0, ImGuiWindowFlags_NoBackground ); {
                    ImGui::GetWindowDrawList( )->AddRectFilled( ImGui::GetWindowPos( ), { ImGui::GetWindowPos( ).x + ImGui::GetWindowWidth( ), ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) - 2 }, ImGui::GetColorU32( ImGuiCol_ChildBg ), GImGui->Style.WindowRounding, ImDrawFlags_RoundCornersTop );
                    ImGui::GetWindowDrawList( )->AddRectFilled( { ImGui::GetWindowPos( ).x, ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) - 2 }, { ImGui::GetWindowPos( ).x + ImGui::GetWindowWidth( ), ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) }, ImGui::GetColorU32( ImGuiCol_Scheme ) );
                    ImGui::GetWindowDrawList( )->AddImage( logo, { ImGui::GetWindowPos( ).x + ImGui::GetWindowWidth( ) / 2 - 31 / 2, ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) / 2 - 14 / 2 - 2 }, { ImGui::GetWindowPos( ).x + ImGui::GetWindowWidth( ) / 2 + 31 / 2, ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) / 2 + 14 / 2 - 2 }, { 0, 0 }, { 1, 1 }, ImColor( 1.f, 1.f, 1.f, GImGui->Style.Alpha ) );
                }
                ImGui::EndChild( );

                ImGui::SetCursorPosY( 42 );
                ImGui::BeginChild( "tabs", { ImGui::GetWindowWidth( ), ImGui::GetWindowHeight( ) - 42 }, 0, ImGuiWindowFlags_NoBackground ); {
                    imgui_blur::create_blur( ImGui::GetWindowDrawList( ), ImGui::GetWindowPos( ), ImGui::GetWindowPos( ) + ImGui::GetWindowSize( ), ImColor( 1.f, 1.f, 1.f, GImGui->Style.Alpha ), GImGui->Style.WindowRounding, ImDrawFlags_RoundCornersBottom );
                    ImGui::GetWindowDrawList( )->AddRectFilled( ImGui::GetWindowPos( ), ImGui::GetWindowPos( ) + ImGui::GetWindowSize( ), ImGui::GetColorU32( ImGuiCol_WindowBg, 0.875f ), GImGui->Style.WindowRounding, ImDrawFlags_RoundCornersBottom );

                    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, { 1, 1 } );
                    ImGui::BeginGroup( ); {
                        for ( int i = 0; i < ui::tabs.size( ); ++i ) {
                            ui::tab( i );
                        }
                    }
                    ImGui::EndGroup( );
                    ImGui::PopStyleVar( );
                }
                ImGui::EndChild( );

                ImGui::SetCursorPos({ 0, ImGui::GetWindowHeight( ) - 42 });
                ImGui::BeginChild( "profile", { ImGui::GetWindowWidth( ), 42 }, 0, ImGuiWindowFlags_NoBackground ); {
                    ImGui::GetWindowDrawList( )->AddRectFilled( { ImGui::GetWindowPos( ).x, ImGui::GetWindowPos( ).y + 1 }, ImGui::GetWindowPos( ) + ImGui::GetWindowSize( ), ImGui::GetColorU32( ImGuiCol_Header ), GImGui->Style.WindowRounding, ImDrawFlags_RoundCornersBottom );
                    ImGui::GetWindowDrawList( )->AddLine( ImGui::GetWindowPos( ), { ImGui::GetWindowPos( ).x + ImGui::GetWindowWidth( ), ImGui::GetWindowPos( ).y }, ImGui::GetColorU32( ImGuiCol_BorderShadow ) );
                    ImGui::GetWindowDrawList( )->AddImageRounded( avatar, { ImGui::GetWindowPos( ).x + 15, ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) / 2 - 24 / 2 }, { ImGui::GetWindowPos( ).x + 39, ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) / 2 + 24 / 2 }, { 0, 0 }, { 1, 1 }, ImColor( 1.f, 1.f, 1.f, GImGui->Style.Alpha ), 100 );
                    ImGui::GetWindowDrawList( )->AddText( font( 2 ), font( 2 )->FontSize, { ImGui::GetWindowPos( ).x + 47, ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) / 2 - font( 2 )->CalcTextSizeA( font( 2 )->FontSize, FLT_MAX, 0.f, "evrope" ).y / 2 }, ImGui::GetColorU32( ImGuiCol_Text ), "evrope" );
                    ImGui::GetWindowDrawList( )->AddText( font( 2 ), font( 2 )->FontSize, { ImGui::GetWindowPos( ).x + ImGui::GetWindowWidth( ) - font( 2 )->CalcTextSizeA( font( 2 )->FontSize, FLT_MAX, 0.f, "28d" ).x - 15, ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) / 2 - font( 2 )->CalcTextSizeA( font( 2 )->FontSize, FLT_MAX, 0.f, "28d" ).y / 2 }, ImGui::GetColorU32( ImGuiCol_TextDisabled ), "28d" );
                }
                ImGui::EndChild( );
            }
            ImGui::EndChild( );

            ImGui::SetCursorPos({ 185, 0 });
            ImGui::BeginChild( "main", ImGui::GetContentRegionAvail( ), 0, ImGuiWindowFlags_NoBackground ); {
                ImGui::BeginChild( "main header", { ImGui::GetWindowWidth( ), 42 }, 0, ImGuiWindowFlags_NoBackground ); {
                    ImGui::GetWindowDrawList( )->AddRectFilled( ImGui::GetWindowPos( ), { ImGui::GetWindowPos( ).x + ImGui::GetWindowWidth( ), ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) - 2 }, ImGui::GetColorU32( ImGuiCol_ChildBg ), GImGui->Style.WindowRounding, ImDrawFlags_RoundCornersTop );
                    ImGui::GetWindowDrawList( )->AddRectFilled( { ImGui::GetWindowPos( ).x, ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) - 2 }, { ImGui::GetWindowPos( ).x + ImGui::GetWindowWidth( ), ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) }, ImGui::GetColorU32( ImGuiCol_Scheme ) );
                    ImGui::GetWindowDrawList( )->AddText( { ImGui::GetWindowPos( ).x + 15, ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) / 2 - ImGui::CalcTextSize( ui::tabs[ui::cur_tab].m_icon, 0, 1 ).y / 2 }, ImGui::GetColorU32( ImGuiCol_Text, ui::anim ), ui::tabs[ui::cur_tab].m_icon, ImGui::FindRenderedTextEnd( ui::tabs[ui::cur_tab].m_icon ) );
                    ImGui::GetWindowDrawList( )->AddText( font( 2 ), font( 2 )->FontSize, { ImGui::GetWindowPos( ).x + 40, ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) / 2 - font( 2 )->CalcTextSizeA( font( 2 )->FontSize, FLT_MAX, 0.f, ui::tabs[ui::cur_tab].m_name, FindRenderedTextEnd( ui::tabs[ui::cur_tab].m_name ) ).y / 2 }, ImGui::GetColorU32( ui::tabs[ui::cur_tab].m_subtabs.size( ) > 0 ? ImGuiCol_TextDisabled : ImGuiCol_Text, ui::anim ), ui::tabs[ui::cur_tab].m_name, ImGui::FindRenderedTextEnd( ui::tabs[ui::cur_tab].m_name ) );
                    if ( ui::tabs[ui::cur_tab].m_subtabs.size( ) > 0 ) {
                        ImGui::RenderArrow( ImGui::GetWindowDrawList( ), { ImGui::GetWindowPos( ).x + 47 + font( 2 )->CalcTextSizeA( font( 2 )->FontSize, FLT_MAX, 0.f, ui::tabs[ui::cur_tab].m_name, FindRenderedTextEnd( ui::tabs[ui::cur_tab].m_name ) ).x, ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) / 2 - 10 }, ImGui::GetColorU32( ImGuiCol_Text, ui::anim * ui::content_anim ), ImGuiDir_Right, 12 );
                        ImGui::GetWindowDrawList( )->AddText( font( 2 ), font( 2 )->FontSize, { ImGui::GetWindowPos( ).x + 60 + font( 2 )->CalcTextSizeA( font( 2 )->FontSize, FLT_MAX, 0.f, ui::tabs[ui::cur_tab].m_name, FindRenderedTextEnd( ui::tabs[ui::cur_tab].m_name ) ).x, ImGui::GetWindowPos( ).y + ImGui::GetWindowHeight( ) / 2 - font( 2 )->CalcTextSizeA( font( 2 )->FontSize, FLT_MAX, 0.f, ui::tabs[ui::cur_tab].m_subtabs[ui::tabs[ui::cur_tab].cur_subtab], FindRenderedTextEnd( ui::tabs[ui::cur_tab].m_subtabs[ui::tabs[ui::cur_tab].cur_subtab] ) ).y / 2 }, ImGui::GetColorU32( ImGuiCol_Scheme, ui::anim * ui::content_anim ), ui::tabs[ui::cur_tab].m_subtabs[ui::tabs[ui::cur_tab].cur_subtab], ImGui::FindRenderedTextEnd( ui::tabs[ui::cur_tab].m_subtabs[ui::tabs[ui::cur_tab].cur_subtab] ) );
                    }
                }
                ImGui::EndChild( );

                ImGui::GetWindowDrawList( )->AddRectFilled( { ImGui::GetWindowPos( ).x, ImGui::GetWindowPos( ).y + 42 }, ImGui::GetWindowPos( ) + ImGui::GetWindowSize( ), ImGui::GetColorU32( ImGuiCol_WindowBg ), GImGui->Style.WindowRounding, ImDrawFlags_RoundCornersBottom );

                ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, { 10, 10 } );
                ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 15, 15 } );
                ImGui::PushStyleVar( ImGuiStyleVar_Alpha, ui::anim * ui::content_anim );
                ImGui::SetCursorPosY( 42 );
                ImGui::BeginChild( "content", ImGui::GetContentRegionAvail( ), 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysUseWindowPadding ); {
                    ui::render_page( );
                }
                ImGui::EndChild( );
                ImGui::PopStyleVar( 3 );
            }
            ImGui::EndChild( );
        }
        ImGui::End( );

        ui::handle_anims( );
    }
};

class c_frame {
public:
    c_frame( ) {
        ImGui_ImplDX9_NewFrame( );
        ImGui_ImplWin32_NewFrame( );
        ImGui::NewFrame( );
        imgui_blur::new_frame( );
    }

    ~c_frame( ) {
        ImGui::EndFrame( );
        g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA( 90, 90, 90, 255 );
        g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.f, 0 );
        if ( g_pd3dDevice->BeginScene( ) >= 0 ) {
            ImGui::Render( );
            ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
            g_pd3dDevice->EndScene( );
        }
    }
};

class c_context {
public:
    c_context( HWND hwnd ) {
        ImGui::CreateContext( );
        ImGui_ImplWin32_Init( hwnd );
        ImGui_ImplDX9_Init( g_pd3dDevice );
    }

    ~c_context( ) {
        ImGui_ImplDX9_Shutdown( );
        ImGui_ImplWin32_Shutdown( );
        ImGui::DestroyContext( );
    }
};

bool CreateDeviceD3D( HWND hWnd ) {
    if ( ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) == NULL )
        return false;

    ZeroMemory( &g_d3dpp, sizeof( g_d3dpp ) );
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if ( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice ) < 0 )
        return false;

    return true;
}

void CleanupDeviceD3D( ) {
    if ( g_pd3dDevice ) { g_pd3dDevice->Release( ); g_pd3dDevice = NULL; }
    if ( g_pD3D ) { g_pD3D->Release( ); g_pD3D = NULL; }
}

void ResetDevice( ) {
    imgui_blur::on_device_reset( );
    ImGui_ImplDX9_InvalidateDeviceObjects( );
    HRESULT hr = g_pd3dDevice->Reset( &g_d3dpp );
    if ( hr == D3DERR_INVALIDCALL )
        IM_ASSERT( 0 );
    ImGui_ImplDX9_CreateDeviceObjects( );
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
    if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) )
        return true;

    switch ( msg )
    {
    case WM_SIZE:
        if ( wParam == SIZE_MINIMIZED )
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage( 0 );
        return 0;
    }
    return ::DefWindowProc( hWnd, msg, wParam, lParam );
}