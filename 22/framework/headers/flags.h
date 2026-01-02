#pragma once

typedef int style_var;
typedef int style_col;
typedef int window_flags;
typedef int child_flags;
typedef int gui_cond;
typedef int draw_flags;
typedef int mouse_button;

using c_text = std::string;
using c_vec4 = ImVec4;
using c_col = ImColor;
using c_vec2 = ImVec2;
using c_multi_string = std::vector<std::string>;
using c_multi_bool = std::vector<bool>;
using c_window = ImGuiWindow;
using c_id = ImGuiID;
using c_rect = ImRect;
using c_draw_list = ImDrawList;

enum style_var_
{
    style_var_alpha,
    style_var_disabled_alpha,
    style_var_window_padding,
    style_var_window_rounding,
    style_var_window_border_size,
    style_var_window_min_size,
    style_var_window_title_align,
    style_var_child_rounding,
    style_var_child_border_size,
    style_var_popup_rounding,
    style_var_popup_border_size,
    style_var_frame_padding,
    style_var_frame_rounding,
    style_var_frame_border_size,
    style_var_item_spacing,
    style_var_item_inner_spacing,
    style_var_indent_spacing,
    style_var_cell_padding,
    style_var_scrollbar_size,
    style_var_scrollbar_rounding,
    style_var_grab_min_size,
    style_var_grab_rounding,
    style_var_tab_rounding,
    style_var_tab_border_size,
    style_var_tab_bar_border_size,
    style_var_tab_bar_overline_size,
    style_var_table_angled_headers_angle,
    style_var_table_angled_headers_text_align,
    style_var_button_text_align,
    style_var_selectable_text_align,
    style_var_separator_text_border_size,
    style_var_separator_text_align,
    style_var_separator_text_padding,
    style_var_window_shadow_size,
    style_var_window_shadow_offset,
    style_var_scrollbar_border_padding,
    style_var_scrollbar_content_padding,
    style_var_count
};

enum style_col_
{
    style_col_text,
    style_col_text_disabled,
    style_col_window_bg,
    style_col_child_bg,
    style_col_popup_bg,
    style_col_border,
    style_col_border_shadow,
    style_col_frame_bg,
    style_col_frame_bg_hovered,
    style_col_frame_bg_active,
    style_col_title_bg,
    style_col_title_bg_active,
    style_col_title_bg_collapsed,
    style_col_menu_bar_bg,
    style_col_scrollbar_bg,
    style_col_scrollbar_grab,
    style_col_scrollbar_grab_hovered,
    style_col_scrollbar_grab_active,
    style_col_check_mark,
    style_col_slider_grab,
    style_col_slider_grab_active,
    style_col_button,
    style_col_button_hovered,
    style_col_button_active,
    style_col_header,
    style_col_header_hovered,
    style_col_header_active,
    style_col_separator,
    style_col_separator_hovered,
    style_col_separator_active,
    style_col_resize_grip,
    style_col_resize_grip_hovered,
    style_col_resize_grip_active,
    style_col_tab_hovered,
    style_col_tab,
    style_col_tab_selected,
    style_col_tab_selected_overline,
    style_col_tab_dimmed,
    style_col_tab_dimmed_selected,
    style_col_tab_dimmed_selected_overline,
    style_col_plot_lines,
    style_col_plot_lines_hovered,
    style_col_plot_histogram,
    style_col_plot_histogram_hovered,
    style_col_table_header_bg,
    style_col_table_border_strong,
    style_col_table_border_light,
    style_col_table_row_bg,
    style_col_table_row_bg_alt,
    style_col_text_link,
    style_col_text_selected_bg,
    style_col_drag_drop_target,
    style_col_nav_highlight,
    style_col_nav_windowing_highlight,
    style_col_nav_windowing_dim_bg,
    style_col_modal_window_dim_bg,
    style_col_window_shadow,
    style_col_count,

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    style_col_tab_active = style_col_tab_selected,
    style_col_tab_unfocused = style_col_tab_dimmed,
    style_col_tab_unfocused_active = style_col_tab_dimmed_selected,
#endif
};

enum window_flags_
{
    window_flags_none = 0,
    window_flags_no_title_bar = 1 << 0,
    window_flags_no_resize = 1 << 1,
    window_flags_no_move = 1 << 2,
    window_flags_no_scrollbar = 1 << 3,
    window_flags_no_scroll_with_mouse = 1 << 4,
    window_flags_no_collapse = 1 << 5,
    window_flags_always_auto_resize = 1 << 6,
    window_flags_no_background = 1 << 7,
    window_flags_no_saved_settings = 1 << 8,
    window_flags_no_mouse_inputs = 1 << 9,
    window_flags_menu_bar = 1 << 10,
    window_flags_horizontal_scrollbar = 1 << 11,
    window_flags_no_focus_on_appearing = 1 << 12,
    window_flags_no_bring_to_front_on_focus = 1 << 13,
    window_flags_always_vertical_scrollbar = 1 << 14,
    window_flags_always_horizontal_scrollbar = 1 << 15,
    window_flags_no_nav_inputs = 1 << 16,
    window_flags_no_nav_focus = 1 << 17,
    window_flags_unsaved_document = 1 << 18,
    window_flags_no_nav = window_flags_no_nav_inputs | window_flags_no_nav_focus,
    window_flags_no_decoration = window_flags_no_title_bar | window_flags_no_resize | window_flags_no_scrollbar | window_flags_no_collapse,
    window_flags_no_inputs = window_flags_no_mouse_inputs | window_flags_no_nav_inputs | window_flags_no_nav_focus,

    window_flags_child_window = 1 << 24,
    window_flags_tooltip = 1 << 25,
    window_flags_popup = 1 << 26,
    window_flags_modal = 1 << 27,
    window_flags_child_menu = 1 << 28,

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    window_flags_always_use_window_padding = 1 << 30,
    window_flags_nav_flattened = 1 << 31,
#endif
};

enum child_flags_
{
    child_flags_none = 0,
    child_flags_borders = 1 << 0,
    child_flags_always_use_window_padding = 1 << 1,
    child_flags_resize_x = 1 << 2,
    child_flags_resize_y = 1 << 3,
    child_flags_auto_resize_x = 1 << 4,
    child_flags_auto_resize_y = 1 << 5,
    child_flags_always_auto_resize = 1 << 6,
    child_flags_frame_style = 1 << 7,
    child_flags_nav_flattened = 1 << 8,

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    child_flags_border = child_flags_borders,
#endif
};

enum gui_cond_
{
    gui_cond_none = 0,
    gui_cond_always = 1 << 0,
    gui_cond_once = 1 << 1,
    gui_cond_first_use_ever = 1 << 2,
    gui_cond_appearing = 1 << 3,
};

enum draw_flags_
{
    draw_flags_none = 0,
    draw_flags_closed = 1 << 0,
    draw_flags_round_corners_top_left = 1 << 4,
    draw_flags_round_corners_top_right = 1 << 5,
    draw_flags_round_corners_bottom_left = 1 << 6,
    draw_flags_round_corners_bottom_right = 1 << 7,
    draw_flags_round_corners_none = 1 << 8,
    draw_flags_round_corners_top = draw_flags_round_corners_top_left | draw_flags_round_corners_top_right,
    draw_flags_round_corners_bottom = draw_flags_round_corners_bottom_left | draw_flags_round_corners_bottom_right,
    draw_flags_round_corners_left = draw_flags_round_corners_bottom_left | draw_flags_round_corners_top_left,
    draw_flags_round_corners_right = draw_flags_round_corners_bottom_right | draw_flags_round_corners_top_right,
    draw_flags_round_corners_all = draw_flags_round_corners_top_left | draw_flags_round_corners_top_right | draw_flags_round_corners_bottom_left | draw_flags_round_corners_bottom_right,
    draw_flags_round_corners_default = draw_flags_round_corners_all,
    draw_flags_round_corners_mask = draw_flags_round_corners_all | draw_flags_round_corners_none,
    draw_flags_shadow_cut_out_shape_background = 1 << 9
};

enum mouse_button_
{
    mouse_button_left = 0,
    mouse_button_right = 1,
    mouse_button_middle = 2,
    mouse_button_count = 5
};

struct data_var_info
{
    ImU32           count;
    ImU32           offset;
    void* get_var_ptr(void* parent) const { return (void*)((unsigned char*)parent + offset); }
};

struct gui_style
{
    float alpha;
    float disabled_alpha;
    ImVec2 window_padding;
    float window_rounding;
    float window_border_size;
    ImVec2 window_min_size;
    ImVec2 window_title_align;
    ImGuiDir window_menu_button_position;
    float child_rounding;
    float child_border_size;
    float popup_rounding;
    float popup_border_size;
    ImVec2 frame_padding;
    float frame_rounding;
    float frame_border_size;
    ImVec2 item_spacing;
    ImVec2 item_inner_spacing;
    ImVec2 cell_padding;
    ImVec2 touch_extra_padding;
    float indent_spacing;
    float columns_min_spacing;
    float scrollbar_size;
    float scrollbar_rounding;
    float grab_min_size;
    float grab_rounding;
    float log_slider_deadzone;
    float tab_rounding;
    float tab_border_size;
    float tab_min_width_for_close_button;
    float tab_bar_border_size;
    float tab_bar_overline_size;
    float table_angled_headers_angle;
    ImVec2 table_angled_headers_text_align;
    ImGuiDir color_button_position;
    ImVec2 button_text_align;
    ImVec2 selectable_text_align;
    float separator_text_border_size;
    ImVec2 separator_text_align;
    ImVec2 separator_text_padding;
    float window_shadow_size;
    ImVec2 window_shadow_offset;
    ImVec2 scrollbar_border_padding;
    float scrollbar_content_padding;
    ImVec2 display_window_padding;
    ImVec2 display_safe_area_padding;
    float mouse_cursor_scale;
    bool anti_aliased_lines;
    bool anti_aliased_lines_use_tex;
    bool anti_aliased_fill;
    float curve_tessellation_tol;
    float circle_tessellation_max_error;
    ImVec4 colors[ImGuiCol_COUNT];

    float hover_stationary_delay;
    float hover_delay_short;
    float hover_delay_normal;
    ImGuiHoveredFlags hover_flags_for_tooltip_mouse;
    ImGuiHoveredFlags hover_flags_for_tooltip_nav;

    gui_style();
};

inline gui_style::gui_style()
{
    alpha = 1.0f;
    disabled_alpha = 0.60f;
    window_padding = ImVec2(8, 8);
    window_rounding = 0.0f;
    window_border_size = 1.0f;
    window_min_size = ImVec2(32, 32);
    window_title_align = ImVec2(0.0f, 0.5f);
    window_menu_button_position = ImGuiDir_Left;
    child_rounding = 0.0f;
    child_border_size = 1.0f;
    popup_rounding = 0.0f;
    popup_border_size = 1.0f;
    frame_padding = ImVec2(4, 3);
    frame_rounding = 0.0f;
    frame_border_size = 0.0f;
    item_spacing = ImVec2(8, 4);
    item_inner_spacing = ImVec2(4, 4);
    cell_padding = ImVec2(4, 2);
    touch_extra_padding = ImVec2(0, 0);
    indent_spacing = 21.0f;
    columns_min_spacing = 6.0f;
    scrollbar_size = 14.0f;
    scrollbar_rounding = 9.0f;
    grab_min_size = 12.0f;
    grab_rounding = 0.0f;
    log_slider_deadzone = 4.0f;
    tab_rounding = 4.0f;
    tab_border_size = 0.0f;
    tab_min_width_for_close_button = 0.0f;
    tab_bar_border_size = 1.0f;
    tab_bar_overline_size = 2.0f;
    table_angled_headers_angle = 35.0f * (IM_PI / 180.0f);
    table_angled_headers_text_align = ImVec2(0.5f, 0.0f);
    color_button_position = ImGuiDir_Right;
    button_text_align = ImVec2(0.5f, 0.5f);
    selectable_text_align = ImVec2(0.0f, 0.0f);
    separator_text_border_size = 3.0f;
    separator_text_align = ImVec2(0.0f, 0.5f);
    separator_text_padding = ImVec2(20.0f, 3.0f);
    window_shadow_size = 100.0f;
    window_shadow_offset = ImVec2(0, 0);
    scrollbar_border_padding = ImVec2(6, 6);
    scrollbar_content_padding = 6;
    display_window_padding = ImVec2(19, 19);
    display_safe_area_padding = ImVec2(3, 3);
    mouse_cursor_scale = 1.0f;
    anti_aliased_lines = true;
    anti_aliased_lines_use_tex = true;
    anti_aliased_fill = true;
    curve_tessellation_tol = 1.25f;
    circle_tessellation_max_error = 0.30f;
    hover_stationary_delay = 0.15f;
    hover_delay_short = 0.15f;
    hover_delay_normal = 0.40f;
    hover_flags_for_tooltip_mouse = ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled;
    hover_flags_for_tooltip_nav = ImGuiHoveredFlags_NoSharedDelay | ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_AllowWhenDisabled;

    colors[style_col_text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[style_col_text_disabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[style_col_window_bg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[style_col_child_bg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[style_col_popup_bg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[style_col_border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[style_col_border_shadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[style_col_frame_bg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[style_col_frame_bg_hovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[style_col_frame_bg_active] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[style_col_title_bg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[style_col_title_bg_active] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[style_col_title_bg_collapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[style_col_menu_bar_bg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[style_col_scrollbar_bg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[style_col_scrollbar_grab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[style_col_scrollbar_grab_hovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[style_col_scrollbar_grab_active] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[style_col_check_mark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[style_col_slider_grab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[style_col_slider_grab_active] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[style_col_button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[style_col_button_hovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[style_col_button_active] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[style_col_header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[style_col_header_hovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[style_col_header_active] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[style_col_separator] = colors[style_col_border];
    colors[style_col_separator_hovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[style_col_separator_active] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[style_col_resize_grip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[style_col_resize_grip_hovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[style_col_resize_grip_active] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[style_col_tab_hovered] = colors[style_col_header_hovered];
    colors[style_col_tab] = ImLerp(colors[style_col_header], colors[style_col_title_bg_active], 0.80f);
    colors[style_col_tab_selected] = ImLerp(colors[style_col_header_active], colors[style_col_title_bg_active], 0.60f);
    colors[style_col_tab_selected_overline] = colors[style_col_header_active];
    colors[style_col_tab_dimmed] = ImLerp(colors[style_col_tab], colors[style_col_title_bg], 0.80f);
    colors[style_col_tab_dimmed_selected] = ImLerp(colors[style_col_tab_selected], colors[style_col_title_bg], 0.40f);
    colors[style_col_tab_dimmed_selected_overline] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[style_col_plot_lines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[style_col_plot_lines_hovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[style_col_plot_histogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[style_col_plot_histogram_hovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[style_col_table_header_bg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[style_col_table_border_strong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[style_col_table_border_light] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[style_col_table_row_bg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[style_col_table_row_bg_alt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[style_col_text_link] = colors[style_col_header_active];
    colors[style_col_text_selected_bg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[style_col_drag_drop_target] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[style_col_nav_highlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[style_col_nav_windowing_highlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[style_col_nav_windowing_dim_bg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[style_col_modal_window_dim_bg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[style_col_window_shadow] = ImVec4(0.08f, 0.08f, 0.08f, 0.35f);

}

static const data_var_info style_var_info[] =
{
    { 1, (ImU32)offsetof(gui_style, alpha) },
    { 1, (ImU32)offsetof(gui_style, disabled_alpha) },
    { 2, (ImU32)offsetof(gui_style, window_padding) },
    { 1, (ImU32)offsetof(gui_style, window_rounding) },
    { 1, (ImU32)offsetof(gui_style, window_border_size) },
    { 2, (ImU32)offsetof(gui_style, window_min_size) },
    { 2, (ImU32)offsetof(gui_style, window_title_align) },
    { 1, (ImU32)offsetof(gui_style, child_rounding) },
    { 1, (ImU32)offsetof(gui_style, child_border_size) },
    { 1, (ImU32)offsetof(gui_style, popup_rounding) },
    { 1, (ImU32)offsetof(gui_style, popup_border_size) },
    { 2, (ImU32)offsetof(gui_style, frame_padding) },
    { 1, (ImU32)offsetof(gui_style, frame_rounding) },
    { 1, (ImU32)offsetof(gui_style, frame_border_size) },
    { 2, (ImU32)offsetof(gui_style, item_spacing) },
    { 2, (ImU32)offsetof(gui_style, item_inner_spacing) },
    { 1, (ImU32)offsetof(gui_style, indent_spacing) },
    { 2, (ImU32)offsetof(gui_style, cell_padding) },
    { 1, (ImU32)offsetof(gui_style, scrollbar_size) },
    { 1, (ImU32)offsetof(gui_style, scrollbar_rounding) },
    { 1, (ImU32)offsetof(gui_style, grab_min_size) },
    { 1, (ImU32)offsetof(gui_style, grab_rounding) },
    { 1, (ImU32)offsetof(gui_style, tab_rounding) },
    { 1, (ImU32)offsetof(gui_style, tab_border_size) },
    { 1, (ImU32)offsetof(gui_style, tab_bar_border_size) },
    { 1, (ImU32)offsetof(gui_style, tab_bar_overline_size) },
    { 1, (ImU32)offsetof(gui_style, table_angled_headers_angle) },
    { 2, (ImU32)offsetof(gui_style, table_angled_headers_text_align) },
    { 2, (ImU32)offsetof(gui_style, button_text_align) },
    { 2, (ImU32)offsetof(gui_style, selectable_text_align) },
    { 1, (ImU32)offsetof(gui_style, separator_text_border_size) },
    { 2, (ImU32)offsetof(gui_style, separator_text_align) },
    { 2, (ImU32)offsetof(gui_style, separator_text_padding) },
    { 1, (ImU32)offsetof(gui_style, window_shadow_size) },
    { 2, (ImU32)offsetof(gui_style, window_shadow_offset) },
    { 2, (ImU32)offsetof(gui_style, scrollbar_border_padding) },
    { 1, (ImU32)offsetof(gui_style, scrollbar_content_padding) },
};

struct gui_style_mod
{
    style_var   var_idx;
    union { int backup_int[2]; float backup_float[2]; };
    gui_style_mod(style_var idx, int v) { var_idx = idx; backup_int[0] = v; }
    gui_style_mod(style_var idx, float v) { var_idx = idx; backup_float[0] = v; }
    gui_style_mod(style_var idx, ImVec2 v) { var_idx = idx; backup_float[0] = v.x; backup_float[1] = v.y; }
};

struct gui_color_mod
{
    style_col        col;
    ImVec4          backup_value;
};

inline std::vector<gui_style_mod> style_var_stack;
inline std::vector<gui_color_mod> style_col_stack;

inline const data_var_info* get_style_var_info(style_var idx)
{
    return &style_var_info[idx];
}

