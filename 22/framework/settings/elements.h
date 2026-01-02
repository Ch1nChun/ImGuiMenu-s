#pragma once
#include <string>
#include "imgui.h"

class c_elements
{
public:

    c_vec2 padding{ 10, 10 };

    float child_width;
    float tab_window_width;

    struct 
    {
        c_text name{ "Toronto" };
        c_vec2 size{ 334, 60 };
        float rounding{ 3 };
    } window;

    
};

inline std::unique_ptr<c_elements> elements = std::make_unique<c_elements>();
