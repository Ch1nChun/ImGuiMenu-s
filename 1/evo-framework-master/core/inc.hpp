#pragma once
#pragma warning ( disable : 4005 )
#pragma warning ( disable : 26495 )
#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4715 )
#pragma warning ( disable : 4018 )
#pragma warning ( disable : 4244 )

#include <windows.h>
#include <windowsx.h>
#include <memory>
#include <sstream>
#include <iostream>
#include <functional>
#include <WinUser.h>
#include <string_view>
#include <map>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <deque>
#include <mutex>
#include <optional>
#include <filesystem>
#include <fstream>
#include <random>
#include <array>
#include <mutex>

/* directx includes */
#include <d3d9.h>
#include <d3dx9.h>

/* libs for directx */
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "freetype.lib")

#include "thirdparty/imgui/imgui.h"
#include "thirdparty/imgui/imgui_freetype.h"
#include "thirdparty/imgui/imgui_impl_win32.h"
#include "thirdparty/imgui/imgui_impl_dx9.h"

#define draw_list ImGui::GetBackgroundDrawList( )

/* inc */
#include "evo-sdk/input_system/input_system.hpp"
#include "evo-sdk/sdk/sdk.hpp"
#include "evo-sdk/render/render_system.hpp"
#include "evo-sdk/animation_system/animation_system.hpp"
#include "evo-sdk/input_system/input_system.hpp"

#include "ui/container/container.hpp"
#include "ui/menu/menu.hpp"

#include "ui/controls/checkbox/checkbox.hpp"
#include "ui/controls/slider/slider.hpp"
#include "ui/controls/combo/combo.hpp"
#include "ui/controls/multicombo/multicombo.hpp"
#include "ui/controls/colorpicker/colorpicker.hpp"
#include "ui/controls/keybind/keybind.hpp"
#include "ui/controls/text/text.hpp"
#include "ui/controls/textinput/textinput.hpp"
#include "ui/controls/listbox/listbox.hpp"
#include "ui/controls/button/button.hpp"
#include "ui/controls/esp_preview/esp_preview.hpp"
#include "ui/controls/popup/popup.hpp"

#include "ui/controls/child/child.hpp"
#include "ui/controls/window/window.hpp"


/* resources */
#define IDS_APP_TITLE 103
#define IDR_MAINFRAME 128
#define IDD_GUITEST_DIALOG 102
#define IDD_ABOUTBOX 103
#define IDM_ABOUT 104
#define IDM_EXIT 105
#define IDI_GUITEST 107
#define IDI_SMALL 108
#define IDC_GUITEST 109
#define IDC_MYICON 2
#ifndef IDC_STATIC
#define IDC_STATIC -1
#endif