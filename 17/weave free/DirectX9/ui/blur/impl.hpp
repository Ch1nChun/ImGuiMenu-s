#pragma once

#include <d3d9.h>
#include <wrl/client.h>

#include "../../imgui/imgui_internal.h"
#include "x.hpp"
#include "y.hpp"

namespace imgui_blur
{
    void set_device( IDirect3DDevice9* device ) noexcept;
    void clear_blur_textures( ) noexcept;
    void on_device_reset( ) noexcept;
    void new_frame( ) noexcept;
    void create_blur( ImDrawList* d, ImVec2 min, ImVec2 max, ImColor col = ImColor( 255, 255, 255 ), float rounding = 0.f, ImDrawFlags round_flags = ImDrawFlags_RoundCornersAll ) noexcept;
}