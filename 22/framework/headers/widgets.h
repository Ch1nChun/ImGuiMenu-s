#pragma once
#include "includes.h"

#define IMGUI_DEFINE_MATH_OPERATORS

struct custom_slider_t
{
    bool held;
    c_rect rect;
    c_rect active;
    c_vec2 circle_pos;
};

class c_widgets
{
public:
    bool tab_button(std::string name, std::string icon, int tab);
    bool category_button(std::string name, std::string description, bool* callback);
    bool child(std::string name);
    void end_child();
    bool checkbox(std::string name, std::string description, bool* callback);
    bool dropdown(std::string name, std::string description, int* callback, std::vector<std::string> variants);
    bool slider(std::string name, std::string description, float* callback, float vmin, float vmax, std::string format = "%.1f");
    custom_slider_t custom_slider(std::string name, float* callback, float vmin, float vmax, float width);
    bool color_picker(std::string name, c_vec4* color);
    bool text_field(std::string name, char* buf, int buf_size);
};

inline std::unique_ptr<c_widgets> widgets = std::make_unique<c_widgets>();

enum notify_type
{
    success = 0,
    warning = 1,
    error = 2
};

struct notify_state
{
    int notify_id;
    std::string_view text;
    notify_type type{ success };

    ImVec2 window_size{ 0, 0 };
    float notify_alpha{ 0 };
    bool active_notify{ true };
    float notify_timer{ 0 };
    float notify_pos{ 0 };
};

class c_notify
{
public:
    void setup_notify();

    void add_notify(std::string_view text, notify_type type);

private:
    ImVec2 render_notify(int cur_notify_value, float notify_alpha, float notify_percentage, float notify_pos, std::string_view text, notify_type type);

    float notify_time{ 15 };
    int notify_count{ 0 };

    float notify_spacing{ 20 };
    ImVec2 notify_padding{ 20, 20 };

    std::vector<notify_state> notifications;

};

inline std::unique_ptr<c_notify> notify = std::make_unique<c_notify>();
