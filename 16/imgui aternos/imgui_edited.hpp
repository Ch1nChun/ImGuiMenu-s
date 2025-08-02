#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui_internal.h>
#include "imgui_settings.h"
#include "imstb_textedit.h"

#include <cstdlib>
#include <imgui.h>

#include <map>

#include <string>
#include <wtypes.h>

namespace edited
{ 
    bool            BeginChild(ImGuiID id, const ImVec2& size = ImVec2(0, 0), ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0);
    bool            BeginChild(const char* str_id, const ImVec2& size = ImVec2(0, 0), ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0);
    void            EndChild();
    bool            BeginChildEx(const char* name, ImGuiID id, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags);
    bool            Tab(bool selected, const char* icon, const char* label, const char* description, const ImVec2& size_arg);
    bool            Checkbox(const char* label, const char* description, bool* v);

    bool            QuandScalar(const char* label, const char* description, ImGuiDataType data_type, void* p_data0, void* p_data1, const void* p_min0, const void* p_max0, const void* p_min1, const void* p_max1, const char* format0 = "%d", const char* format1 = "%d", ImGuiSliderFlags flags = 0);
    bool            QuandFloat(const char* label, const char* description, float* v0, float* v1, float v_min0 = 0.0f, float v_max0 = 0.0f, float v_min1 = 0.0f, float v_max1 = 0.0f, const char* format0 = "%d", const char* format1 = "%d", ImGuiSliderFlags flags = 0);
    bool            QuandInt(const char* label, const char* description, int* v0, int* v1, int v_min0 = 0, int v_max0 = 0, int v_min1 = 0, int v_max1 = 0, const char* format0 = "%d", const char* format1 = "%d", ImGuiSliderFlags flags = 0);

    bool            SliderInt(const char* label, const char* description, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool            SliderFloat(const char* label, const char* description, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool            SliderScalar(const char* label, const char* description, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);

     bool           ColorEdit4(const char* label, const char* description, float col[4], ImGuiColorEditFlags flags = 0);
     bool           ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags = 0, const float* ref_col = NULL);
     bool           ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags = 0, const ImVec2& size = ImVec2(0, 0));

     bool		    Selectable(const char* label, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
     bool		    Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
     bool		    BeginCombo(const char* label, const char* description, const char* preview_value, int val = 0, bool multi = false, ImGuiComboFlags flags = 0);
     void		    EndCombo();
     void		    MultiCombo(const char* label, const char* description, bool variable[], const char* labels[], int count);
     bool		    Combo(const char* label, const char* description, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items = -1);
     bool		    Combo(const char* label, const char* description, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
     bool		    Combo(const char* label, const char* description, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);

     bool           RangeSliderBehavior(const ImRect& frame_bb, ImGuiID id, float* v1, float* v2, float v_min, float v_max, float power, int decimal_precision, ImGuiSliderFlags flags = 0);
     bool           RangeSliderFloat(const char* label, const char* description, float* v1, float* v2, float v_min, float v_max, const char* display_format = "(%.3f, %.3f)", float power = 1.0f);

     void           esp_preview(ImTextureID player_preview, bool* nickname, float nick_color[4], bool* weapon, float weapon_color[4], int* hp, float hp_color[4], bool* zoom, float zoom_color[4], bool* bomb, float bomb_color[4], bool* c4, float c4_color[4], bool* money, float money_color[4], bool* hit, float hit_color[4], bool* esp_box, float box_color[4], bool* hp_line, float hp_line_color[4]);

     bool           checkboxP(const char* label, bool* v, const ImVec2& size_arg);
     bool           Keybind(const char* label, const char* description, int* key);

}