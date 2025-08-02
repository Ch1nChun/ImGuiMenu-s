#include "../settings/functions.h"

void render_color_rect_with_alpha_checkboard(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, ImU32 col, float grid_step, ImVec2 grid_off, float rounding, ImDrawFlags flags)
{
    if ((flags & ImDrawFlags_RoundCornersMask_) == 0)
        flags = ImDrawFlags_RoundCornersDefault_;
    if (((col & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT) < 0xFF)
    {
        ImU32 col_bg1 = GetColorU32(ImAlphaBlendColors(IM_COL32(34, 32, 40, 255), col));
        ImU32 col_bg2 = GetColorU32(ImAlphaBlendColors(IM_COL32(10, 7, 19, 255), col));
        draw_list->AddRectFilled(p_min, p_max, col_bg1, rounding, flags);

        int yi = 0;
        for (float y = p_min.y + grid_off.y; y < p_max.y; y += grid_step, yi++)
        {
            float y1 = ImClamp(y, p_min.y, p_max.y), y2 = ImMin(y + grid_step, p_max.y);
            if (y2 <= y1)
                continue;
            for (float x = p_min.x + grid_off.x + (yi & 1) * grid_step; x < p_max.x; x += grid_step * 2.0f)
            {
                float x1 = ImClamp(x, p_min.x, p_max.x), x2 = ImMin(x + grid_step, p_max.x);
                if (x2 <= x1)
                    continue;
                ImDrawFlags cell_flags = ImDrawFlags_RoundCornersNone;
                if (y1 <= p_min.y) { if (x1 <= p_min.x) cell_flags |= ImDrawFlags_RoundCornersTopLeft; if (x2 >= p_max.x) cell_flags |= ImDrawFlags_RoundCornersTopRight; }
                if (y2 >= p_max.y) { if (x1 <= p_min.x) cell_flags |= ImDrawFlags_RoundCornersBottomLeft; if (x2 >= p_max.x) cell_flags |= ImDrawFlags_RoundCornersBottomRight; }

                // Combine flags
                cell_flags = (flags == ImDrawFlags_RoundCornersNone || cell_flags == ImDrawFlags_RoundCornersNone) ? ImDrawFlags_RoundCornersNone : (cell_flags & flags);
                draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col_bg2, rounding, cell_flags);
            }
        }
    }
    else
    {
        draw_list->AddRectFilled(p_min, p_max, col, rounding, flags);
    }
}

bool color_button(const char* desc_id, const ImVec4& col, bool opened)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID((std::stringstream{} << desc_id << "color_button").str().c_str());

    const ImVec2 pos = window->DC.CursorPos;
    const ImRect rect(pos, pos + SCALE(20, 20));
    ItemSize(rect, 0.0f);
    if (!ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(rect, id, &hovered, &held);

    draw->render_text(window->DrawList, set->c_font.icon[1], rect.Min, rect.Max, gui->get_clr(col), "D", NULL, NULL, ImVec2(0.5f, 0.5f));

    return pressed;
}

static void color_edit_restore_h(const float* col, float* H)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.ColorEditCurrentID != 0);
    if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
        return;
    *H = g.ColorEditSavedHue;
}

static void color_edit_restore_hs(const float* col, float* H, float* S, float* V)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.ColorEditCurrentID != 0);
    if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
        return;

    // When S == 0, H is undefined.
    // When H == 1 it wraps around to 0.
    if (*S == 0.0f || (*H == 0.0f && g.ColorEditSavedHue == 1))
        *H = g.ColorEditSavedHue;

    // When V == 0, S is undefined.
    if (*V == 0.0f)
        *S = g.ColorEditSavedSat;
}

ImVec4 imu32_to_imvec4(ImU32 color)
{
    float r = ((color >> 0) & 0xFF) / 255.0f;
    float g = ((color >> 8) & 0xFF) / 255.0f;
    float b = ((color >> 16) & 0xFF) / 255.0f;
    float a = ((color >> 24) & 0xFF) / 255.0f;
    return ImVec4(r, g, b, a);
}

bool color_picker(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
{
    struct picker_state
    {
        float lerp[2] = { 0, 0 };
    };

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImDrawList* draw_list = window->DrawList;
    ImGuiStyle& style = g.Style;
    ImGuiIO& io = g.IO;

    const float width = CalcItemWidth();
    const bool is_readonly = ((g.NextItemData.ItemFlags | g.CurrentItemFlags) & ImGuiItemFlags_ReadOnly) != 0;
    g.NextItemData.ClearFlags();

    PushID(label);
    const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
    if (set_current_color_edit_id)
        g.ColorEditCurrentID = window->IDStack.back();
    BeginGroup();

    if (!(flags & ImGuiColorEditFlags_NoSidePreview))
        flags |= ImGuiColorEditFlags_NoSmallPreview;

    // Context menu: display and store options.
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        ColorPickerOptionsPopup(col, flags);

    // Read stored options
    if (!(flags & ImGuiColorEditFlags_PickerMask_))
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
    if (!(flags & ImGuiColorEditFlags_InputMask_))
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_)); // Check that only 1 is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));  // Check that only 1 is selected
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

    // Setup
    int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
    bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
    ImVec2 picker_pos = window->DC.CursorPos;

    ImVec2 hue_bar_pos = picker_pos + ImVec2(0, SCALE(set->c_picker.picker_size.y) + SCALE(set->c_picker.padding.x));
    ImVec2 alpha_bar_pos = hue_bar_pos + ImVec2(0, SCALE(set->c_picker.bar_size.y) + SCALE(set->c_picker.padding.x));

    float backup_initial_col[4];
    memcpy(backup_initial_col, col, components * sizeof(float));

    float H = col[0], S = col[1], V = col[2];
    float R = col[0], G = col[1], B = col[2];

    if (flags & ImGuiColorEditFlags_InputRGB)
    {
        // Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
        ColorConvertRGBtoHSV(R, G, B, H, S, V);
        color_edit_restore_hs(col, &H, &S, &V);
    }
    else if (flags & ImGuiColorEditFlags_InputHSV)
    {
        ColorConvertHSVtoRGB(H, S, V, R, G, B);
    }

    picker_state* animstate = gui->anim_container(&animstate, GetID((std::stringstream{} << GetCurrentWindow()->ID << "saved_pos").str().c_str()));

    bool value_changed = false, value_changed_h = false, value_changed_sv = false;
    PushItemFlag(ImGuiItemFlags_NoNav, true);
    // SV rectangle logic
    InvisibleButton("sv", SCALE(set->c_picker.picker_size));
    if (IsItemActive() && !is_readonly)
    {
        S = ImSaturate((io.MousePos.x - picker_pos.x) / (SCALE(set->c_picker.picker_size.x) - 1));
        V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (SCALE(set->c_picker.picker_size.y) - 1));
        color_edit_restore_h(col, &H); // Greatly reduces hue jitter and reset to 0 when hue == 255 and color is rapidly modified using SV square.
        value_changed = value_changed_sv = true;
    }

    // Hue bar logic
    SetCursorScreenPos(hue_bar_pos);
    InvisibleButton("hue", SCALE(set->c_picker.bar_size));
    if (IsItemActive() && !is_readonly)
    {
        H = 1.0f - ImSaturate((io.MousePos.x - picker_pos.x) / (SCALE(set->c_picker.bar_size.x) - 1));
        value_changed = value_changed_h = true;
    }

    // Alpha bar logic
    if (alpha_bar)
    {
        SetCursorScreenPos(alpha_bar_pos);
        InvisibleButton("alpha", SCALE(set->c_picker.bar_size));
        if (IsItemActive())
        {
            col[3] = ImSaturate((io.MousePos.x - picker_pos.x) / (SCALE(set->c_picker.bar_size.x) - 1));
            value_changed = true;
        }
    }
    PopItemFlag(); // ImGuiItemFlags_NoNav

    // Convert back color to RGB
    if (value_changed_h || value_changed_sv)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);
            g.ColorEditSavedHue = H;
            g.ColorEditSavedSat = S;
            g.ColorEditSavedID = g.ColorEditCurrentID;
            g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            col[0] = H;
            col[1] = S;
            col[2] = V;
        }
    }

    // R,G,B and H,S,V slider color editor
    bool value_changed_fix_hue_wrap = false;

    // Try to cancel hue wrap (after ColorEdit4 call), if any
    if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB))
    {
        float new_H, new_S, new_V;
        ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
        if (new_H <= 0 && H > 0)
        {
            if (new_V <= 0 && V != new_V)
                ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
            else if (new_S <= 0)
                ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
        }
    }

    if (value_changed)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            R = col[0];
            G = col[1];
            B = col[2];
            ColorConvertRGBtoHSV(R, G, B, H, S, V);
            color_edit_restore_hs(col, &H, &S, &V);   // Fix local Hue as display below will use it immediately.
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            H = col[0];
            S = col[1];
            V = col[2];
            ColorConvertHSVtoRGB(H, S, V, R, G, B);
        }
    }

    if ((flags & ImGuiColorEditFlags_NoInputs) == 0)
    {
        ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
        ImGuiColorEditFlags sub_flags = (flags & sub_flags_to_forward) | ImGuiColorEditFlags_NoPicker;
        if (flags & ImGuiColorEditFlags_DisplayHex || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
        {
            SetCursorPosY(GetCursorPosY() + SCALE(10));
            value_changed |= widget->color_edit("##hex", col, sub_flags | ImGuiColorEditFlags_DisplayHex);
        }
    }

    const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
    const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
    const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
    const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
    const ImU32 col_hues[6 + 1] = {
        IM_COL32(255,0,0,style_alpha8),    // 7
        IM_COL32(255,0,255,style_alpha8),  // 6
        IM_COL32(0,0,255,style_alpha8),    // 5
        IM_COL32(0,255,255,style_alpha8),  // 4
        IM_COL32(0,255,0,style_alpha8),    // 3
        IM_COL32(255,255,0,style_alpha8),  // 2
        IM_COL32(255,0,0,style_alpha8)     // 1
    };

    ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
    ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
    ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha)); // Important: this is still including the main rendering/style alpha!!

    ImVec2 sv_cursor_pos;

    // Render SV Square
    draw->rect_filled_multi_color(draw_list, picker_pos, picker_pos + SCALE(set->c_picker.picker_size), col_white, hue_color32, hue_color32, col_white, SCALE(set->c_picker.rounding + 1));
    draw->rect_filled_multi_color(draw_list, picker_pos, picker_pos + SCALE(set->c_picker.picker_size) + SCALE(0, 1), 0, 0, col_black, col_black, SCALE(set->c_picker.rounding));

    sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * SCALE(set->c_picker.picker_size.x)), picker_pos.x + 2, picker_pos.x + SCALE(set->c_picker.picker_size.x) - 2); // Sneakily prevent the circle to stick out too much
    sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * SCALE(set->c_picker.picker_size.y)), picker_pos.y + 2, picker_pos.y + SCALE(set->c_picker.picker_size.y) - 2);

    // Render Hue Bar
    for (int i = 0; i < 6; ++i)
        draw->rect_filled_multi_color(draw_list, hue_bar_pos + ImVec2(i * (SCALE(set->c_picker.bar_size.x) / 6) - (i == 5 ? 1 : 0), 0), hue_bar_pos + ImVec2((i + 1) * (SCALE(set->c_picker.bar_size.x) / 6) + (i == 0 ? 1 : 0), SCALE(set->c_picker.bar_size.y)), col_hues[i], col_hues[i + 1], col_hues[i + 1], col_hues[i], SCALE(set->c_element.rounding), i == 0 ? ImDrawFlags_RoundCornersLeft : i == 5 ? ImDrawFlags_RoundCornersRight : ImDrawFlags_RoundCornersNone);

    float hue_bar_circle_pos = IM_ROUND((1.0f - H) * SCALE(set->c_picker.bar_size.x));
    hue_bar_circle_pos = ImClamp(hue_bar_circle_pos, 4.f, SCALE(set->c_picker.bar_size.x) - 5.f);
    animstate->lerp[0] = ImLerp(animstate->lerp[0], hue_bar_circle_pos, gui->fixed_speed(20.f));
    draw_list->AddCircle(hue_bar_pos + ImVec2(animstate->lerp[0], SCALE(set->c_picker.bar_size.y) / 2), SCALE(set->c_picker.bar_size.y) / 2, col_white, 30, SCALE(1.f));

    // Render cursor/preview circle (clamp S/V within 0..1 range because floating points colors may lead HSV values to be out of range)
    draw_list->AddCircleFilled(sv_cursor_pos, SCALE(set->c_picker.bar_size.y) / 2, user_col32_striped_of_alpha, 20);
    draw_list->AddCircle(sv_cursor_pos, SCALE(set->c_picker.bar_size.y) / 2, col_white, 20, SCALE(1.f));

    // Render alpha bar
    if (alpha_bar)
    {
        float alpha = ImSaturate(col[3]);
        render_color_rect_with_alpha_checkboard(draw_list, alpha_bar_pos, alpha_bar_pos + SCALE(set->c_picker.bar_size), ImColor(0, 0, 0, 0), SCALE(set->c_picker.bar_size.y) / 2, ImVec2(0, 0), SCALE(set->c_element.rounding), 0);
        draw->rect_filled_multi_color(draw_list, alpha_bar_pos, alpha_bar_pos + SCALE(set->c_picker.bar_size), user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, SCALE(set->c_element.rounding));
        float alpha_bar_circle_pos = IM_ROUND(alpha * SCALE(set->c_picker.bar_size.x));
        alpha_bar_circle_pos = ImClamp(alpha_bar_circle_pos, 5.f, SCALE(set->c_picker.bar_size.x) - 5.f);
        animstate->lerp[1] = ImLerp(animstate->lerp[1], alpha_bar_circle_pos, gui->fixed_speed(20.f));
        draw_list->AddCircle(alpha_bar_pos + ImVec2(animstate->lerp[1], SCALE(set->c_picker.bar_size.y) / 2), SCALE(set->c_picker.bar_size.y) / 2, col_white, 30, SCALE(1.f));
    }

    EndGroup();

    if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0)
        value_changed = false;
    if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
        MarkItemEdited(g.LastItemData.ID);

    if (set_current_color_edit_id)
        g.ColorEditCurrentID = 0;
    PopID();

    return value_changed;
}

bool c_widget::color_edit(std::string_view label, float col[4], ImGuiColorEditFlags flags)
{
    struct color_state
    {
        bool active = false;
        bool hovered = false;
        float alpha = 0.f;
        float saturation = 4;
        ImU32 hue_col;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float square_sz = GetFrameHeight();
    const char* label_display_end = FindRenderedTextEnd(label.data());
    float w_full = CalcItemWidth();
    g.NextItemData.ClearFlags();

    BeginGroup();
    PushID(label.data());
    const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
    if (set_current_color_edit_id)
        g.ColorEditCurrentID = window->IDStack.back();

    // If we're not showing any slider there's no point in doing any HSV conversions
    const ImGuiColorEditFlags flags_untouched = flags;
    if (flags & ImGuiColorEditFlags_NoInputs)
        flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

    // Context menu: display and modify options (before defaults are applied)
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        ColorEditOptionsPopup(col, flags);

    // Read stored options
    if (!(flags & ImGuiColorEditFlags_DisplayMask_))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_);
    if (!(flags & ImGuiColorEditFlags_DataTypeMask_))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DataTypeMask_);
    if (!(flags & ImGuiColorEditFlags_PickerMask_))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_);
    if (!(flags & ImGuiColorEditFlags_InputMask_))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);
    flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_));
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_DisplayMask_)); // Check that only 1 is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));   // Check that only 1 is selected

    const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
    const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
    const int components = alpha ? 4 : 3;
    const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
    const float w_inputs = ImMax(w_full - w_button, 1.0f);
    w_full = w_inputs + w_button;

    // Convert to the formats we need
    float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
    if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
        ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
    else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
    {
        // Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
        ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
        color_edit_restore_hs(col, &f[0], &f[1], &f[2]);
    }
    int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

    bool value_changed = false;
    bool value_changed_as_float = false;

    const ImVec2 pos = window->DC.CursorPos;
    const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;

    color_state* state = gui->anim_container(&state, GetID(label.data()));

    if ((flags & ImGuiColorEditFlags_DisplayHex) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
    {
        // RGB Hexadecimal Input
        char buf[64];
        if (alpha)
            ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
        else
            ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));
        SetNextItemWidth(w_inputs);
        gui->set_cursor_pos_y(gui->get_cursor_pos_y() - SCALE(11));
        if (widget->text_field("##Text", "M", buf, IM_ARRAYSIZE(buf), SCALE(150, 30), ImGuiInputTextFlags_CharsUppercase))
        {
            value_changed = true;
            char* p = buf;
            while (*p == '#' || ImCharIsBlankA(*p))
                p++;
            i[0] = i[1] = i[2] = 0;
            i[3] = 0xFF; // alpha default to 255 is not parsed by scanf (e.g. inputting #FFFFFF omitting alpha)
            int r;
            if (alpha)
                r = sscanf(p, "%02X%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2], (unsigned int*)&i[3]); // Treat at unsigned (%X is unsigned)
            else
                r = sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
            IM_UNUSED(r); // Fixes C6031: Return value ignored: 'sscanf'.
        }
    }

    if (g.LastItemData.Rect.Contains(g.IO.MousePos) && g.IO.MouseClicked[1] || (state->active && (g.IO.MouseClicked[0] || g.IO.MouseClicked[1]) && !state->hovered))
        state->active = !state->active;

    ImGuiWindow* picker_active_window = NULL;
    if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
    {
        const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);
        if (color_button(label.data(), col_v4, state->active))
            g.ColorPickerRef = col_v4;

        state->alpha = ImClamp(state->alpha + (8.f * g.IO.DeltaTime * (state->active ? 1.f : -1.f)), 0.f, 1.f);

        if (!IsRectVisible(g.LastItemData.Rect.Min, g.LastItemData.Rect.Max + ImVec2(0, 2)))
        {
            state->active = false;
            state->alpha = 0.f;
        }

        //SetNextWindowSize(SCALE(170, flags & ImGuiColorEditFlags_AlphaBar ? 272 : 254));
        SetNextWindowPos(g.LastItemData.Rect.GetTL());
        gui->push_style_var(ImGuiStyleVar_Alpha, state->alpha);
        gui->push_style_var(ImGuiStyleVar_WindowBorderSize, 1.f);
        gui->push_style_var(ImGuiStyleVar_WindowRounding, SCALE(set->c_child.rounding));

        gui->push_style_color(ImGuiCol_PopupBg, gui->get_clr(clr->c_child.layout));
        gui->push_style_color(ImGuiCol_Border, gui->get_clr(clr->c_child.stroke));

        if (state->alpha >= 0.01f);
        {
            gui->begin((std::stringstream{} << "picker_window" << GetID(label.data())).str(), NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize);
            {
                if (state->active)
                    SetWindowFocus();
                state->hovered = IsWindowHovered();
                picker_active_window = g.CurrentWindow;

                ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
                ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;

                gui->begin_group();

                value_changed |= color_picker("##picker", col, picker_flags, &g.ColorPickerRef.x);

                gui->end_group();

            }
            gui->end();
        }
        gui->pop_style_var(3);
        gui->pop_style_color(2);
    }

    // Convert back
    if (value_changed && picker_active_window == NULL)
    {
        if (!value_changed_as_float)
            for (int n = 0; n < 4; n++)
                f[n] = i[n] / 255.0f;
        if ((flags & ImGuiColorEditFlags_DisplayHSV) && (flags & ImGuiColorEditFlags_InputRGB))
        {
            g.ColorEditSavedHue = f[0];
            g.ColorEditSavedSat = f[1];
            ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
            g.ColorEditSavedID = g.ColorEditCurrentID;
            g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(f[0], f[1], f[2], 0));
        }
        if ((flags & ImGuiColorEditFlags_DisplayRGB) && (flags & ImGuiColorEditFlags_InputHSV))
            ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

        col[0] = f[0];
        col[1] = f[1];
        col[2] = f[2];
        if (alpha)
            col[3] = f[3];
    }

    if (set_current_color_edit_id)
        g.ColorEditCurrentID = 0;
    PopID();
    EndGroup();

    // When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
    if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
        g.LastItemData.ID = g.ActiveId;

    if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
        MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}
