#include "../settings/functions.h"

void esp_preview::fill_box(const ImVec2& pos, const ImVec2& size)
{
    box.position = pos;
    box.size = size;
}

void esp_preview::fill_areas()
{
    area_rect[area_top] = ImRect(box.position - ImVec2(0, area_size), box.position + ImVec2(box.size.x, 0));
    area_rect[area_bottom] = ImRect(box.position + ImVec2(0, box.size.y), box.position + box.size + ImVec2(0, area_size));
    area_rect[area_left] = ImRect(box.position - ImVec2(area_size, 0), box.position + ImVec2(0, box.size.y));
    area_rect[area_right] = ImRect(box.position + ImVec2(box.size.x, 0), box.position + box.size + ImVec2(area_size, 0));

    hovered_area = area_none;

    for (int i = 0; i < IM_ARRAYSIZE(area_rect); i++)
    {
        if (ImGui::IsMouseHoveringRect(area_rect[i].Min, area_rect[i].Max)) hovered_area = i;
    }

}

void esp_preview::fill_buffers()
{
    std::fill(std::begin(bars_buffer), std::end(bars_buffer), 0);
    std::fill(std::begin(texts_buffer), std::end(texts_buffer), 0);

}

void esp_preview::initialize_preview(const ImVec2& pos, const ImVec2& size, bool render_text, bool render_bar)
{
    fill_box(pos, size);
    fill_areas();
    fill_buffers();

    esp->render_box(0);

    if (render_bar)
    for (int i = 0; i < esp->bar.size(); i++) esp->render_bar(esp->bar.at(i));

    if (render_text)
    for (int i = 0; i < esp->text.size(); i++) esp->render_text(esp->text.at(i));

}

void esp_preview::render_box(int type)
{
    const ImVec2 box_pos = box.position;
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    draw->add_line(GetWindowDrawList(), box.position, box.position + ImVec2(0, box.size.y / 3), gui->get_clr(clr->c_other_clr.white_clr));
    draw->add_line(GetWindowDrawList(), box.position, box.position + ImVec2(box.size.x / 3, 0), gui->get_clr(clr->c_other_clr.white_clr));

    draw->add_line(GetWindowDrawList(), box.position + ImVec2(box.size.x * 2 / 3, 0), box.position + ImVec2(box.size.x, 0), gui->get_clr(clr->c_other_clr.white_clr));
    draw->add_line(GetWindowDrawList(), box.position + ImVec2(box.size.x, 0), box.position + ImVec2(box.size.x, box.size.y / 3), gui->get_clr(clr->c_other_clr.white_clr));

    draw->add_line(GetWindowDrawList(), box.position + ImVec2(box.size.x, box.size.y * 2 / 3), box.position + box.size, gui->get_clr(clr->c_other_clr.white_clr));
    draw->add_line(GetWindowDrawList(), box.position + box.size, box.position + box.size - ImVec2(box.size.x / 3, 0), gui->get_clr(clr->c_other_clr.white_clr));

    draw->add_line(GetWindowDrawList(), box.position + ImVec2(box.size.x / 3, box.size.y), ImVec2(box.position.x, box.position.y + box.size.y), gui->get_clr(clr->c_other_clr.white_clr));
    draw->add_line(GetWindowDrawList(), ImVec2(box.position.x, box.position.y + box.size.y), box.position + ImVec2(0, box.size.y * 2 / 3), gui->get_clr(clr->c_other_clr.white_clr));
}

void esp_preview::render_text(item_state& state)
{
    const ImVec2 text_size = ImGui::CalcTextSize(state.name.c_str());
    const ImVec2 box_pos = box.position - ImGui::GetWindowPos();
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    ImVec2 pos_offset[]
    {
        ImVec2(0.f, texts_buffer[state.position] * -var->c_preview.texts_spacing - bars_buffer[state.position] * var->c_preview.bars_spacing),
        ImVec2(0.f, texts_buffer[state.position] * var->c_preview.texts_spacing + bars_buffer[state.position] * var->c_preview.bars_spacing),
        ImVec2(bars_buffer[state.position] * -var->c_preview.bars_spacing, texts_buffer[state.position] * var->c_preview.texts_spacing),
        ImVec2(bars_buffer[state.position] * var->c_preview.bars_spacing, texts_buffer[state.position] * var->c_preview.texts_spacing),
    };

    texts_buffer[state.position]++;

    ImVec2 positions[]
    {
        ImVec2(box_pos.x + (box.size.x / 2) - text_size.x / 2, box_pos.y - text_size.y - var->c_preview.box_padding) + pos_offset[position_top],
        ImVec2(box_pos.x + (box.size.x / 2) - text_size.x / 2, box_pos.y + box.size.y + var->c_preview.box_padding) + pos_offset[position_bottom],
        ImVec2(box_pos.x - text_size.x - var->c_preview.box_padding, box_pos.y) + pos_offset[position_left],
        ImVec2(box_pos.x + box.size.x + var->c_preview.box_padding, box_pos.y) + pos_offset[position_right],
    };

    state.location.x = ImLerp(state.location.x, positions[state.position].x, ImGui::GetIO().DeltaTime * var->c_preview.anim_speed);
    state.location.y = ImLerp(state.location.y, positions[state.position].y, ImGui::GetIO().DeltaTime * var->c_preview.anim_speed);

    if (state.swapped) {
        if (abs(state.location.x - positions[state.position].x) < 1.f && abs(state.location.y - positions[state.position].y) < 1.f) {
            state.swapped = false;
        }
    }

    ImRect rect = ImRect(ImGui::GetWindowPos() + state.location, ImGui::GetWindowPos() + state.location + ImGui::CalcTextSize(state.name.c_str()));
    state.rect = rect;

    if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max) && ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
        state.active = true;

    if (state.active)
    {
        draw->add_text(window->DrawList, 0, 0, ImGui::GetMousePos() - ImVec2(text_size.x / 2, text_size.y / 2), ImGui::GetColorU32(state.color), state.name.c_str());

        for (auto& stored : text) {
            if (stored.name == state.name || stored.position != state.position || stored.swapped)
                continue;

            if (ImGui::IsMouseHoveringRect(stored.rect.Min, stored.rect.Max))
            {
                std::swap(state, stored);

                state.active = false;
                state.swapped = true;

                stored.active = true;
                stored.swapped = true;
            }
        }

        if (hovered_area != area_none)
        {
            state.position = hovered_area;
        }

        if (ImGui::IsMouseReleased(0))
        {
            state.active = false;
        }
    }

    draw->add_text(window->DrawList, 0, 0, rect.Min, ImGui::GetColorU32(state.color), state.name.c_str());

}

void esp_preview::render_bar(item_state& state)
{
    const float bar_size = 3.f;
    const ImVec2 box_pos = box.position - ImGui::GetWindowPos();
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    ImVec2 pos_offset[]
    {
        ImVec2(0.f, bars_buffer[state.position] * -var->c_preview.bars_spacing),
        ImVec2(0.f, bars_buffer[state.position] * var->c_preview.bars_spacing),
        ImVec2(bars_buffer[state.position] * -var->c_preview.bars_spacing, 0.f),
        ImVec2(bars_buffer[state.position] * var->c_preview.bars_spacing, 0.f),
    };

    bars_buffer[state.position]++;

    ImVec2 positions[]
    {
        ImVec2(box_pos.x, box_pos.y - var->c_preview.box_padding - bar_size) + pos_offset[position_top],
        ImVec2(box_pos.x, box_pos.y + box.size.y + var->c_preview.box_padding) + pos_offset[position_bottom],
        ImVec2(box_pos.x - var->c_preview.box_padding - bar_size, box_pos.y) + pos_offset[position_left],
        ImVec2(box_pos.x + box.size.x + var->c_preview.box_padding, box_pos.y) + pos_offset[position_right],
    };

    ImVec2 bar_sizes[]
    {
        ImVec2(box.size.x, bar_size),
        ImVec2(box.size.x, bar_size),
        ImVec2(bar_size, box.size.y),
        ImVec2(bar_size, box.size.y),
    };

    state.location.x = ImLerp(state.location.x, positions[state.position].x, ImGui::GetIO().DeltaTime * var->c_preview.anim_speed);
    state.location.y = ImLerp(state.location.y, positions[state.position].y, ImGui::GetIO().DeltaTime * var->c_preview.anim_speed);

    if (state.swapped) {
        if (abs(state.location.x - positions[state.position].x) < 1.f && abs(state.location.y - positions[state.position].y) < 1.f) {
            state.swapped = false;
        }
    }

    ImRect rect = ImRect(ImGui::GetWindowPos() + state.location, ImGui::GetWindowPos() + state.location + bar_sizes[state.position]);
    state.rect = rect;

    if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max) && ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
        state.active = true;

    if (state.active)
    {
        draw->add_rect_filled(window->DrawList, ImGui::GetMousePos() - ImVec2(rect.GetWidth() / 2, rect.GetHeight() / 2), ImGui::GetMousePos() + ImVec2(rect.GetWidth() / 2, rect.GetHeight() / 2), ImGui::GetColorU32(state.color));

        for (auto& stored : bar) {
            if (stored.name == state.name || stored.position != state.position || stored.swapped) continue;

            if (ImGui::IsMouseHoveringRect(stored.rect.Min, stored.rect.Max))
            {
                std::swap(state, stored);

                state.active = false;
                state.swapped = true;

                stored.active = true;
                stored.swapped = true;
            }
        }

        if (hovered_area != area_none)
        {
            state.position = hovered_area;
        }

        if (ImGui::IsMouseReleased(0))
        {
            state.active = false;
        }
    }

    draw->add_rect_filled(window->DrawList, rect.Min, rect.Max, ImGui::GetColorU32(state.color));

}
