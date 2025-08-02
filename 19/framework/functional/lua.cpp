#include "../settings/functions.h"

bool load_button(std::string_view label, bool* callback) {
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const ImVec2 pos = window->DC.CursorPos;
    ImRect rect(pos, pos + SCALE(100, 40));

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered = IsItemHovered();
    bool clicked = hovered && g.IO.MouseClicked[0];

    if (clicked)
        *callback = !*callback;
    
    float* state = gui->anim_container(&state, id);
    *state = ImClamp(*state + (gui->fixed_speed(8.f) * (!*callback ? 1.f : -1.f)), 0.f, 1.f);

    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max, gui->get_clr(clr->c_lua.stroke), SCALE(2.f));
    draw->render_text(window->DrawList, set->c_font.inter_medium[1], rect.Min, rect.Max, gui->get_clr(clr->c_text.text), "UNLOAD", NULL, NULL, ImVec2(0.5f, 0.5f));

    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max, gui->get_clr(clr->c_other_clr.accent_clr, *state), SCALE(2.f));
    draw->render_text(window->DrawList, set->c_font.inter_medium[1], rect.Min, rect.Max, gui->get_clr(clr->c_text.text_active, *state), "LOAD", NULL, NULL, ImVec2(0.5f, 0.5f));
    
    return clicked;
}

bool c_widget::lua_tool_button(std::string_view icon, std::string_view aid)
{
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID((std::stringstream{} << icon << aid).str().c_str());

    const ImVec2 pos = window->DC.CursorPos;
    ImRect rect(pos, pos + SCALE(15, 15));

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(rect, id, &hovered, &held);
    
    ImVec4* state = gui->anim_container(&state, id);
    *state = ImLerp(*state, hovered ? clr->c_text.text_active : clr->c_text.text_hov, gui->fixed_speed(12.f));
    
    draw->render_text(window->DrawList, set->c_font.icon[3], rect.Min, rect.Max, gui->get_clr(*state), icon.data(), NULL, NULL, ImVec2(0.5f, 0.5f));
    
    return pressed;
}

bool c_widget::lua_selectable(lua_data* data, int lua_id)
{
    
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID((std::stringstream{} << data->name.data() << lua_id).str().c_str());

    const ImVec2 pos = window->DC.CursorPos;
    const float width = GetContentRegionAvail().x;
    ImRect rect(pos, pos + ImVec2(width, SCALE(60)));

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;
    
    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max, gui->get_clr(clr->c_lua.background), SCALE(set->c_child.rounding));
    draw->add_rect(window->DrawList, rect.Min, rect.Max, gui->get_clr(clr->c_lua.stroke), SCALE(set->c_child.rounding), 0, SCALE(1.f));
    draw->render_text(window->DrawList, set->c_font.inter_medium[1], rect.Min + SCALE(10, 11), rect.Max, gui->get_clr(clr->c_text.text_active), data->name.data());
    draw->render_text(window->DrawList, set->c_font.inter_medium[0], rect.Min + SCALE(10, 33), rect.Max, gui->get_clr(clr->c_text.text_active), "Modified:");
    draw->render_text(window->DrawList, set->c_font.inter_medium[0], rect.Min + SCALE(69, 33), rect.Max, gui->get_clr(clr->c_other_clr.accent_clr), data->date.data());

    const ImVec2 stored_cursor_pos = GetCursorScreenPos();

    SetCursorScreenPos(ImVec2(rect.Max.x - SCALE(110), rect.Min.y + SCALE(10)));
    load_button((std::stringstream{} << data->name << id).str(), &data->loaded);

    SetCursorScreenPos(ImVec2(rect.Max.x - SCALE(146), rect.Min.y + SCALE(23)));
    bool pressed = lua_tool_button("B", std::to_string(id));

    SetCursorScreenPos(ImVec2(rect.Max.x - SCALE(180), rect.Min.y + SCALE(23)));
    if (lua_tool_button("C", std::to_string(id)))
        var->c_lua.data.erase(var->c_lua.data.begin() + lua_id);
    
    SetCursorScreenPos(stored_cursor_pos);
    
    return pressed;
}
