#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui_freetype.h"
#include "imgui_internal.h"
#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")
#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include "imgui_settings.h"

#define ICON_WARNING "0"

#define RD_TOP ImDrawFlags_RoundCornersTop
#define RD_LEFT ImDrawFlags_RoundCornersLeft
#define RD_RIGHT ImDrawFlags_RoundCornersRight
#define RD_DOWN ImDrawFlags_RoundCornersBottom

#define RD_TOP_RIGHT ImDrawFlags_RoundCornersTopRight
#define RD_TOP_LEFT ImDrawFlags_RoundCornersTopLeft
#define RD_DOWN_RIGHT ImDrawFlags_RoundCornersBottomRight
#define RD_DOWN_LEFT ImDrawFlags_RoundCornersBottomLeft

#define MARKUP GetContentRegionMax().x - ImGui::GetStyle().WindowPadding.x
#define centerT ImVec2(0.5, 0.55)

inline DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

using namespace std;
using namespace ImGui;

namespace edited
{

    bool           BeginChild(ImGuiID id, const ImVec2& size = ImVec2(0, 0), ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0);
    bool           BeginChild(const char* str_id, const ImVec2& size = ImVec2(0, 0), ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0);
    void           EndChild();
    bool           BeginChildEx(const char* name, ImGuiID id, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags);

    bool           CheckboxWithPopup(bool popup_box, const char* label, const char* hint, bool* v, int* key, int* mode, bool* value, bool keybind, bool picker, float color[4], ImGuiColorEditFlags flags = 0);
    bool           button(bool select, const char* label, const ImVec2& size_arg);
    bool           Keybind(const char* label, int* key, int* mode, bool* show_in);

    bool           ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0);
    bool           ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags = 0, const float* ref_col = NULL);
    bool           ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags = 0, const ImVec2& size = ImVec2(0, 0));

    bool           SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool           SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool           SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);

    bool		   BeginCombo(const char* label, const char* preview_value, int val = 0, bool multi = false, ImGuiComboFlags flags = 0);
    void		   EndCombo();
    void		   MultiCombo(const char* label, bool variable[], const char* labels[], int count);
    bool		   Combo(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items = -1);
    bool		   Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
    bool		   Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);

    bool		   Selectable(const char* label, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
    bool		   Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));

    void           RenderTextColor(ImFont* font, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, const char* text, const ImVec2& align);
    void           TextColoredWrapped(const char* label, ImU32 color, float wrap_size);

    bool           BeginPopupW(const char* id, const char* hint);
    void           EndPopupW();

    bool           Pages(bool selected, const char* label, ImTextureID image_icon, const ImVec2& size_arg);
    bool           info_box(const char* icon, const char* label, ImVec2 size);

    bool           ICObutton(ImFont* icon_font, const char* icon, const ImVec2& size_arg);

    void           esp_preview(ImTextureID player_preview, bool* nickname, float nick_color[4], bool* weapon, float weapon_color[4], int* hp, float hp_color[4], bool* zoom, float zoom_color[4], bool* bomb, float bomb_color[4], bool* c4, float c4_color[4], bool* money, float money_color[4], bool* hit, float hit_color[4], bool* esp_box, float box_color[4], bool* hp_line, float hp_line_color[4]);
}