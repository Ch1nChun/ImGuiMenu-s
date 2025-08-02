#include "../settings/functions.h"

bool c_widget::selection(std::string_view label, const ImVec2& size, int selection_id, int& selection_variable)
{
    struct selection_state
    {
        ImVec4 background;
        ImVec4 text;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const bool selected = selection_id == selection_variable;
    const float label_size = CalcTextSize(label.data(), NULL, true).x;

    const ImVec2 pos = window->DC.CursorPos;
    ImRect rect(pos, pos + SCALE(size));

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);
    if (pressed) selection_variable = selection_id;

    selection_state* state = gui->anim_container(&state, id);

    state->background = ImLerp(state->background, selected ? clr->c_element.dropdown_selection_layout : clr->c_element.layout, gui->fixed_speed(12.f));
    state->text = ImLerp(state->text, selected ? clr->c_other_clr.accent_clr : clr->c_text.text, gui->fixed_speed(12.f));
    
    draw->render_text(window->DrawList, set->c_font.icon[6], rect.Min - SCALE(12, 12), rect.Max + SCALE(12, 12), gui->get_clr(clr->c_child.layout), "A", NULL, NULL, ImVec2(0.5f, 0.5f));
    draw->render_text(window->DrawList, set->c_font.icon[5], rect.Min - SCALE(1, 0), rect.Max - SCALE(1, 0), gui->get_clr(state->background), "A", NULL, NULL, ImVec2(0.5f, 0.5f));
    draw->render_text(window->DrawList, set->c_font.icon[1], rect.Min, rect.Max, gui->get_clr(state->text), label.data(), 0, 0, { 0.5, 0.5 });

    return pressed;
}
