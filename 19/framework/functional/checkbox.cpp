#include "../settings/functions.h"

bool c_widget::checkbox(std::string_view label, bool* callback)
{
    struct c_checkbox
    {
        ImVec4 background = clr->c_element.layout;

        ImVec4 text_colored = clr->c_text.text;
        ImVec4 circle = clr->c_element.circle;

        float circle_offset = 0;
    };

    ImGuiWindow* window = GetCurrentWindow();
    ImGuiContext& g = *GImGui;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const ImVec2 label_size = CalcTextSize(label.data(), NULL, true);
    const ImVec2 pos = window->DC.CursorPos;

    const float w = GetContentRegionAvail().x;
    const float h = SCALE(30);

    const ImRect clickable_rect(pos, pos + ImVec2(w, h));
    const ImRect rect(pos + ImVec2(w - SCALE(40), SCALE(4)), pos + ImVec2(w, h - SCALE(4)));

    ItemSize(clickable_rect, 0.f);
    if (!ItemAdd(clickable_rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(clickable_rect, id, &hovered, &held);

    c_checkbox* state = gui->anim_container(&state, id);
    state->text_colored = ImLerp(state->text_colored, *callback ? clr->c_text.text_active : hovered ? clr->c_text.text_hov : clr->c_text.text, ImGui::GetIO().DeltaTime * 8.f);
    state->background = ImLerp(state->background, *callback ? clr->c_element.checkbox_active : clr->c_element.layout, ImGui::GetIO().DeltaTime * 8.f);

    state->circle = ImLerp(state->circle, *callback ? clr->c_other_clr.accent_clr : clr->c_element.circle, ImGui::GetIO().DeltaTime * 8.f);
    state->circle_offset = ImLerp(state->circle_offset, *callback ? 28.f : 12.f, gui->fixed_speed(30.f));

    if (pressed)
    {
        *callback = !(*callback);
        MarkItemEdited(id);
    }

    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max, gui->get_clr(state->background), SCALE(100.f));
    draw->add_circle_filled(window->DrawList, { rect.Min.x + SCALE(state->circle_offset), rect.GetCenter().y }, SCALE(6.f), gui->get_clr(state->circle), SCALE(100.f));

    draw->render_text(window->DrawList, set->c_font.inter_medium[0], clickable_rect.Min, clickable_rect.Max, gui->get_clr(state->text_colored), label.data(), NULL, NULL, { 0.0, 0.5 });

    return pressed;
}

bool c_widget::checkbox_with_key(std::string_view label, bool* callback, int* key, bool* mode, bool* value, bool* show_in_binds)
{
    struct c_checkbox
    {
        ImVec4 background = clr->c_element.layout;

        ImVec4 text_colored = clr->c_text.text;
        ImVec4 circle = clr->c_element.circle;

        float circle_offset = 0;
    };

    ImGuiWindow* window = GetCurrentWindow();
    ImGuiContext& g = *GImGui;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const ImVec2 label_size = CalcTextSize(label.data(), NULL, true);
    const ImVec2 pos = window->DC.CursorPos;

    const float w = GetContentRegionAvail().x;
    const float h = SCALE(30);

    const ImRect clickable_rect(pos, pos + ImVec2(w, h));
    const ImRect rect(pos + ImVec2(w - SCALE(40), SCALE(4)), pos + ImVec2(w, h - SCALE(4)));

    ItemSize(clickable_rect, 0.f);
    if (!ItemAdd(clickable_rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(clickable_rect, id, &hovered, &held);

    c_checkbox* state = gui->anim_container(&state, id);
    state->text_colored = ImLerp(state->text_colored, *callback ? clr->c_text.text_active : hovered ? clr->c_text.text_hov : clr->c_text.text, ImGui::GetIO().DeltaTime * 8.f);
    state->background = ImLerp(state->background, *callback ? clr->c_element.checkbox_active : clr->c_element.layout, ImGui::GetIO().DeltaTime * 8.f);

    state->circle = ImLerp(state->circle, *callback ? clr->c_other_clr.accent_clr : clr->c_element.circle, ImGui::GetIO().DeltaTime * 8.f);
    state->circle_offset = ImLerp(state->circle_offset, *callback ? 28.f : 12.f, gui->fixed_speed(30.f));

    widget->begin_popup((std::stringstream{} << id << "keybind").str().c_str(), 170, {w - SCALE(92), SCALE(-40)});
    {

        widget->keybind("keybind", key);

        widget->separator();

        widget->keybind_button("mode", mode);

        widget->separator();

        widget->checkbox("Value", value);

        widget->separator();

        widget->checkbox("Show binds", show_in_binds);

    }
    widget->end_popup();

    if (pressed)
    {
        *callback = !(*callback);
        MarkItemEdited(id);
    }

    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max, gui->get_clr(state->background), SCALE(100.f));
    draw->add_circle_filled(window->DrawList, { rect.Min.x + SCALE(state->circle_offset), rect.GetCenter().y }, SCALE(6.f), gui->get_clr(state->circle), SCALE(100.f));

    draw->render_text(window->DrawList, set->c_font.icon[1], clickable_rect.Min, clickable_rect.Max - ImVec2(SCALE(60), 0), gui->get_clr(clr->c_element.popup_icon), "E", NULL, NULL, {1.0, 0.5});
    draw->render_text(window->DrawList, set->c_font.inter_medium[0], clickable_rect.Min, clickable_rect.Max, gui->get_clr(state->text_colored), label.data(), NULL, NULL, { 0.0, 0.5 });

    return pressed;
}

bool c_widget::checkbox_with_color(std::string_view label, bool* callback, float col[4], bool alpha)
{
    struct c_checkbox
    {
        ImVec4 background = clr->c_element.layout;

        ImVec4 text_colored = clr->c_text.text;
        ImVec4 circle = clr->c_element.circle;

        float circle_offset = 0;
    };

    ImGuiWindow* window = GetCurrentWindow();
    ImGuiContext& g = *GImGui;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const ImVec2 label_size = CalcTextSize(label.data(), NULL, true);
    const ImVec2 pos = window->DC.CursorPos;

    const float w = GetContentRegionAvail().x;
    const float h = SCALE(30);

    const ImRect clickable_rect(pos, pos + ImVec2(w, h));
    const ImRect rect(pos + ImVec2(w - SCALE(40), SCALE(4)), pos + ImVec2(w, h - SCALE(4)));

    ItemSize(clickable_rect, 0.f);
    if (!ItemAdd(clickable_rect, id)) return false;
    const ImVec2 stored_pos = GetCursorScreenPos();

    bool hovered, held, pressed = ButtonBehavior(clickable_rect, id, &hovered, &held);

    c_checkbox* state = gui->anim_container(&state, id);
    state->text_colored = ImLerp(state->text_colored, *callback ? clr->c_text.text_active : hovered ? clr->c_text.text_hov : clr->c_text.text, ImGui::GetIO().DeltaTime * 8.f);
    state->background = ImLerp(state->background, *callback ? clr->c_element.checkbox_active : clr->c_element.layout, ImGui::GetIO().DeltaTime * 8.f);

    state->circle = ImLerp(state->circle, *callback ? clr->c_other_clr.accent_clr : clr->c_element.circle, ImGui::GetIO().DeltaTime * 8.f);
    state->circle_offset = ImLerp(state->circle_offset, *callback ? 28.f : 12.f, gui->fixed_speed(30.f));

    SetCursorScreenPos(clickable_rect.Max - SCALE(78, 24));
    widget->color_edit((std::stringstream{} << id << "colorpicker").str(), col, alpha ? ImGuiColorEditFlags_AlphaBar : 0);
    SetCursorScreenPos(stored_pos);

    if (pressed)
    {
        *callback = !(*callback);
        MarkItemEdited(id);
    }

    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max, gui->get_clr(state->background), SCALE(100.f));
    draw->add_circle_filled(window->DrawList, { rect.Min.x + SCALE(state->circle_offset), rect.GetCenter().y }, SCALE(6.f), gui->get_clr(state->circle), SCALE(100.f));

    draw->render_text(window->DrawList, set->c_font.inter_medium[0], clickable_rect.Min, clickable_rect.Max, gui->get_clr(state->text_colored), label.data(), NULL, NULL, { 0.0, 0.5 });

    return pressed;
}
