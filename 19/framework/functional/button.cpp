#include "../settings/functions.h"

bool c_widget::button(std::string_view label, const ImVec2& size)
{
    struct button_state
    {
        ImVec4 background;
    };

    ImGuiWindow* window = GetCurrentWindow();

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const float label_size = CalcTextSize(label.data(), NULL, true).x;
    const float width = GetContentRegionAvail().x;

    const ImVec2 pos = window->DC.CursorPos;
    ImRect rect(pos, pos + size);

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);

    if (pressed) MarkItemEdited(id);

    button_state* state = gui->anim_container(&state, id);
    state->background = ImLerp(state->background, IsItemActive() ? clr->c_other_clr.accent_clr : clr->c_element.layout, gui->fixed_speed(8.f));

    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max, gui->get_clr(state->background), SCALE(set->c_element.rounding));
    draw->render_text(set->c_font.inter_medium[0], window->DrawList, rect.Min, rect.Max, gui->get_clr(clr->c_text.text_active), label.data(), NULL, NULL, {0.5, 0.5}, NULL);

    return pressed;
}

bool c_widget::tool_button(std::string_view label, std::string_view icon, const ImVec2& size) {
    struct tool_button {
        bool clicked = false;
        float alpha = 0.f;
    };
    
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const ImVec2 pos = window->DC.CursorPos;
    ImRect rect(pos, pos + size);

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);
    
    tool_button* state = gui->anim_container(&state, id);

    if (pressed)
        state->clicked = true;
    
    state->alpha = ImClamp(state->alpha + (gui->fixed_speed(8.f) * (state->clicked ? 1.f : -1.f)), 0.f, 1.f);

    if (state->alpha >= 0.99f)
        state->clicked = false;
    
    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max, gui->get_clr(clr->c_lua.stroke), SCALE(2.f));
    draw->render_text(window->DrawList, set->c_font.inter_medium[1], rect.Min, rect.Max, gui->get_clr(clr->c_text.text), label.data(), NULL, NULL, ImVec2(0.5f, 0.5f));
    
    if (icon != "")
        draw->render_text(window->DrawList, set->c_font.icon[4], rect.Min, rect.Max, gui->get_clr(clr->c_text.text), "D", NULL, NULL, ImVec2(0.18f, 0.5f));
    
    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max, gui->get_clr(clr->c_other_clr.accent_clr, 1.f - state->alpha), SCALE(2.f));
    draw->render_text(window->DrawList, set->c_font.inter_medium[1], rect.Min, rect.Max, gui->get_clr(clr->c_text.text_active, 1.f - state->alpha), label.data(), NULL, NULL, ImVec2(icon != "" ? 0.7f : 0.5f, 0.5f));
    
    if (icon != "")
        draw->render_text(window->DrawList, set->c_font.icon[4], rect.Min, rect.Max, gui->get_clr(clr->c_text.text_active, 1.f - state->alpha), "D", NULL, NULL, ImVec2(0.18f, 0.5f));

    return pressed;
}
