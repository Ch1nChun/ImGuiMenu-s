#pragma once
#include <string>
#include <vector>
#include "imgui.h"
#include "../headers/flags.h"


class c_variables
{
public:

	struct
	{
		float dpi = 1.f;
		int stored_dpi = 100;
		bool dpi_changed = true;

		int tab;
		int tab_stored;
		float tab_alpha;
	} gui;

	gui_style style;

};

inline std::unique_ptr<c_variables> var = std::make_unique<c_variables>();
