// Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>

#include "font.hpp"
#include "custom.hpp"

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
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
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX9 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

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
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    io.Fonts->AddFontFromMemoryTTF( ( void* )font_binary, sizeof( font_binary ), 13.f );
    io.Fonts->AddFontFromMemoryTTF( ( void* )icons_binary, sizeof( icons_binary ), 15.f );
    io.Fonts->AddFontFromMemoryTTF( ( void* )font_bold_binary, sizeof( font_bold_binary ), 20.f );

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(72 / 255.f, 79 / 255.f, 81 / 255.f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
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

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 0, 0 } );
        ImGui::Begin( "menu", 0, ImGuiWindowFlags_NoDecoration );
        {
            ImGui::PopStyleVar( );

            ImGuiWindow* window = ImGui::GetCurrentWindow( );

            ImDrawList* draw = ImGui::GetWindowDrawList( );
            ImVec2 p = ImGui::GetWindowPos( );

            ImGui::SetWindowSize( { 650, 450 } );

            static bool navbar_collapsed = true;
            static float navbar_width = 0.f; navbar_width = ImLerp( navbar_width, navbar_collapsed ? 0.f : 1.f, 0.04f );
            content_anim = ImLerp( content_anim, 1.f, 0.04f );

            ImGui::PushItemFlag( ImGuiItemFlags_Disabled, !navbar_collapsed );
            ImGui::PushStyleVar( ImGuiStyleVar_Alpha, content_anim * ( navbar_width > 0.005f ? 1 / navbar_width / 2 : 1.f ) );
            ImGui::SetCursorPos( { 81, 25 } );
            ImGui::BeginGroup( );
            ImGui::PushFont( ImGui::GetIO( ).Fonts->Fonts[2] );
            ImGui::Text( "Ragebot" );
            ImGui::PopFont( );

            ImGui::BeginChild( "main", { window->Size.x - 81, window->Size.y - window->DC.CursorPos.y + window->Pos.y }, 0, ImGuiWindowFlags_NoBackground );
            {
                switch ( tab ) {
                case 0: {
                    ImGui::BeginChild( "subtabs", { ImGui::GetWindowWidth( ) - 30, 40 }, 1 );
                    {
                        ImGui::SetCursorPos( { 16, 0 } );

                        if ( custom::subtab( "General##subtab", subtab == 0 ) ) subtab = 0; ImGui::SameLine( 0, 20 );
                        if ( custom::subtab( "Accuracy##subtab", subtab == 1 ) ) subtab = 1; ImGui::SameLine( 0, 20 );
                        if ( custom::subtab( "Exploits##subtab", subtab == 2 ) ) subtab = 2;
                    }
                    ImGui::EndChild( );

                    ImGui::BeginChild( "main_child", { ImGui::GetWindowWidth( ), ImGui::GetWindowHeight( ) - 55 }, 0, ImGuiWindowFlags_NoBackground );
                    {
                        ImGui::BeginGroup( );
                        {
                            //1
                            custom::begin_child( "General", { ( ImGui::GetWindowWidth( ) - 30 ) / 2 - ImGui::GetStyle( ).ItemSpacing.x / 2, 0 } );
                            {
                                static bool enabled = true;
                                static int slider = 0, combo = 0;
                                static char text[64];
                                static float col[4];
                                static float floatValue = 0.5f;

                                ImGui::Checkbox( "Enabled", &enabled );
                                ImGui::SliderInt("Float Slider", &slider, 0, 100);
                                ImGui::SliderFloat("Float Slider", &floatValue, 0.0f, 1.0f);
                               
                            }
                            custom::end_child( );

                            //3
                            custom::begin_child( "Window 3", { ( ImGui::GetWindowWidth( ) - 30 ) / 2 - ImGui::GetStyle( ).ItemSpacing.x / 2, 0 } );
                            {
                                static bool enabled = true;
                                static int selectedOption = 0;
                                static int slider = 0, combo = 0;
                                static char text[64];
                                static float col[4];

                                ImGui::RadioButton("Option 1", &selectedOption, 0);
                                ImGui::RadioButton("Option 2", &selectedOption, 1);

                               
                            }
                            custom::end_child( );
                        }
                        ImGui::EndGroup( );
                        ImGui::SameLine( 0, 15 );
                        ImGui::BeginGroup( );
                        {
                            //2
                            custom::begin_child( "Window 2", { ( ImGui::GetWindowWidth( ) - 30 ) / 2 - ImGui::GetStyle( ).ItemSpacing.x / 2, 0 } );
                            {
                                static bool enabled = true;
                                static int slider = 0, combo = 0;
                                static char text[64];
                                static float col[4];
                                static const char* items[] = { "Option 1", "Option 2", "Option 3" };
                                static int currentItem = 0;
                                static float color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
                                ImGui::Combo("Combo Box", &currentItem, items, IM_ARRAYSIZE(items));
                                
                                ImGui::ColorEdit4("Color Picker", color, ImGuiWindowFlags_NoDecoration);
                              
                            }
                            custom::end_child( );

                            //4
                            custom::begin_child( "Window 4", { ( ImGui::GetWindowWidth( ) - 30 ) / 2 - ImGui::GetStyle( ).ItemSpacing.x / 2, 0 } );
                            {
                                static bool enabled = true;
                                static int slider = 0, combo = 0;
                                static char text[64];
                                static float col[4];

                                ImGui::Button("Button");
                            
                            }
                            custom::end_child( );
                        }
                        ImGui::EndGroup( );
                        ImGui::Spacing( );
                    }
                    ImGui::EndChild( );
                } break;
                }
            }
            ImGui::EndChild( );

            ImGui::EndGroup( );
            ImGui::PopStyleVar( );
            ImGui::PopItemFlag( );

            ImGui::SetCursorPos( { 0, 0 } );

            ImGui::BeginChild( "navbar", { 50 + 100 * navbar_width, window->Size.y }, 0, ImGuiWindowFlags_NoBackground );
            {
                ImGui::GetWindowDrawList( )->AddRectFilled( p, p + ImGui::GetWindowSize( ), ImGui::GetColorU32( ImGuiCol_ChildBg ), ImGui::GetStyle( ).WindowRounding, ImDrawFlags_RoundCornersLeft );
                ImGui::GetWindowDrawList( )->AddRectFilled( { p.x + ImGui::GetWindowWidth( ) - 1, p.y }, p + ImGui::GetWindowSize( ), ImGui::GetColorU32( ImGuiCol_Border ) );

                ImGui::SetCursorPosY( 87 );

                ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, { 0, 16 } );
                if ( custom::tab( "A", "Ragebot", tab == 0 ) ) tab = 0;
                if ( custom::tab( "C", "Anti-Aim", tab == 1 ) ) tab = 1;
                if ( custom::tab( "B", "Visuals", tab == 2 ) ) tab = 2;
                if ( custom::tab( "D", "Inventory", tab == 3 ) ) tab = 3;
                if ( custom::tab( "E", "Misc", tab == 4 ) ) tab = 4;
                ImGui::PopStyleVar( );
            }
            ImGui::EndChild( );

            ImGui::SetCursorPos( { 41.5f + 100 * navbar_width, 47 } );
            if ( custom::collapse_button( navbar_collapsed ) ) navbar_collapsed = !navbar_collapsed;
        }
        ImGui::End( );

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*clear_color.w*255.0f), (int)(clear_color.y*clear_color.w*255.0f), (int)(clear_color.z*clear_color.w*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
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

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
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
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
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
