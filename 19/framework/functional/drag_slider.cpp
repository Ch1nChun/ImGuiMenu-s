#include "../settings/functions.h"

template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
bool slider_behavior_t(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, TYPE* v, const TYPE v_min, const TYPE v_max, const char* format, ImGuiSliderFlags flags, ImRect* out_grab_bb)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImGuiAxis axis = (flags & ImGuiSliderFlags_Vertical) ? ImGuiAxis_Y : ImGuiAxis_X;
    const bool is_logarithmic = (flags & ImGuiSliderFlags_Logarithmic) != 0;
    const bool is_floating_point = (data_type == ImGuiDataType_Float) || (data_type == ImGuiDataType_Double);
    const float v_range_f = (float)(v_min < v_max ? v_max - v_min : v_min - v_max);

    const float grab_padding = 2.0f;
    const float slider_sz = (bb.Max[axis] - bb.Min[axis]) - grab_padding * 2.0f;
    float grab_sz = 13.f;
    const float slider_usable_sz = slider_sz - grab_sz;
    const float slider_usable_pos_min = bb.Min[axis] + grab_padding + grab_sz * 0.5f;
    const float slider_usable_pos_max = bb.Max[axis] - grab_padding - grab_sz * 0.5f;

    float logarithmic_zero_epsilon = 0.0f;
    float zero_deadzone_halfsize = 0.0f;
    if (is_logarithmic)
    {
        const int decimal_precision = is_floating_point ? ImParseFormatPrecision(format, 3) : 1;
        logarithmic_zero_epsilon = ImPow(0.1f, (float)decimal_precision);
        zero_deadzone_halfsize = (style.LogSliderDeadzone * 0.5f) / ImMax(slider_usable_sz, 1.0f);
    }

    bool value_changed = false;
    if (g.ActiveId == id)
    {
        bool set_new_value = false;
        float clicked_t = 0.0f;
        if (g.ActiveIdSource == ImGuiInputSource_Mouse)
        {
            if (!g.IO.MouseDown[0])
            {
                ClearActiveID();
            }
            else
            {
                const float mouse_abs_pos = g.IO.MousePos[axis];
                if (g.ActiveIdIsJustActivated)
                {
                    float grab_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
                    if (axis == ImGuiAxis_Y)
                        grab_t = 1.0f - grab_t;
                    const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
                    const bool clicked_around_grab = (mouse_abs_pos >= grab_pos - grab_sz * 0.5f - 1.0f) && (mouse_abs_pos <= grab_pos + grab_sz * 0.5f + 1.0f);
                    g.SliderGrabClickOffset = (clicked_around_grab && is_floating_point) ? mouse_abs_pos - grab_pos : 0.0f;
                }
                if (slider_usable_sz > 0.0f)
                    clicked_t = ImSaturate((mouse_abs_pos - g.SliderGrabClickOffset - slider_usable_pos_min) / slider_usable_sz);
                if (axis == ImGuiAxis_Y)
                    clicked_t = 1.0f - clicked_t;
                set_new_value = true;
            }
        }
        else if (g.ActiveIdSource == ImGuiInputSource_Keyboard || g.ActiveIdSource == ImGuiInputSource_Gamepad)
        {
            if (g.ActiveIdIsJustActivated)
            {
                g.SliderCurrentAccum = 0.0f;
                g.SliderCurrentAccumDirty = false;
            }

            float input_delta = (axis == ImGuiAxis_X) ? GetNavTweakPressedAmount(axis) : -GetNavTweakPressedAmount(axis);
            if (input_delta != 0.0f)
            {
                const bool tweak_slow = IsKeyDown((g.NavInputSource == ImGuiInputSource_Gamepad) ? ImGuiKey_NavGamepadTweakSlow : ImGuiKey_NavKeyboardTweakSlow);
                const bool tweak_fast = IsKeyDown((g.NavInputSource == ImGuiInputSource_Gamepad) ? ImGuiKey_NavGamepadTweakFast : ImGuiKey_NavKeyboardTweakFast);
                const int decimal_precision = is_floating_point ? ImParseFormatPrecision(format, 3) : 0;
                if (decimal_precision > 0)
                {
                    input_delta /= 100.0f;
                    if (tweak_slow)
                        input_delta /= 10.0f;
                }
                else
                {
                    if ((v_range_f >= -100.0f && v_range_f <= 100.0f && v_range_f != 0.0f) || tweak_slow)
                        input_delta = ((input_delta < 0.0f) ? -1.0f : +1.0f) / v_range_f;
                    else
                        input_delta /= 100.0f;
                }
                if (tweak_fast)
                    input_delta *= 10.0f;

                g.SliderCurrentAccum += input_delta;
                g.SliderCurrentAccumDirty = true;
            }

            float delta = g.SliderCurrentAccum;
            if (g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated)
            {
                ClearActiveID();
            }
            else if (g.SliderCurrentAccumDirty)
            {
                clicked_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);

                if ((clicked_t >= 1.0f && delta > 0.0f) || (clicked_t <= 0.0f && delta < 0.0f))
                {
                    set_new_value = false;
                    g.SliderCurrentAccum = 0.0f;
                }
                else
                {
                    set_new_value = true;
                    float old_clicked_t = clicked_t;
                    clicked_t = ImSaturate(clicked_t + delta);

                    TYPE v_new = ScaleValueFromRatioT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, clicked_t, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
                    if (is_floating_point && !(flags & ImGuiSliderFlags_NoRoundToFormat))
                        v_new = RoundScalarWithFormatT<TYPE>(format, data_type, v_new);
                    float new_clicked_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, v_new, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);

                    if (delta > 0)
                        g.SliderCurrentAccum -= ImMin(new_clicked_t - old_clicked_t, delta);
                    else
                        g.SliderCurrentAccum -= ImMax(new_clicked_t - old_clicked_t, delta);
                }

                g.SliderCurrentAccumDirty = false;
            }
        }

        if (set_new_value)
            if ((g.LastItemData.InFlags & ImGuiItemFlags_ReadOnly) || (flags & ImGuiSliderFlags_ReadOnly))
                set_new_value = false;

        if (set_new_value)
        {
            TYPE v_new = ScaleValueFromRatioT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, clicked_t, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);

            if (is_floating_point && !(flags & ImGuiSliderFlags_NoRoundToFormat)) v_new = RoundScalarWithFormatT<TYPE>(format, data_type, v_new);

            if (*v != v_new)
            {
                *v = v_new;
                value_changed = true;
            }
        }
    }

    if (slider_sz < 1.0f)
    {
        *out_grab_bb = ImRect(bb.Min, bb.Min);
    }
    else
    {
        float grab_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
        if (axis == ImGuiAxis_Y)
            grab_t = 1.0f - grab_t;
        const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
        if (axis == ImGuiAxis_X)
            *out_grab_bb = ImRect(grab_pos - grab_sz * 0.5f, bb.Min.y + grab_padding, grab_pos + grab_sz * 0.5f, bb.Max.y - grab_padding);
        else
            *out_grab_bb = ImRect(bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f, bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f);
    }

    return value_changed;
}

bool slider_behavior(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, void* p_v, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags, ImRect* out_grab_bb)
{
    IM_ASSERT((flags == 1 || (flags & ImGuiSliderFlags_InvalidMask_) == 0) && "Invalid ImGuiSliderFlags flag!  Has the 'float power' argument been mistakenly cast to flags? Call function with ImGuiSliderFlags_Logarithmic flags instead.");

    switch (data_type)
    {
    case ImGuiDataType_S8: { ImS32 v32 = (ImS32) * (ImS8*)p_v;  bool r = slider_behavior_t<ImS32, ImS32, float>(bb, id, ImGuiDataType_S32, &v32, *(const ImS8*)p_min, *(const ImS8*)p_max, format, flags, out_grab_bb); if (r) *(ImS8*)p_v = (ImS8)v32;  return r; }
    case ImGuiDataType_U8: { ImU32 v32 = (ImU32) * (ImU8*)p_v;  bool r = slider_behavior_t<ImU32, ImS32, float>(bb, id, ImGuiDataType_U32, &v32, *(const ImU8*)p_min, *(const ImU8*)p_max, format, flags, out_grab_bb); if (r) *(ImU8*)p_v = (ImU8)v32;  return r; }
    case ImGuiDataType_S16: { ImS32 v32 = (ImS32) * (ImS16*)p_v; bool r = slider_behavior_t<ImS32, ImS32, float>(bb, id, ImGuiDataType_S32, &v32, *(const ImS16*)p_min, *(const ImS16*)p_max, format, flags, out_grab_bb); if (r) *(ImS16*)p_v = (ImS16)v32; return r; }
    case ImGuiDataType_U16: { ImU32 v32 = (ImU32) * (ImU16*)p_v; bool r = slider_behavior_t<ImU32, ImS32, float>(bb, id, ImGuiDataType_U32, &v32, *(const ImU16*)p_min, *(const ImU16*)p_max, format, flags, out_grab_bb); if (r) *(ImU16*)p_v = (ImU16)v32; return r; }
    case ImGuiDataType_S32:
        IM_ASSERT(*(const ImS32*)p_min >= INT_MIN / 2 && *(const ImS32*)p_max <= INT_MAX / 2);
        return slider_behavior_t<ImS32, ImS32, float >(bb, id, data_type, (ImS32*)p_v, *(const ImS32*)p_min, *(const ImS32*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_U32:
        IM_ASSERT(*(const ImU32*)p_max <= UINT_MAX / 2);
        return slider_behavior_t<ImU32, ImS32, float >(bb, id, data_type, (ImU32*)p_v, *(const ImU32*)p_min, *(const ImU32*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_S64:
        IM_ASSERT(*(const ImS64*)p_min >= LLONG_MIN / 2 && *(const ImS64*)p_max <= LLONG_MAX / 2);
        return slider_behavior_t<ImS64, ImS64, double>(bb, id, data_type, (ImS64*)p_v, *(const ImS64*)p_min, *(const ImS64*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_U64:
        IM_ASSERT(*(const ImU64*)p_max <= ULLONG_MAX / 2);
        return slider_behavior_t<ImU64, ImS64, double>(bb, id, data_type, (ImU64*)p_v, *(const ImU64*)p_min, *(const ImU64*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_Float:
        IM_ASSERT(*(const float*)p_min >= -FLT_MAX / 2.0f && *(const float*)p_max <= FLT_MAX / 2.0f);
        return slider_behavior_t<float, float, float >(bb, id, data_type, (float*)p_v, *(const float*)p_min, *(const float*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_Double:
        IM_ASSERT(*(const double*)p_min >= -DBL_MAX / 2.0f && *(const double*)p_max <= DBL_MAX / 2.0f);
        return slider_behavior_t<double, double, double>(bb, id, data_type, (double*)p_v, *(const double*)p_min, *(const double*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_COUNT: break;
    }
    IM_ASSERT(0);
    return{ false };
}

template<typename T>
struct c_slider {
    ImVec4 text = clr->c_text.text;
    float grab_slide{ 0.f }, grab_offset{ 0.f }, circle{ 8.f };
    float alpha{ 0 };
    bool selected[5];
    float height[5];
    float alphab[5];
    T slow_value{ 0 };
};

template<typename T>
bool slider_scalar(std::string_view label, ImGuiDataType data_type, T* p_data, const T* p_min, const T* p_max, float power, const char* format)
{
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const float width = GetContentRegionAvail().x;
    const float height = SCALE(30);

    const ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, pos + ImVec2(width, height));
    const ImRect slider(pos + ImVec2(width - SCALE(55), 4), pos + ImVec2(width, height - SCALE(4)));

    ItemSize(ImRect(rect.Min, rect.Max));
    if (!ItemAdd(rect, id)) return false;

    if (!format) format = DataTypeGetInfo(data_type)->PrintFmt;
    bool hovered = ItemHoverable(slider, id, g.LastItemData.InFlags), held, pressed = ButtonBehavior(ImRect(rect.Min, rect.Max), id, &hovered, &held, NULL);

    ImRect grab_bb;
    const bool value_changed = slider_behavior(ImRect(slider.Min, slider.Max), id, data_type, p_data, p_min, p_max, format, NULL, &grab_bb);
    char value_buf[64];

    if (value_changed) MarkItemEdited(id);

    c_slider<T>* state = gui->anim_container(&state, id);

    state->grab_slide = ImLerp(state->grab_slide, grab_bb.Min.x - slider.Min.x, gui->fixed_speed(25.f));
    state->alpha = ImLerp(state->alpha, hovered ? 1.f : 0.f, gui->fixed_speed(8.f));
    state->text = ImLerp(state->text, IsItemActive() ? clr->c_text.text_active : hovered ? clr->c_text.text_hov : clr->c_text.text, gui->fixed_speed(8.f));

    draw->add_rect_filled(window->DrawList, slider.Min, slider.Max, gui->get_clr(clr->c_element.layout), set->c_element.rounding);
    
    if (hovered)
        set->c_window.slider_id = id;

    if (set->c_window.slider_id == id)
        set->c_window.slider_hovered = hovered;

    T* f_data = static_cast<T*>(p_data);
    const T* f_min = static_cast<const T*>(p_min);
    const T* f_max = static_cast<const T*>(p_max);

    const int num_bars = 5;

    float bar_width = SCALE(3);
    float bar_spacing = (slider.GetWidth() - num_bars * bar_width) / (num_bars);

    for (int i = 0; i < num_bars; ++i) {
        float side_padding = SCALE(6.f);
        float x = slider.Min.x + (slider.GetWidth() - side_padding) / num_bars * i;
        state->alphab[i] = ImLerp(state->alphab[i], state->selected[i] ? 1.f : 0.1f, gui->fixed_speed(12.f));

        state->height[i] = ImLerp(state->height[i], state->selected[i] ? SCALE(8.f) : SCALE(5.f), gui->fixed_speed(8.f));

        float lower_bound = *f_min + (*f_max - *f_min) * (i * 0.25f);
        float upper_bound = *f_min + (*f_max - *f_min) * ((i + 1) * 0.25);

        if (*f_data >= lower_bound && *f_data < upper_bound)
            state->selected[i] = true;
        else
            state->selected[i] = false;

        draw->add_rect_filled(window->DrawList, ImVec2(x + side_padding, slider.GetCenter().y - state->height[i]), ImVec2(x + bar_width + side_padding, slider.GetCenter().y + state->height[i]), gui->get_clr(clr->c_other_clr.accent_clr, state->alphab[i]), 30);
    }

    T* data_ptr = static_cast<T*>(p_data);
    T target_value = *data_ptr;
    
    if (hovered)
    {
        if (*f_data < *f_max)
        {
            if (GetIO().MouseWheel > 0.0f)
            {
                *data_ptr += power;
            }
        }

        if (*f_data > *f_min)
        {
            if (GetIO().MouseWheel < 0.0f)
            {
                *data_ptr -= power;
            }
        }

        *f_data = ImClamp(*f_data, *f_min, *f_max);
    }

    if constexpr (std::is_arithmetic_v<T>) {

        T step = (target_value - state->slow_value) * gui->fixed_speed(15.f);
        if (step == 0 && state->slow_value != target_value) step = (target_value > state->slow_value) ? 1 : -1;
        state->slow_value += step;
    } 

    snprintf(value_buf, sizeof(value_buf), format, state->slow_value);

    draw->render_text(set->c_font.inter_medium[0], window->DrawList, rect.Min, rect.Max, gui->get_clr(state->text), label.data(), NULL, NULL, { 0.0, 0.5 }, NULL);
    draw->render_text(set->c_font.inter_medium[0], window->DrawList, rect.Min, rect.Max - SCALE(70, 0), gui->get_clr(state->text), value_buf, NULL, NULL, {1.0, 0.5}, NULL);

    return value_changed;
}

bool c_widget::slider_float(std::string_view label, float* v, float v_min, float v_max, float power, const char* format)
{
    return slider_scalar<float>(label, ImGuiDataType_Float, v, &v_min, &v_max, power, format);
}

bool c_widget::slider_int(std::string_view label, int* v, int v_min, int v_max, int power, const char* format)
{
    return slider_scalar<int>(label, ImGuiDataType_S32, v, &v_min, &v_max, power, format);
}
