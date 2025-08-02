#include "../settings/functions.h"

void c_notify::add_notify(std::string_view text, float notify_delay, notify_position type)
{
    notifications.push_back({ notify_count++, text, notify_delay, 0.f, 0.f, 0.f, true, type });
}

void c_notify::setup_notify()
{
    const float speed = 4.f;

    int cur_notify_value = 0;
    for (int i = 0; i < notifications.size(); ++i)
    {
        auto& notification = notifications[i];

        if (notification.active_notify)
            notification.notify_timer += speed * ImGui::GetIO().DeltaTime;

        if (notification.notify_timer >= notification.notify_delay) notification.active_notify = false;

        notification.notify_alpha = ImClamp(notification.notify_alpha + (gui->fixed_speed(4.f) * (notification.active_notify ? 1.f : -1.f)), 0.f, 1.f);

        notification.notify_offset = easing->im_ease(i, notification.active_notify, -(CalcTextSize(notification.text.data()).x + SCALE(120)), SCALE(20.f), gui->fixed_speed(14.f), back);

        if (notification.notify_alpha <= 0.f && !notification.active_notify) cur_notify_value--;

        render_notify(cur_notify_value, notification.notify_alpha, notification.notify_offset, notification.notify_timer, notification.notify_delay, notification.text, notification.type);
        cur_notify_value++;
    }
}

void c_notify::render_notify(int notification_index, float notification_alpha, float notification_offset, float notification_duration, float notification_delay, std::string_view notification_text, notify_position notification_position)
{
    float font_width = set->c_font.inter_medium[1]->CalcTextSizeA(set->c_font.inter_medium[1]->FontSize, FLT_MAX, -1, notification_text.data()).x;
    float percent_remaining = (notification_duration / notification_delay) * 100;

    int padding_x = SCALE(20);
    int padding_y = SCALE(80);

    if (percent_remaining < 20) notification_alpha *= percent_remaining / 20;

    gui->set_next_window_size(ImVec2(padding_x + font_width, SCALE(60)));

    ImVec2 position;
    switch (notification_position)
    {
    case top_left:
        position = ImVec2(notification_offset, padding_x + notification_index * padding_y);
        break;
    case top_right:
        position = ImVec2(GetIO().DisplaySize.x - ((font_width + padding_x) + notification_offset), padding_x + notification_index * padding_y);
        break;
    case bottom_left:
        position = ImVec2(notification_offset, (GetIO().DisplaySize.y - padding_y) - notification_index * padding_y);
        break;
    case bottom_right:
        position = ImVec2(GetIO().DisplaySize.x - ((font_width + padding_x) + notification_offset), (GetIO().DisplaySize.y - padding_y) - notification_index * padding_y);
        break;
    }

    static float notification_y_offset[250] = { 0.0f };
    notification_y_offset[notification_index] = ImLerp(notification_y_offset[notification_index], (percent_remaining < 20) ? (1.0f - (percent_remaining / 20.0f)) * padding_y : 0.0f, gui->fixed_speed(15.0f));
    position.y += notification_y_offset[notification_index];

    gui->set_next_window_pos(position);

    gui->push_style_var(ImGuiStyleVar_Alpha, notification_alpha);

    gui->begin((std::stringstream() << "notify" << notification_index).str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
    {
        const ImVec2 position = ImGui::GetWindowPos();
        const ImVec2 content_region = ImGui::GetContentRegionMax();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImGuiStyle* gui_style = &ImGui::GetStyle();

        { // style
            gui_style->WindowPadding = { 0, 0 };
            gui_style->ItemSpacing = SCALE(20, 20);
            gui_style->WindowBorderSize = { 0 };
            gui_style->WindowRounding = SCALE(4.f);
        }

        { // decorations
            draw->add_rect_filled(draw_list, { position.x, position.y }, position + content_region, gui->get_clr(clr->c_window.layout), gui_style->WindowRounding);
            draw->render_text(draw_list, set->c_font.inter_medium[1], { position.x + SCALE(10), position.y }, { position.x + CalcTextSize(notification_text.data()).x + SCALE(60), position.y + SCALE(40) }, gui->get_clr(clr->c_text.text_active), notification_text.data(), NULL, NULL, { 0.0, 0.5 }, NULL);

            draw->add_rect_filled(draw_list, { position.x + SCALE(10), position.y + (content_region.y - SCALE(15)) }, { position.x + content_region.x - SCALE(10), position.y + (content_region.y - SCALE(10)) }, gui->get_clr(clr->c_element.layout), SCALE((10.f)));
            draw->add_rect_filled(draw_list, { position.x + SCALE(10), position.y + (content_region.y - SCALE(15)) }, { position.x + SCALE(20) + (content_region.x - SCALE(30)) * percent_remaining / 100, position.y + (content_region.y - SCALE(10)) }, gui->get_clr(clr->c_other_clr.accent_clr), SCALE(10.f));
        }
    }
    gui->end();

    gui->pop_style_var();
}