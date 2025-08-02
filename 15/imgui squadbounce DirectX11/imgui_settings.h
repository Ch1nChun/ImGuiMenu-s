#include "imgui.h"

namespace font
{
	inline ImFont* inter_medium_widget = nullptr;
	inline ImFont* inter_medium_small = nullptr;
	inline ImFont* inter_medium_big = nullptr;
	inline ImFont* inter_bold = nullptr;
	inline ImFont* icomoon = nullptr;
	inline ImFont* icomoon_default = nullptr;
}

namespace c
{

	inline ImVec4 accent = ImColor(101, 87, 255, 255);
	inline ImVec4 black = ImColor(0, 0, 0, 255);

	inline ImVec4 green = ImColor(0, 255, 0, 255);
	inline ImVec4 red = ImColor(255, 0, 0, 255);

	namespace bg
	{
		inline ImVec4 filling = ImColor(6, 7, 20, 255);
		inline ImVec4 stroke = ImColor(19, 21, 32, 255);
		inline ImVec4 topbar = ImColor(7, 7, 11, 255);
		inline ImVec4 container = ImColor(7, 8, 10, 255);
		inline ImVec2 size = ImVec2(770, 430);
		inline float rounding = 12;
	}

	namespace child
	{
		inline ImVec4 filling = ImColor(6, 9, 10, 100);
		inline ImVec4 stroke = ImColor(18, 20, 34, 255);
		inline ImVec4 top = ImColor(9, 10, 15, 255);
		inline ImVec4 child_text = ImColor(33, 37, 62, 255);

		inline float rounding = 8;
	}

	namespace element
	{
		namespace info_bar
		{
			inline ImVec4 filling = ImColor(0, 0, 0, 255);
			inline ImVec4 stroke = ImColor(14, 15, 22, 255);

			inline ImVec4 box = ImColor(8, 9, 12, 255);
			inline ImVec4 box_outline = ImColor(12, 12, 20, 255);
		}

		inline ImVec4 page_active = ImColor(108, 107, 130, 30);
		inline ImVec4 selectable = ImColor(18, 20, 34, 255);

		namespace popup_elements
		{
			inline ImVec4 filling = ImColor(10, 10, 10, 170);
			inline ImVec4 cog = ImColor(64, 69, 104, 255);
		}

		namespace notify
		{
			inline ImVec4 red_status = ImColor(255, 48, 47, 255);
			inline ImVec4 blue_status = ImColor(77, 98, 229, 255);
			inline ImVec4 green_status = ImColor(74, 201, 126, 255);
			inline ImVec4 yellow_status = ImColor(226, 111, 32, 255);

		}

		namespace combo
		{
			inline ImVec4 filling = ImColor(15, 16, 23, 255);
		}

		inline ImVec4 filling = ImColor(25, 23, 48, 255);
		inline ImVec4 circle_mark = ImColor(33, 31, 59, 255);
		inline ImVec4 text_active = ImColor(255, 255, 255, 255);
		inline ImVec4 text_hov = ImColor(111, 115, 148, 255);
		inline ImVec4 text = ImColor(75, 78, 101, 255);
		inline ImVec4 selector = ImColor(30, 31, 33, 80);

		inline float rounding = 8;
	}
}