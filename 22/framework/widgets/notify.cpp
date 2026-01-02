#include "../headers/functions.h"
#include "../headers/widgets.h"

void c_notify::add_notify(std::string_view text,  notify_type type)
{
    notifications.push_back({ notify_count++, text, type });
}

void c_notify::setup_notify()
{
    int cur_notify_value = 0;
    float accumulated_height = 0.f;

    for (auto& notification : notifications)
    {
        cur_notify_value++;
        if (notification.active_notify)
            notification.notify_timer += gui->fixed_speed(4.f);

        if (notification.notify_timer >= notify_time)
            notification.active_notify = false;

        gui->easing(notification.notify_alpha, notification.active_notify ? 1.f : 0.f, 4.f, static_easing);

        if (notification.notify_alpha > 0.f)
        {
            float target_position = accumulated_height + notify_padding.y;
            gui->easing(notification.notify_pos, target_position, 8.f, dynamic_easing);

            ImVec2 window_size = render_notify(cur_notify_value, notification.notify_alpha, notification.notify_timer, notification.notify_pos, notification.text, notification.type);

            accumulated_height += window_size.y + notify_spacing;
        }
    }
}

ImVec2 c_notify::render_notify(int cur_notify_value, float notify_alpha, float notify_percentage, float notify_pos, std::string_view text, notify_type type)
{
    ImVec2 window_size;

    SetNextWindowPos(ImVec2(notify_padding.x, notify_pos));
    gui->push_var(ImGuiStyleVar_Alpha, notify_alpha);
    gui->begin("notify_" + std::to_string(cur_notify_value), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize);
    {
        Text(text.data());

        window_size = gui->window_size();
    }
    gui->end();
    gui->pop_var();

    return window_size;
}