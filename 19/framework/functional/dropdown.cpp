#include "../settings/functions.h"

static float calc_combo_size(int items_count, float item_size)
{
    ImGuiContext& g = *GImGui;
    if (items_count <= 0) return FLT_MAX;

    return item_size * items_count + g.Style.ItemSpacing.y * (items_count - 1);
}

static const char* items_array_getter(void* data, int idx)
{
    const char* const* items = (const char* const*)data;
    return items[idx];
}

bool selectable_ex(std::string_view label, bool active, ImDrawFlags rounded_flag)
{
    struct c_selectable
    {
        float alpha{ 0.f }, offset{ 0.f };
        ImVec4 rect{ clr->c_element.layout }, text{ clr->c_text.text };
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g{ *GImGui };
    const ImGuiStyle& style{ g.Style };
    const ImGuiID id{ window->GetID(label.data()) };

    const float width{ GetContentRegionAvail().x };
    float rect_size{ SCALE(30.f) };

    const ImVec2 pos{ window->DC.CursorPos };
    const ImRect rect(pos, pos + ImVec2(width, rect_size));

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered{ IsItemHovered() }, pressed{ hovered && g.IO.MouseClicked[0] };
    if (pressed) MarkItemEdited(id);

    c_selectable* state{ gui->anim_container(&state, id) };

    state->alpha = ImClamp(state->alpha + (gui->fixed_speed(6.f) * (active ? 1.f : -1.f)), 0.f, 1.f);
    state->text = ImLerp(state->text, active ? clr->c_other_clr.accent_clr : clr->c_text.text_active, gui->fixed_speed(set->c_element.rounding));

    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max, gui->get_clr(clr->c_element.dropdown_selection_layout, state->alpha), SCALE(set->c_element.rounding), rounded_flag);
    draw->render_text(set->c_font.inter_medium[0], window->DrawList, rect.Min + SCALE(7, 0), rect.Max, gui->get_clr(state->text), label.data(), NULL, NULL, { 0.0, 0.5 }, NULL);

    return pressed;
}

bool selectable(const char* label, bool* p_selected, ImDrawFlags rounded_flag)
{
    if (selectable_ex(label, *p_selected, rounded_flag))
    {
        *p_selected = !*p_selected;
        return true;
    }
    return false;
}

bool dropdown_list(std::string_view label, std::string_view preview_value, int val, int max_count, ImGuiComboFlags flags, bool multi)
{
    struct c_combo
    {
        ImVec4 text{ clr->c_text.text };
        bool combo_opened = false, hovered = false;
        float alpha{ 0.f }, offset{ 0.f };
        float text_size;
    };

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
    g.NextWindowData.ClearFlags();

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    if (val > max_count) val = max_count;
    const ImVec2 pos = window->DC.CursorPos;

    const float width = GetContentRegionAvail().x;
    const float rect_size = SCALE(30.f);

    const ImRect rect(pos, pos + ImVec2(width, rect_size));
    const ImRect combo(pos + ImVec2(0, SCALE(2)), pos + ImVec2(width, rect_size - SCALE(2)));

    ImRect open_combo(pos + ImVec2(width - SCALE(60), SCALE(2)), pos + ImVec2(width, rect_size - SCALE(2)));

    ItemSize(rect, 0.f);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);

    c_combo* state = gui->anim_container(&state, id);
    if (hovered && g.IO.MouseClicked[0] || state->combo_opened && g.IO.MouseClicked[0] && !state->hovered) state->combo_opened = !state->combo_opened;

    state->offset = ImLerp(state->offset, state->combo_opened ? SCALE(5.f) : SCALE(25.f), gui->fixed_speed(12.f) );
    state->alpha = ImClamp(state->alpha + (gui->fixed_speed(4.f) * (state->combo_opened ? 1.f : -1.f)), 0.f, 1.f);
    state->text = ImLerp(state->text, state->combo_opened ? clr->c_text.text_active : hovered ? clr->c_text.text_hov : clr->c_text.text, gui->fixed_speed(8.f));

    if (!IsRectVisible(rect.Min, rect.Max + ImVec2(0, 2)))
    {
        state->combo_opened = false;
        state->alpha = 0.f;
    }

    state->text_size = ImLerp(state->text_size, CalcTextSize(preview_value.data()).x < width - CalcTextSize(label.data()).x - 60 ? CalcTextSize(preview_value.data()).x : width - CalcTextSize(label.data()).x - 60, g.IO.DeltaTime * 8.f);
    open_combo.Min.x -= state->text_size;

    draw->add_rect_filled(window->DrawList, open_combo.Min, open_combo.Max, gui->get_clr(clr->c_element.layout), SCALE(set->c_element.rounding));
    draw->render_text(set->c_font.inter_medium[0], window->DrawList, open_combo.Min + SCALE(10, 0), open_combo.Max - ImVec2(rect_size, 0), gui->get_clr(clr->c_text.text_active), preview_value.data(), NULL, NULL, { 0.0, 0.5 }, NULL);

    draw->render_text(set->c_font.icon[0], window->DrawList, open_combo.Max - ImVec2(rect_size, rect_size), open_combo.Max, gui->get_clr(clr->c_text.text), "G", NULL, NULL, { 0.5, 0.60 }, NULL);
    draw->render_text(set->c_font.inter_medium[0], window->DrawList, rect.Min, rect.Max, gui->get_clr(state->text), label.data(), NULL, NULL, { 0.0, 0.5 }, NULL);

    if (!state->combo_opened && state->alpha < 0.01f) return false;

    gui->push_style_var(ImGuiStyleVar_Alpha, state->alpha);
    gui->push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    gui->push_style_var(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0 ));

    gui->push_style_var(ImGuiStyleVar_WindowRounding, set->c_element.rounding);
    gui->push_style_var(ImGuiStyleVar_WindowBorderSize, 0.f);

    gui->push_style_color(ImGuiCol_WindowBg, gui->get_clr(clr->c_element.layout));

    gui->set_next_window_pos(open_combo.GetBL() + ImVec2(0, state->offset));
    gui->set_next_window_size(ImVec2(open_combo.GetWidth(), calc_combo_size(val, rect_size)));

    gui->begin(label, NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
    {
        gui->begin_group();
        state->hovered = IsWindowHovered();

        if (!multi)
            if (IsWindowHovered() && g.IO.MouseClicked[0]) state->combo_opened = false;
    }
    return true;
}

void end_combo()
{
    gui->end_group();

    gui->pop_style_color(1);
    gui->pop_style_var(5);
    gui->end();
}

bool combo(std::string_view label, int* current_item, std::vector<std::string>& items, int popup_max_height_in_items, int max_count)
{
    ImGuiContext& g = *GImGui;

    const std::string* preview_value = nullptr;
    if (*current_item >= 0 && *current_item < static_cast<int>(items.size()))
        preview_value = &items[*current_item];

    if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
        SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, calc_combo_size(popup_max_height_in_items, 30)));

    if (!dropdown_list(label.data(), preview_value ? preview_value->c_str() : nullptr, static_cast<int>(items.size()), max_count, ImGuiComboFlags_None, false))
        return false;

    bool value_changed = false;
    for (int i = 0; i < static_cast<int>(items.size()); i++)
    {
        const std::string& item_text = items[i];
        PushID(i);
        const bool item_selected = (i == *current_item);
        if (selectable_ex(item_text.c_str(), item_selected, i == 0 ? ImDrawFlags_RoundCornersTop : i == items.size() - 1 ? ImDrawFlags_RoundCornersBottom : ImDrawFlags_RoundCornersNone) && *current_item != i)
        {
            value_changed = true;
            *current_item = i;
        }

        if (g.IO.MouseClicked[1])

            if (item_selected) SetItemDefaultFocus();
        PopID();
    }

    end_combo();

    if (value_changed) MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

bool c_widget::dropdown(std::string_view label, int* current_item, std::vector<std::string>& items, int max_count)
{
    const bool value_changed{ combo(label, current_item, items, -1, max_count) };
    return value_changed;
}

void c_widget::multi_dropdown(std::string_view label, bool variable[], std::vector<std::string>& labels, int max_count)
{
    ImGuiContext& g{ *GImGui };

    std::string preview{ "Select" };

    for (auto i = 0, j = 0; i < labels.size(); i++)
    {
        if (variable[i])
        {
            if (j)
                preview += (", ") + (std::string)labels[i];
            else
                preview = labels[i];

            j++;
        }
    }

    if (dropdown_list(label.data(), preview.c_str(), labels.size(), max_count, 0, true))
    {
        for (auto i = 0; i < labels.size(); i++) selectable(labels[i].data(), &variable[i], i == 0 ? ImDrawFlags_RoundCornersTop : i == labels.size() - 1 ? ImDrawFlags_RoundCornersBottom : ImDrawFlags_RoundCornersNone);

        end_combo();
    }

    preview = ("Select");
}

bool tool_dropdown_list(std::string_view label, std::string_view preview_value, int val, int max_count, ImGuiComboFlags flags, bool multi)
{
    struct c_combo
    {
        ImVec4 text{ clr->c_text.text };
        bool combo_opened = false, hovered = false;
        float alpha{ 0.f }, offset{ 0.f };
    };

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
    g.NextWindowData.ClearFlags();

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    if (val > max_count) val = max_count;
    const ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, pos + SCALE(130, 36));

    ItemSize(rect, 0.f);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);

    c_combo* state = gui->anim_container(&state, id);
    if (hovered && g.IO.MouseClicked[0] || state->combo_opened && g.IO.MouseClicked[0] && !state->hovered) state->combo_opened = !state->combo_opened;

    state->offset = ImLerp(state->offset, state->combo_opened ? SCALE(5.f) : SCALE(25.f), gui->fixed_speed(12.f) );
    state->alpha = ImClamp(state->alpha + (gui->fixed_speed(4.f) * (state->combo_opened ? 1.f : -1.f)), 0.f, 1.f);
    state->text = ImLerp(state->text, state->combo_opened ? clr->c_text.text_active : hovered ? clr->c_text.text_hov : clr->c_text.text, gui->fixed_speed(8.f));

    if (!IsRectVisible(rect.Min, rect.Max + ImVec2(0, 2)))
    {
        state->combo_opened = false;
        state->alpha = 0.f;
    }

    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max, gui->get_clr(clr->c_element.layout), SCALE(set->c_element.rounding));
    draw->render_text(set->c_font.inter_medium[0], window->DrawList, rect.Min + SCALE(10, 0), rect.Max - ImVec2(SCALE(36.f), 0), gui->get_clr(clr->c_text.text_active), preview_value.data(), NULL, NULL, { 0.0, 0.5 }, NULL);

    draw->render_text(set->c_font.icon[0], window->DrawList, rect.Max - ImVec2(SCALE(36.f), SCALE(36.f)), rect.Max, gui->get_clr(clr->c_text.text), "G", NULL, NULL, { 0.5, 0.50 }, NULL);

    if (!state->combo_opened && state->alpha < 0.01f) return false;

    gui->push_style_var(ImGuiStyleVar_Alpha, state->alpha);
    gui->push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    gui->push_style_var(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0 ));

    gui->push_style_var(ImGuiStyleVar_WindowRounding, set->c_element.rounding);
    gui->push_style_var(ImGuiStyleVar_WindowBorderSize, 0.f);

    gui->push_style_color(ImGuiCol_WindowBg, gui->get_clr(clr->c_element.layout));

    gui->set_next_window_pos(rect.GetBL() + ImVec2(0, state->offset));
    gui->set_next_window_size(ImVec2(rect.GetWidth(), calc_combo_size(val, SCALE(30.f))));

    gui->begin(label, NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
    {
        gui->begin_group();
        state->hovered = IsWindowHovered();

        if (!multi)
            if (IsWindowHovered() && g.IO.MouseClicked[0]) state->combo_opened = false;
    }
    return true;
}

bool tool_combo(std::string_view label, int* current_item, std::vector<std::string>& items, int popup_max_height_in_items, int max_count)
{
    ImGuiContext& g = *GImGui;

    const std::string* preview_value = nullptr;
    if (*current_item >= 0 && *current_item < static_cast<int>(items.size()))
        preview_value = &items[*current_item];

    if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
        SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, calc_combo_size(popup_max_height_in_items, 30)));

    if (!tool_dropdown_list(label.data(), preview_value ? preview_value->c_str() : nullptr, static_cast<int>(items.size()), max_count, ImGuiComboFlags_None, false))
        return false;

    bool value_changed = false;
    for (int i = 0; i < static_cast<int>(items.size()); i++)
    {
        const std::string& item_text = items[i];
        PushID(i);
        const bool item_selected = (i == *current_item);
        if (selectable_ex(item_text.c_str(), item_selected, i == 0 ? ImDrawFlags_RoundCornersTop : i == items.size() - 1 ? ImDrawFlags_RoundCornersBottom : ImDrawFlags_RoundCornersNone) && *current_item != i)
        {
            value_changed = true;
            *current_item = i;
        }

        if (g.IO.MouseClicked[1])

            if (item_selected) SetItemDefaultFocus();
        PopID();
    }

    end_combo();

    if (value_changed) MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

bool c_widget::tool_dropdown(std::string_view label, int* current_item, std::vector<std::string>& items, int max_count)
{
    const bool value_changed{ tool_combo(label, current_item, items, -1, max_count) };
    return value_changed;
}