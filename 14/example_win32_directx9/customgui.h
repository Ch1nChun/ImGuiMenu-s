#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#define IM_USE using namespace ImGui;

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imstb_textedit.h"
#include "hashes.h"

#include <string>

#define IM_FLOOR(_VAL)                  ((float)(int)(_VAL))     

namespace CGUI
{


	namespace vars
	{
		static const signed char    IM_S8_MIN = -128;
		static const signed char    IM_S8_MAX = 127;
		static const unsigned char  IM_U8_MIN = 0;
		static const unsigned char  IM_U8_MAX = 0xFF;
		static const signed short   IM_S16_MIN = -32768;
		static const signed short   IM_S16_MAX = 32767;
		static const unsigned short IM_U16_MIN = 0;
		static const unsigned short IM_U16_MAX = 0xFFFF;
		static const ImS32          IM_S32_MIN = INT_MIN;
		static const ImS32          IM_S32_MAX = INT_MAX;
		static const ImU32          IM_U32_MIN = 0;
		static const ImU32          IM_U32_MAX = UINT_MAX;
#ifdef LLONG_MIN
		static const ImS64          IM_S64_MIN = LLONG_MIN;
		static const ImS64          IM_S64_MAX = LLONG_MAX;
#else
		static const ImS64          IM_S64_MIN = -9223372036854775807LL - 1;
		static const ImS64          IM_S64_MAX = 9223372036854775807LL;
#endif
		static const ImU64          IM_U64_MIN = 0;
#ifdef ULLONG_MAX
		static const ImU64          IM_U64_MAX = ULLONG_MAX;
#else
		static const ImU64          IM_U64_MAX = (2ULL * 9223372036854775807LL + 1);
#endif
	}

	bool		SharedFile(const char* label, const char* desc, const char* author, const char* path, int post_data, int downloads, int* size1, float* alpha);

	bool		Tab(const char* label, const char* icon, const bool selected, const bool resize_state, ImColor active_clr, int* anim, int* text_animate, ImColor non_active_clr = ImColor(0, 0, 0, 0));

	bool		Button(const char* label, ImVec2 size_arg = ImVec2(0, 0), ImGuiButtonFlags flags = 0);
	bool		Config(const char* label, bool selected, ImVec2 size_arg, ImGuiButtonFlags flags);
	bool		SubTab(const char* label, bool selected, ImVec2 size1 = ImVec2(100, 60));
	bool		BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags);
	void		EndCombo();
	void		Spots(const char* label, bool* v);
	void		Checkbox(const char* label, bool* v);
	void		ShowHelpMarker(const char* desc, bool sameline = true, float sl_d = 0.f);

	bool        Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
	bool        Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);
	bool        Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);

	bool        InputScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_step = NULL, const void* p_step_fast = NULL, const char* format = NULL, ImGuiInputTextFlags flags = 0);
	bool        InputInt(const char* label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);

	void        SameLine(float offset_from_start_x = 0.0f, float spacing = 10.0f);
	void		TextWFont(const char* label, ImFont* font, ImColor clr = ImColor(255, 255, 255));

	bool        SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d");
	bool        SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);

	void		DrawLogo(ImVec2 center, float size = 30, bool pulse = false, float speeed = 0.35f, int alpha = 255);
	}