#include "imgui.h"

#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")

class c_settings
{
public:

	struct
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

		std::string name = "S\nY\nN\nT\nH\nE\nT\nI\nC";

		ImVec2 window_size = ImVec2(840, 630);
		ImVec2 padding = ImVec2(0, 0);
		ImVec2 item_spacing = ImVec2(4, 4);

		float scrollbar_size = 20.f;
		float border_size = 0.f;

		float general_rounding = 12.f;
		float rounding = 8.f;

		bool slider_hovered = false;
		ImGuiID slider_id = 0;
	} c_window;

	struct
	{
		ImVec2 child_window_padding = ImVec2(10, 10);
		ImVec2 child_item_spacing = ImVec2(10, 10);

		float rounding = 4.f;

	} c_child;

	struct
	{
		float rounding = 2.f;

	} c_element;

	struct
	{
		float size = 11.f;
		float rounding = 2.f;
		ImVec2 padding = ImVec2(10, 10);
		ImVec2 picker_size = ImVec2(150, 100);
		ImVec2 bar_size = ImVec2(picker_size.x, 8);
	} c_picker;

	struct
	{
		ID3D11ShaderResourceView* bg = nullptr;
		ID3D11ShaderResourceView* logo = nullptr;

	} c_texture;

	struct
	{
		ImFont* icon[7];
		ImFont* inter_medium[2];
		ImFont* name;

	} c_font;

};

inline c_settings* set = new c_settings();

class c_colors
{
public:

	struct
	{
		ImVec4 col_bg_alpha_0 = ImColor(125, 125, 125, 255);
		ImVec4 col_bg_alpha_1 = ImColor(185, 185, 185, 255);

		ImVec4 accent_clr = ImColor(142, 132, 255, 255);

		ImVec4 black_clr = ImColor(0, 0, 0, 255);
		ImVec4 white_clr = ImColor(255, 255, 255, 255);

	} c_other_clr;

	struct
	{
		ImVec4 general_layout = ImColor(0, 0, 0, 90);
		ImVec4 general_stroke = ImColor(21, 23, 26, 255);

		ImVec4 layout = ImColor(13, 13, 18, 255);
		ImVec4 stroke = ImColor(19, 18, 26, 255);

	} c_window;

	struct
	{
		ImVec4 layout = ImColor(16, 16, 22, 255);
		ImVec4 stroke = ImColor(19, 18, 26, 255);

	} c_child;

	struct
	{
		ImVec4 dropdown_selection_layout = ImColor(31, 31, 41, 255);
		ImVec4 popup_icon = ImColor(45, 43, 62, 255);

		ImVec4 checkbox_active = ImColor(37, 36, 53, 255);
		ImVec4 separator = ImColor(19, 18, 26, 255);

		ImVec4 layout = ImColor(21, 21, 29, 255);
		ImVec4 circle = ImColor(41, 41, 53, 255);

	} c_element;

	struct
	{
		ImVec4 text_active = ImColor(255, 255, 255, 255);
		ImVec4 text_hov = ImColor(104, 104, 120, 255);
		ImVec4 text = ImColor(49, 49, 61, 255);

	} c_text;

	struct
	{
		ImVec4 background = ImColor(18, 18, 25);
		ImVec4 stroke = ImColor(23, 23, 31);
	} c_lua;

};

inline c_colors* clr = new c_colors();

