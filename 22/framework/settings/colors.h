#pragma once
#include "../headers/includes.h"
#include "../headers/flags.h"
#include <memory>

class c_colors
{
public:
    c_col layout{ 25, 25, 28 };
    c_col white{ 255, 255, 255 };
    c_col black{ 0, 0, 0 };
    c_col accent{ 176, 180, 255 };
    c_col child{ 28, 28, 33 };
    c_col widget{ 33, 33, 40 };
    c_col text{ 110, 110, 129 };
    c_col cirlce{ 50, 50, 63 };
    c_col border{ 35, 35, 44 };
};

inline std::unique_ptr<c_colors> clr = std::make_unique<c_colors>();
