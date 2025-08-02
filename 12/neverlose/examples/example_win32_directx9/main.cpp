// Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#define  IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>

#include "gui.hpp"
#include "hashes.hpp"

#include "blur.hpp"
#include "bytes.hpp"

using namespace ImGui;

#define ALPHA    ( ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )
#define NO_ALPHA ( ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )

IDirect3DTexture9* avatar{ };
IDirect3DTexture9* bg{ };

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_OVERLAPPEDWINDOW, 0, 0, 1920, 1080, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    io.Fonts->AddFontFromMemoryTTF( museo500_binary, sizeof museo500_binary, 14 );
    static const ImWchar icon_ranges[ ] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromMemoryTTF( &font_awesome_binary, sizeof font_awesome_binary, 13, &icons_config, icon_ranges );

    io.Fonts->AddFontFromMemoryTTF( museo900_binary, sizeof museo900_binary, 28 );

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if ( !avatar )
            D3DXCreateTextureFromFileInMemoryEx( g_pd3dDevice, &esliboganet, sizeof esliboganet, 30, 30, D3DX_DEFAULT, 0,
                D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &avatar );

        if ( !bg )
            D3DXCreateTextureFromFileExA( g_pd3dDevice, "C:\\Windows\\Web\\Screen\\img104.jpg", 1920, 1080, D3DX_DEFAULT, 0,
                D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &bg );

        blur::device = g_pd3dDevice;

        static bool bools[50]{};
        static int ints[50]{}, combo = 0;
        vector < const char* > items = { "Option", "Option 1", "Option 2", "Option 3", "Option 4", "Option 5", ICON_FA_AXE_BATTLE " Option 6", "Option 7", "Option 8", "Option 9" };
        static char buf[64];

        static float color[4] = { 1.f, 1.f, 1.f, 1.f };

        PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );

        ImGui::Begin( "Hello, world!", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground ); {

            auto window = GetCurrentWindow( );
            auto draw   = window->DrawList;
            auto pos    = window->Pos;
            auto size   = window->Size;
            auto style  = GetStyle( );

            gui.m_anim = ImLerp( gui.m_anim, 1.f, 0.045f );

            SetWindowSize( ImVec2( 690, 500 ) );

            GetBackgroundDrawList( )->AddImage( bg, ImVec2( 0, 0 ), io.DisplaySize );

            draw->AddText( io.Fonts->Fonts[1], io.Fonts->Fonts[1]->FontSize, pos + ImVec2( 170 / 2 - io.Fonts->Fonts[1]->CalcTextSizeA( io.Fonts->Fonts[1]->FontSize, FLT_MAX, 0, "NEVERLOSE" ).x / 2 + 1, 20 ), gui.accent_color.to_im_color( ), "NEVERLOSE" );
            draw->AddText( io.Fonts->Fonts[1], io.Fonts->Fonts[1]->FontSize, pos + ImVec2( 170 / 2 - io.Fonts->Fonts[1]->CalcTextSizeA( io.Fonts->Fonts[1]->FontSize, FLT_MAX, 0, "NEVERLOSE" ).x / 2, 20 ), GetColorU32( ImGuiCol_Text ), "NEVERLOSE" );

            draw->AddLine( pos + ImVec2( 0, size.y - 50 ), pos + ImVec2( 170, size.y - 50 ), GetColorU32( ImGuiCol_WindowBg, 0.5f ) );
            draw->AddImageRounded( avatar, pos + ImVec2( 15, size.y - 40 ), pos + ImVec2( 45, size.y - 10 ), ImVec2( 0, 0 ), ImVec2( 1, 1 ), ImColor( 1.f, 1.f, 1.f, 1.f ), 100 );
            draw->AddText( pos + ImVec2( 50, size.y - 40 ), gui.text.to_im_color( ), "evrope" );
            draw->AddText( pos + ImVec2( 50, size.y - 25 ), gui.text_disabled.to_im_color( ), "Till:" );
            draw->AddText( pos + ImVec2( 50 + CalcTextSize( "Till: " ).x, size.y - 25 ), gui.accent_color.to_im_color( ), "Lifetime" );

            SetCursorPos( ImVec2( 10, 70 ) );
            BeginChild( "##tabs", ImVec2( 150, size.y - 120 ) );

            gui.group_title( "Aimbot" );
            if ( gui.tab( ICON_FA_CROSSHAIRS, "Ragebot", gui.m_tab == 0 ) && gui.m_tab != 0 )
                gui.m_tab = 0, gui.m_anim = 0.f;

            if ( gui.tab( ICON_FA_GHOST, "Anti-aim", gui.m_tab == 1 ) && gui.m_tab != 1 )
                gui.m_tab = 1, gui.m_anim = 0.f;

            if ( gui.tab( ICON_FA_MOUSE, "Legitbot", gui.m_tab == 2 ) && gui.m_tab != 2 )
                gui.m_tab = 2, gui.m_anim = 0.f;

            Spacing( ), Spacing( ), Spacing( );

            gui.group_title( "Visuals" );
            if ( gui.tab( ICON_FA_USER, "Players", gui.m_tab == 3 ) && gui.m_tab != 3 )
                gui.m_tab = 3, gui.m_anim = 0.f;

            if ( gui.tab( ICON_FA_PALLET, "World", gui.m_tab == 4 ) && gui.m_tab != 4 )
                gui.m_tab = 4, gui.m_anim = 0.f;

            Spacing( ), Spacing( ), Spacing( );

            gui.group_title( "Miscellaneous" );
            if ( gui.tab( ICON_FA_HAMMER, "Main", gui.m_tab == 5 ) && gui.m_tab != 5 )
                gui.m_tab = 5, gui.m_anim = 0.f;

            if ( gui.tab( ICON_FA_CODE, "Scripts", gui.m_tab == 6 ) && gui.m_tab != 6 )
                gui.m_tab = 6, gui.m_anim = 0.f;

            EndChild( );

            SetCursorPos( ImVec2( 190, 20 ) );
            Button( ICON_FA_SAVE " Save", ImVec2( 100, 25 ) );

            PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );

            SetCursorPos( ImVec2( 300, 20 ) );
            BeginChild( "##subtabs", ImVec2( 240, 25 ) );

            GetWindowDrawList( )->AddRectFilled( GetWindowPos( ), GetWindowPos( ) + GetWindowSize( ), gui.button.to_im_color( ), 4 );
            GetWindowDrawList( )->AddRect( GetWindowPos( ), GetWindowPos( ) + GetWindowSize( ), gui.border.to_im_color( ), 4 );

            for ( int i = 0; i < gui.rage_subtabs.size( ); ++i ) {
                if ( gui.subtab( gui.rage_subtabs.at( i ), gui.m_rage_subtab == i, gui.rage_subtabs.size( ), i == 0 ? ImDrawFlags_RoundCornersLeft : i == gui.rage_subtabs.size( ) ? ImDrawFlags_RoundCornersRight : 0 ) && gui.m_rage_subtab != i )
                    gui.m_rage_subtab = i, gui.m_anim = 0.f;

                if ( i != gui.rage_subtabs.size( ) - 1 )
                    SameLine( );

            }

            EndChild( );

            PopStyleVar( );

            PushStyleVar( ImGuiStyleVar_Alpha, gui.m_anim );
            PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 8, 8 ) );

            SetCursorPos( ImVec2( 185, 81 - ( 5 * gui.m_anim ) ) );
            BeginChild( "##childs", ImVec2( size.x - 200, size.y - 96 ) );

            switch ( gui.m_tab ) {

            case 0:

                gui.group_box( ICON_FA_FEATHER " Feather", ImVec2( GetWindowWidth( ) / 2 - GetStyle( ).ItemSpacing.x / 2, 210 ) ); {

                    Checkbox( "Skoro t'ma otstypit " ICON_FA_DAGGER, &bools[0] );
                    SliderInt( "Slider", &ints[0], 0, 100, "%d%%" );
                    Combo( "Combo", &combo, items.data( ), items.size( ) );
                    InputText( "Input", buf, sizeof buf );
                    ColorEdit4( "Color", color, ALPHA );
                    Button( "Button", ImVec2( GetWindowWidth( ), 25 ) );

                } gui.end_group_box( );

                SameLine( );

                gui.group_box( ICON_FA_CROWN " Crown", ImVec2( GetWindowWidth( ) / 2 - GetStyle( ).ItemSpacing.x / 2, GetWindowHeight( ) ) ); {

                    for ( int i = 1; i < 50; ++i ) {
                        Checkbox( std::to_string( i ).c_str( ), &bools[ i ] );

                        if ( i != 49 )
                            Separator( );

                    }

                } gui.end_group_box( );

                break;

            case 1:

                gui.group_box( ICON_FA_BABY " Baby", ImVec2( GetWindowWidth( ) / 2 - GetStyle( ).ItemSpacing.x / 2, GetWindowHeight( ) / 2 - GetStyle( ).ItemSpacing.y / 2 ) ); {

                } gui.end_group_box( );

                gui.group_box( ICON_FA_AD " Ad", ImVec2( GetWindowWidth( ) / 2 - GetStyle( ).ItemSpacing.x / 2, GetWindowHeight( ) / 2 - GetStyle( ).ItemSpacing.y / 2 ) ); {

                } gui.end_group_box( );

                SameLine( ), SetCursorPosY( 0 );

                gui.group_box( "Non icon name", ImVec2( GetWindowWidth( ) / 2 - GetStyle( ).ItemSpacing.x / 2, GetWindowHeight( ) ) ); {

                } gui.end_group_box( );

                break;

            case 2:

                break;
            }

            EndChild( );

            PopStyleVar( 2 );

        } ImGui::End( );

        PopStyleVar( );

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*clear_color.w*255.0f), (int)(clear_color.y*clear_color.w*255.0f), (int)(clear_color.z*clear_color.w*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
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
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
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
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
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
