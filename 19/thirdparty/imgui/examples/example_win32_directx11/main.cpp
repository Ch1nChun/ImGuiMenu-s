
#include "../../framework/settings/functions.h"
#include "../../framework/data/font.h"
#include "../../framework/data/texture.h"
#include "../../framework/data/imgui_freetype.h"

#pragma comment(lib, "d3dx11.lib")

#include <d3d11.h>
#include <tchar.h>
#include <d3dx11.h>

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main(int, char**)
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"DX11", WS_POPUP, 0, 0, 1920, 1080, nullptr, nullptr, wc.hInstance, nullptr);

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
    cfg.FontDataOwnedByAtlas = false;
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;

    {
        set->c_font.inter_medium[0] = io.Fonts->AddFontFromMemoryTTF(inter_medium, sizeof(inter_medium), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        set->c_font.inter_medium[1] = io.Fonts->AddFontFromMemoryTTF(inter_medium, sizeof(inter_medium), 16.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

        set->c_font.icon[0] = io.Fonts->AddFontFromMemoryTTF(icon, sizeof(icon), 14.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        set->c_font.icon[1] = io.Fonts->AddFontFromMemoryTTF(icon, sizeof(icon), 16.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        set->c_font.icon[2] = io.Fonts->AddFontFromMemoryTTF(icon, sizeof(icon), 40.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        set->c_font.icon[3] = io.Fonts->AddFontFromMemoryTTF(icon2, sizeof(icon2), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        set->c_font.icon[4] = io.Fonts->AddFontFromMemoryTTF(icon2, sizeof(icon2), 9.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        set->c_font.icon[5] = io.Fonts->AddFontFromMemoryTTF(icon2, sizeof(icon2), 76.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        set->c_font.icon[6] = io.Fonts->AddFontFromMemoryTTF(icon2, sizeof(icon2), 96.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

        set->c_font.name = io.Fonts->AddFontFromMemoryTTF(inter_medium, sizeof(inter_medium), 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    }

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    {
        if (set->c_texture.bg == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, background, sizeof(background), &info, pump, &set->c_texture.bg, 0);
        if (set->c_texture.logo == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, logo, sizeof(logo), &info, pump, &set->c_texture.logo, 0);
    }

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

        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();

        if (var->c_dpi.dpi_changed)
        {
            var->c_dpi.dpi = var->c_dpi.dpi_saved / 100.f;

            io.Fonts->Clear();

            {
                set->c_font.inter_medium[0] = io.Fonts->AddFontFromMemoryTTF(inter_medium, sizeof(inter_medium), 15.f * var->c_dpi.dpi, &cfg, io.Fonts->GetGlyphRangesCyrillic());
                set->c_font.inter_medium[1] = io.Fonts->AddFontFromMemoryTTF(inter_medium, sizeof(inter_medium), 16.f * var->c_dpi.dpi, &cfg, io.Fonts->GetGlyphRangesCyrillic());

                set->c_font.icon[0] = io.Fonts->AddFontFromMemoryTTF(icon, sizeof(icon), 14.f * var->c_dpi.dpi, &cfg, io.Fonts->GetGlyphRangesCyrillic());
                set->c_font.icon[1] = io.Fonts->AddFontFromMemoryTTF(icon, sizeof(icon), 16.f * var->c_dpi.dpi, &cfg, io.Fonts->GetGlyphRangesCyrillic());
                set->c_font.icon[2] = io.Fonts->AddFontFromMemoryTTF(icon, sizeof(icon), 40.f * var->c_dpi.dpi, &cfg, io.Fonts->GetGlyphRangesCyrillic());
                set->c_font.icon[3] = io.Fonts->AddFontFromMemoryTTF(icon2, sizeof(icon2), 15.f * var->c_dpi.dpi, &cfg, io.Fonts->GetGlyphRangesCyrillic());
                set->c_font.icon[4] = io.Fonts->AddFontFromMemoryTTF(icon2, sizeof(icon2), 9.f * var->c_dpi.dpi, &cfg, io.Fonts->GetGlyphRangesCyrillic());
                set->c_font.icon[5] = io.Fonts->AddFontFromMemoryTTF(icon2, sizeof(icon2), 76.f * var->c_dpi.dpi, &cfg, io.Fonts->GetGlyphRangesCyrillic());
                set->c_font.icon[6] = io.Fonts->AddFontFromMemoryTTF(icon2, sizeof(icon2), 96.f * var->c_dpi.dpi, &cfg, io.Fonts->GetGlyphRangesCyrillic());

                set->c_font.name = io.Fonts->AddFontFromMemoryTTF(inter_medium, sizeof(inter_medium), 18.f * var->c_dpi.dpi, &cfg, io.Fonts->GetGlyphRangesCyrillic());

                var->c_dpi.dpi_changed = false;
            }
            io.Fonts->Build();

            ImGui_ImplDX11_CreateDeviceObjects();
        }


        {
            gui->render();
        }

        const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 1.f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        HRESULT hr = g_pSwapChain->Present(1, 0);

        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
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
    if (res != S_OK)
        return false;

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
