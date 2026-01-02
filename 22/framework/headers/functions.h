#pragma once
#include "includes.h"
#include <unordered_map>
#include "widgets.h"
#include "../headers/functions.h"

using namespace ImGui;

#define s_(...) scale_impl(__VA_ARGS__, var->gui.dpi)

inline ImVec2 scale_impl(const ImVec2& vec, float dpi) {
    return ImVec2(roundf(vec.x * dpi), roundf(vec.y * dpi));
}

inline ImVec2 scale_impl(float x, float y, float dpi) {
    return ImVec2(roundf(x * dpi), roundf(y * dpi));
}

inline float scale_impl(float var, float dpi) {
    return roundf(var * dpi);
}

enum positions
{
    pos_all,
    pos_x,
    pos_y
};

enum easing_type
{
    static_easing,
    dynamic_easing
};

class c_gui
{
public:
    std::unordered_map<ImGuiID, void*> m_anim_states;

    template <typename T>
    T* anim_container(ImGuiID id)
    {
        auto it = m_anim_states.find(id);
        if (it != m_anim_states.end())
            return static_cast<T*>(it->second);

        T* new_state = new T();
        m_anim_states[id] = new_state;
        return new_state;
    }

    float fixed_speed(float speed) { return speed / ImGui::GetIO().Framerate; }

    template<typename T>
    T easing(T& value, T val, float speed, int type, bool dynamic_round = false)
    {
        if (type == static_easing)
        {
            if constexpr (std::is_same<T, ImVec4>::value)
            {
                return { 1.f, 1.f, 1.f, 1.f };
            }
            else
            {
                T step = fixed_speed(speed);

                if (value < val)
                {
                    value += step;
                    if (value > val) value = val;
                }
                else if (value > val)
                {
                    value -= step;
                    if (value < val) value = val;
                }
            }
        }
        else if (type == dynamic_easing)
        {
            if constexpr (std::is_same<T, ImVec4>::value)
            {
                value = ImLerp(value, val, fixed_speed(speed));
            }
            else
                value = ImLerp(value, val + (dynamic_round ? 0.5f : 0.f), fixed_speed(speed));
        }

        return value;
    }

    bool begin(std::string_view name, bool* p_open = nullptr, window_flags flags = window_flags_none);

    void end();

    void push_color(style_col idx, ImU32 col);

    void pop_color(int count = 1);

    void push_var(style_var idx, float val);

    void push_var(style_var idx, const ImVec2& val);

    void pop_var(int count = 1);

    void push_font(ImFont* font);

    void pop_font();

    void set_pos(const ImVec2& pos, int type);

    void set_pos(float pos, int type);

    ImVec2 get_pos();

    void set_screen_pos(const ImVec2& pos, int type);

    void set_screen_pos(float pos, int type);

    ImVec2 get_screen_pos();

    void begin_group();

    void end_group();

    void begin_content(std::string_view id, const ImVec2& size, const ImVec2& padding = ImVec2(0, 0), const ImVec2& spacing = ImVec2(0, 0), window_flags window_flags__ = 0, child_flags child_flags__ = 0);

    void end_content();

    void sameline(float offset_from_start_x = 0.f, float spacing_w = -1.f);

    void dummy(const ImVec2& size);

    bool begin_def_child(std::string_view name, const ImVec2& size_arg = ImVec2(0, 0), child_flags child_flags = 0, window_flags window_flags = 0);

    void end_def_child();

    void set_next_window_pos(const ImVec2& pos, gui_cond cond = 0, const ImVec2& pivot = ImVec2(0, 0));

    void set_next_window_size(const ImVec2& size, gui_cond cond = 0);

    ImVec2 text_size(ImFont* font, const char* text, const char* text_end = nullptr, bool hide_text_after_double_hash = false, float wrap_width = -1.f);

    ImVec2 window_size();

    float window_width();

    float window_height();

    ImDrawList* window_drawlist();

    ImDrawList* foreground_drawlist();

    ImDrawList* background_drawlist();

    ImVec2 window_pos();

    ImGuiWindow* get_window();

    void push_id(const char* str_id);

    void push_id(const char* str_id_begin, const char* str_id_end);

    void push_id(const void* ptr_id);

    void push_id(int int_id);

    void pop_id();
    
    ImGuiID get_id(const char* str, const char* str_end);

    ImGuiID get_id(const void* ptr);

    ImGuiID get_id(int n);

    ImVec2 content_avail();

    ImVec2 content_max();

    void item_size(const ImVec2& size, float text_baseline_y = -1.f);

    void item_size(const ImRect& bb, float text_baseline_y = -1.f);

    bool item_add(const ImRect& bb, ImGuiID id, const ImRect* nav_bb_arg = NULL, ImGuiItemFlags extra_flags = 0);

    bool is_window_hovered(ImGuiHoveredFlags flags);

    bool is_window_focused(ImGuiFocusedFlags flags);

    void set_window_focus();

    void set_window_focus(const char* name);

    bool is_rect_visible(const ImVec2& size);

    bool is_rect_visible(const ImRect& rect);

    ImVec2 adjust_window_pos(const ImVec2& rect, const ImVec2& window_size);

    bool button_behavior(const ImRect& bb, ImGuiID id, bool* out_hovered, bool* out_held, ImGuiButtonFlags flags = 0);

    bool invisible_button(const char* str_id, const ImVec2& size_arg, mouse_button flags = 0);

    ImVec4 u32_to_float4(ImU32 in);

    ImU32 float4_to_u32(const ImVec4& in);

    void rgb_to_hsv(float r, float g, float b, float& out_h, float& out_s, float& out_v);

    void hsv_to_rgb(float h, float s, float v, float& out_r, float& out_g, float& out_b);

    bool item_hoverable(const ImRect& bb, ImGuiID id, ImGuiItemFlags item_flags = 0);

    bool is_item_hovered(ImGuiHoveredFlags flags);

    bool is_item_active();

    bool is_item_clicked(mouse_button mouse_button);

    bool mouse_down(mouse_button button);

    bool mouse_clicked(mouse_button button, bool repeat = false);

    bool mouse_released(mouse_button button);

    bool mouse_double_clicked(mouse_button button);
    
    const char* text_end(const char* text);

    template<typename T>
    const char* get_fmt(char* text, T* value, std::string_view fmt)
    {
        ImFormatString(text, IM_ARRAYSIZE(text), fmt.data(), *value);
        return text + strlen(text);
    }

    bool is_window_cond(ImGuiWindow* window, const std::vector<std::string>& names);

    ImVec2 mouse_pos();
    
    void set_style();

    void draw_decorations();

    void initialize();

    void render();

    ImRect get_item_rect();
};

inline std::unique_ptr<c_gui> gui = std::make_unique<c_gui>();