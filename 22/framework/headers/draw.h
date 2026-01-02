#pragma once
#include "includes.h"

enum fade_direction : int
{
    vertically,
    horizontally,
    diagonally,
    diagonally_reversed,
};

class c_draw
{
public:
    ImU32 get_clr(const ImVec4& col, float alpha = 1.f);

    ImU32 w_get_clr(style_col idx, float alpha = 1.f);

    void text(ImDrawList* draw_list, const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL, float wrap_width = 0.f, const ImVec4* cpu_fine_clip_rect = 0);

    void text_clipped(ImDrawList* draw_list, ImFont* font, const ImVec2& pos_min, const ImVec2& pos_max, ImU32 color, const char* text, const char* text_display_end = NULL, const ImVec2* text_size_if_known = NULL, const ImVec2& align = ImVec2(0.f, 0.f), const ImRect* clip_rect = NULL);

    void radial_gradient(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out);

    void set_linear_color_alpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1);

    void line(ImDrawList* draw_list, const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 1.0f);

    void rect(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, draw_flags flags = 0, float thickness = 1.0f);

    void rect_filled(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, draw_flags flags = 0);

    void rect_filled_multi_color(ImDrawList* draw, const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left, float rounding = 0.f, draw_flags flags = 0);

    void fade_rect_filled(ImDrawList* draw, const ImVec2& pos_min, const ImVec2& pos_max, ImU32 col_one, ImU32 col_two, fade_direction direction, float rounding = 0.f, draw_flags flags = 0);

    void shadow_rect(ImDrawList* draw_list, const ImVec2& obj_min, const ImVec2& obj_max, ImU32 shadow_col, float shadow_thickness, const ImVec2& shadow_offset, draw_flags flags = 0, float obj_rounding = 0.0f);

    void circle(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments = 0, float thickness = 1.0f);

    void circle_filled(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments = 0);

    void shadow_circle(ImDrawList* draw_list, const ImVec2& obj_center, float obj_radius, ImU32 shadow_col, float shadow_thickness, const ImVec2& shadow_offset, draw_flags flags = 0, int obj_num_segments = 12);

    void triangle(ImDrawList* draw_list, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness = 1.f);

    void triangle_filled(ImDrawList* draw_list, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col);

    void image(ImDrawList* draw_list, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min = ImVec2(0, 0), const ImVec2& uv_max = ImVec2(1, 1), ImU32 col = IM_COL32_WHITE);

    void image_rounded(ImDrawList* draw_list, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min = ImVec2(0, 0), const ImVec2& uv_max = ImVec2(1, 1), ImU32 col = IM_COL32_WHITE, float rounding = 1.f, draw_flags flags = 0);

    void shadow_convex_poly(ImDrawList* draw_list, const ImVec2* points, int points_count, ImU32 shadow_col, float shadow_thickness, const ImVec2& shadow_offset, draw_flags flags = 0);

    void shadow_ngon(ImDrawList* draw_list, const ImVec2& obj_center, float obj_radius, ImU32 shadow_col, float shadow_thickness, const ImVec2& shadow_offset, draw_flags flags, int obj_num_segments);

    void rotate_start(ImDrawList* draw_list);

    void rotate_end(ImDrawList* draw_list, float rad, ImVec2 center = ImVec2(0, 0));

    void push_clip_rect(ImDrawList* draw_list, const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect = false);

    void pop_clip_rect(ImDrawList* draw_list);
};

inline std::unique_ptr<c_draw> draw = std::make_unique<c_draw>();
