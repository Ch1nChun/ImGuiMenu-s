#include "main.hpp"
c_main gui;

int main( int, char** ) {
    WNDCLASSEXW wc = { sizeof( wc ), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle( NULL ), NULL, NULL, NULL, NULL, L"DirectX9", NULL };
    ::RegisterClassExW( &wc );
    HWND hwnd = ::CreateWindowW( wc.lpszClassName, L"DirectX9 Window", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL );

    IMGUI_CHECKVERSION( );

    CreateDeviceD3D( hwnd );
    {
        ::c_context context( hwnd );

        ImGuiIO& io = ImGui::GetIO( ); io.IniFilename = nullptr;
        gui.initialize( io, g_pd3dDevice );

        ::ShowWindow( hwnd, SW_SHOWDEFAULT );
        ::UpdateWindow( hwnd );

        bool done = false;
        while ( !done ) {
            MSG msg;
            while ( ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {
                ::TranslateMessage( &msg );
                ::DispatchMessage( &msg );
                if ( msg.message == WM_QUIT )
                    done = true;
            }
            if ( done )
                break;

            if ( g_DeviceLost ) {
                HRESULT hr = g_pd3dDevice->TestCooperativeLevel( );
                if ( hr == D3DERR_DEVICELOST ) {
                    ::Sleep( 10 );
                    continue;
                }
                if ( hr == D3DERR_DEVICENOTRESET )
                    ResetDevice( );
                g_DeviceLost = false;
            }

            if ( g_ResizeWidth != 0 && g_ResizeHeight != 0 ) {
                g_d3dpp.BackBufferWidth = g_ResizeWidth;
                g_d3dpp.BackBufferHeight = g_ResizeHeight;
                g_ResizeWidth = g_ResizeHeight = 0;
                ResetDevice( );
            }

            {
                ::c_frame frame{};

                gui.render( );
            }
            HRESULT result = g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
            if ( result == D3DERR_DEVICELOST )
                g_DeviceLost = true;
        }
    }
    CleanupDeviceD3D( );

    ::DestroyWindow( hwnd );
    ::UnregisterClassW( wc.lpszClassName, wc.hInstance );
}