#include "customgui.h"


#include <ctime>
#include <algorithm>

static const char* PatchFormatStringFloatToInt(const char* fmt)
{
	if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
		return "%d";
	const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
	const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
	if (fmt_end > fmt_start && fmt_end[-1] == 'f')
	{
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
		if (fmt_start == fmt && fmt_end[0] == 0)
			return "%d";
		ImGuiContext& g = *GImGui;
		ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
		return g.TempBuffer;
#else
		IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
	}
	return fmt;
}

namespace CGUI
{
	void DrawLogo(ImVec2 center, float size, bool pulse, float speeed, int alpha) {
		ImDrawList* draw_list = ImGui::GetCurrentWindow()->DrawList;
		static float mult = 0;
		if (pulse) {
			static bool pulse_state = true;
			if (pulse_state) {
				mult += speeed * (60 / ImGui::GetIO().Framerate);
				if (mult >= 30.f)
					pulse_state = false;
			}
			else if (!pulse_state) {
				mult -= speeed * (60 / ImGui::GetIO().Framerate);
				if (mult <= 0.0f)
					pulse_state = true;
			}
		}

		ImColor light = ImColor(117 - (int)mult, 170 - (int)mult, 255 - (int)mult, alpha),
			mid = ImColor(66 - (int)mult, 138 - (int)mult, 254 - (int)mult, alpha),
			dark = ImColor(54 - (int)mult, 116 - (int)mult, 217 - (int)mult, alpha);

		draw_list->AddTriangleFilled(center,
			{ center.x, center.y - size },
			{ center.x - float(size / 2.5), center.y - size / 5 },
			mid);
		draw_list->AddTriangleFilled(center,
			{ center.x, center.y - size },
			{ center.x + float(size / 2.5), center.y - size / 5 },
			light);
		draw_list->AddTriangleFilled({ center.x - float(size / 2), center.y },
			{ center.x - float(size * 1.2), center.y + float(size / 1.5) },
			{ center.x - size, center.y - float(size / 1.5) },
			mid);
		draw_list->AddTriangleFilled({ center.x + float(size / 2), center.y },
			{ center.x + float(size * 1.2), center.y + float(size / 1.5) },
			{ center.x + size, center.y - float(size / 1.5) },
			dark);
		draw_list->AddTriangleFilled({ center.x - float(size / 2), center.y },
			{ center.x - float(size * 1.2), center.y + float(size / 1.5) },
			{ center.x - size / 4, center.y + float(size / 2.7) },
			light);
		draw_list->AddTriangleFilled({ center.x + float(size / 2), center.y },
			{ center.x + float(size * 1.2), center.y + float(size / 1.5) },
			{ center.x + size / 4, center.y + float(size / 2.7) },
			light);
		draw_list->AddTriangleFilled(center,
			{ center.x, center.y + float(size / 1.2) },
			{ center.x - size, center.y - float(size / 1.5) },
			dark);
		draw_list->AddTriangleFilled(center,
			{ center.x, center.y + float(size / 1.2) },
			{ center.x + size, center.y - float(size / 1.5) },
			mid);
	}

	void SameLine(float offset_from_start_x, float spacing_w)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;
		ImGuiContext& g = *GImGui;
		if (offset_from_start_x != 0.0f) {
			window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + offset_from_start_x + spacing_w + window->DC.GroupOffset.x + window->DC.ColumnsOffset.x;
			window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
		}
		else {
			window->DC.CursorPos.x = window->DC.CursorPosPrevLine.x + spacing_w;
			window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
		}
	}

	void Spots(const char* label, bool* v)
	{
		ImGuiContext& g = *GImGui;
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec4 check_col_vec4 = ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_CheckMark));

		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const ImGuiStyle& style = g.Style;
		const float square_sz = ImGui::GetFrameHeight();
		const ImRect total_bb = ImRect(p, p + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
		const ImRect check_bb = ImRect(p, p + ImVec2(square_sz, square_sz));

		const float nGpad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
		float height = ImGui::GetFrameHeight();
		float width = height * 1.5 + label_size.x;
		const float radius = 9;


		ImGui::InvisibleButton(label, ImVec2(width, height));

		if (ImGui::IsItemClicked())
			*v = !*v;

		float t = *v ? 1.0f : 0.0f;
		float ANIM_SPEED = 0.23f; // Bigger = Slower
		if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg;
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(39 / 255.f, 71 / 255.f, 117 / 255.f, 1.f),
			ImVec4(check_col_vec4.x + 0.03f, check_col_vec4.y + 0.03f, check_col_vec4.z + 0.3f, 1.f), t));

		if (*v || (t != 0.f))
			draw_list->AddCircleFilled(
				check_bb.Min + ImVec2(nGpad + radius + 2, nGpad + radius + 2),
				radius,
				col_bg,
				12
			);
		else
			draw_list->AddCircleFilled(
				check_bb.Min + ImVec2(nGpad + radius + 2, nGpad + radius + 2),
				radius,
				col_bg,
				12
			);
	}

	void Heart(const char* label)
	{
		ImGuiContext& g = *GImGui;
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec4 check_col_vec4 = ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_CheckMark));

		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const ImGuiStyle& style = g.Style;
		const float square_sz = ImGui::GetFrameHeight();
		const ImRect total_bb = ImRect(p, p + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
		const ImRect check_bb = ImRect(p, p + ImVec2(square_sz, square_sz));

		float height = ImGui::GetFrameHeight();
		float width = height;

		ImGui::InvisibleButton(label, ImVec2(width, height));

		if (ImGui::IsItemClicked())
		{

		}

		if (label_size.x > 0.0f)
			ImGui::RenderText(ImVec2(check_bb.Min.x, check_bb.Min.y + style.FramePadding.y), ICON_FA_HEART);
	}

	void Checkbox(const char* label, bool* v)
	{
		ImGuiContext& g = *GImGui;
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec4 check_col_vec4 = ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_CheckMark));

		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const ImGuiStyle& style = g.Style;
		const float square_sz = ImGui::GetFrameHeight();
		const ImRect total_bb = ImRect(p, p + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
		const ImRect check_bb = ImRect(p, p + ImVec2(square_sz, square_sz));

		const float nGpad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
		float height = ImGui::GetFrameHeight();
		float width = height * 1.5 + label_size.x;
		const float radius = 7;

		ImGui::InvisibleButton(label, ImVec2(width, height));

		if (ImGui::IsItemClicked())
			*v = !*v;

		float t = *v ? 1.0f : 0.0f;
		float ANIM_SPEED = 0.23f; // Bigger = Slower
		if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg;
		if (ImGui::IsItemHovered())
			col_bg = ImGui::GetColorU32(ImLerp(ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_FrameBgHovered)),
				ImVec4(check_col_vec4.x + 0.03f,
					check_col_vec4.y + 0.03f,
					check_col_vec4.z + 0.3f, t), t));/*,
			ImGui::SetMouseCursor(7);*/
		else col_bg = ImGui::GetColorU32(ImLerp(ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_FrameBgActive)),
			check_col_vec4, t));


		if (*v || (t != 0.f))
			draw_list->AddCircle(
				check_bb.Min + ImVec2(nGpad + radius + 2, nGpad + radius + 2),
				radius,
				col_bg,
				12,
				3.5 * t
			);
		else
			draw_list->AddCircle(
				check_bb.Min + ImVec2(nGpad + radius + 2, nGpad + radius + 2),
				radius,
				col_bg,
				12,
				3.5 + t
			);

		if (label_size.x > 0.0f)
			ImGui::RenderText(ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y), label);

#ifdef DEBUG
		draw_list->AddText(ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x + 100, check_bb.Min.y + style.FramePadding.y), ImColor(255, 255, 255), std::to_string(t).c_str());
#endif
	}

	std::string timeStampToHReadble(long  timestamp)
	{
		const time_t rawtime = (const time_t)timestamp;

		struct tm* dt;
		char timestr[30];
		char buffer[30];

		dt = localtime(&rawtime);
		// use any strftime format spec here
		strftime(timestr, sizeof(timestr), "%d.%m.%y %H:%M", dt);
		sprintf(buffer, "%s", timestr);
		std::string stdBuffer(buffer);
		return stdBuffer;
	}

	bool SharedFile(const char* label, const char* desc, const char* author, const char* path, int post_data, int downloads, int* size1, float* alpha)
	{
		ImGuiContext& g = *GImGui;
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec4 check_col_vec4 = ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_CheckMark));

		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const ImGuiStyle& style = g.Style;
		const ImRect total_bb = ImRect(p, p + ImVec2(550, 90 + *size1));

		draw_list->AddRectFilled(total_bb.Min, total_bb.Max, ImColor(21, 20, 25), 4.f);
		draw_list->AddCircleFilled(p + ImVec2(45, 45), 30, ImColor(120, 120, 120, 120), 32);

		DrawLogo(p + ImVec2(45, 45), 16);

		draw_list->AddText(p + ImVec2(90, 18), ImColor(240, 240, 240, 240), label);
		draw_list->AddText(p + ImVec2(90, 34), ImColor(160, 160, 160, 240), desc);

		draw_list->AddText(p + ImVec2(90, 61), ImColor(160, 160, 160, 240), "By"); 		draw_list->AddText(p + ImVec2(110, 61), ImColor(117, 170, 255, 240), author);

		draw_list->AddText(p + ImVec2(540 - ImGui::CalcTextSize(timeStampToHReadble(post_data).c_str()).x, 61), ImColor(160, 160, 160, 240), timeStampToHReadble(post_data).c_str());

		ImGui::InvisibleButton(label, ImVec2(550, 90 + *size1));

		if (ImGui::IsItemHovered())
		{
			draw_list->AddRectFilled(total_bb.Min, total_bb.Max, ImColor(220, 220, 220, 10), 4.f);

			if (*size1 < 30)
				*size1 += 5 * (60 / ImGui::GetIO().Framerate);

			if (*alpha < 1.f)
				*alpha += 0.1f * (60 / ImGui::GetIO().Framerate);
		}
		else
		{
			if (*size1 > 0)
				*size1 -= 5 * (60 / ImGui::GetIO().Framerate);

			if (*alpha > 0)
				*alpha -= 0.1f * (60 / ImGui::GetIO().Framerate);
		}

		std::string text3 = std::to_string(downloads);
		text3.append(" ");
		text3.append(ICON_FA_DOWNLOAD);

		draw_list->AddText(p + ImVec2(540 - ImGui::CalcTextSize(text3.c_str()).x, 17), ImColor(160, 160, 160, 240), text3.c_str());

		ImGui::RenderFrame(total_bb.Max - ImVec2(70, 36), total_bb.Max - ImVec2(10, 10), ImColor(64 / 255.f, 139 / 255.f, 255 / 255.f, *alpha), true, 4.f);

		if (*size1 >= 30)
		{

			std::string text = ICON_FA_PLUS;
			text.append("  Add");

			draw_list->AddText(total_bb.Max - ImVec2(60, 31), ImColor(255, 255, 255, 255), text.c_str());
		}

		if (*size1 > 10)
		{
			std::string text2 = "Will be saved as ";
			text2.append(path);
			draw_list->AddText(p + ImVec2(15, 91), ImColor(160 / 255.f, 160 / 255.f, 160 / 255.f, *alpha), text2.c_str());
		}

		std::clamp(*size1, 0, 30);
		std::clamp(*alpha, 0.f, 1.f);

		if (ImGui::IsItemClicked() && ImGui::IsMouseHoveringRect(total_bb.Max - ImVec2(70, 36), total_bb.Max - ImVec2(10, 10)) && *size1 >= 30)
			return true;
		else
			return false;
	}

	bool Tab(const char* label, const char* icon, const bool selected, const bool resize_state, ImColor active_clr, int* alpha, int* text_animate, ImColor non_active_clr)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImVec2 size_arg;

		static float resize_volume = 0;
		size_arg = ImVec2{ 40 + resize_volume,30 };

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		if (non_active_clr == ImColor(0, 0, 0, 0)) non_active_clr = ImColor(230, 230, 230, 110 + *alpha);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
		const ImRect bb(pos, { pos.x + size.x , pos.y + size.y });

		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		//if (ImGui::IsItemHovered())
		//	ImGui::SetMouseCursor(7);

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

		if (resize_state) {
			if (resize_volume < 105)
				resize_volume += 15;
		}
		else {
			if (resize_volume > 0)
				resize_volume -= 15;
		}

		if (selected || hovered) {
			if (*alpha < 255)
				*alpha += 30;
		}
		else {
			if (*alpha > 0)
				*alpha -= 30;
		}

		if (resize_state)
			if (selected)
				window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(71, 139, 255, *alpha), 4);
			else
				window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(34, 36, 41, *alpha), 4);

		std::string text; text.append(icon);
		if (resize_state)
			text.append("  "),
			text.append(label);

		if (selected)
			window->DrawList->AddText({ bb.Min.x + 15,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(text.c_str()).y / 2 }, active_clr, text.c_str());
		else
			window->DrawList->AddText({ bb.Min.x + 15,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(text.c_str()).y / 2 }, non_active_clr, text.c_str());

		return pressed;
	}

	void ShowHelpMarker(const char* desc, bool sameline, float sl_d) {

		if (sameline)
			SameLine(0.f, sl_d == 0 ? 1.f : sl_d);

		ImGui::TextDisabled((const char*)" " "\uf059" " ");

		ImDrawList* draw_list = ImGui::GetOverlayDrawList();
		ImVec2 cur_pos = ImGui::GetIO().MousePos;
		const ImVec2 label_sz = ImGui::CalcTextSize(desc, NULL, true, 0.f);
		ImVec2 textpos = { cur_pos.x + label_sz.x / 2, cur_pos.y + label_sz.y / 2 };

		if (ImGui::IsItemHovered()) {
			draw_list->AddText(textpos, ImColor(255, 255, 255), desc, NULL, true);
		}
	}

	bool Button(const char* label, ImVec2 size_arg, ImGuiButtonFlags flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		if ((size_arg.x && size_arg.y) == 0)
			size_arg = ImVec2(150, 30);

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const float out_sz = 5;

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
			flags |= ImGuiButtonFlags_Repeat;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

		//if (hovered)
		//	ImGui::SetMouseCursor(7);

		bool draw_outer = false;

		float t = draw_outer ? 1.0f : 0.0f;
		float ANIM_SPEED = 0.5f; // Bigger = Slower
		if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = draw_outer ? (t_anim) : (1.0f - t_anim);
		}

		if (pressed)
			draw_outer = !draw_outer;

		if ((t >= 0))
			ImGui::GetWindowDrawList()->AddRectFilled(
				bb.Min - ImVec2(out_sz * (1 - t), out_sz * (1 - t)),
				bb.Max + ImVec2(out_sz * (1 - t), out_sz * (1 - t)),
				ImColor(51, 153, 255, int(t * 255)),
				4.0f
			);

		const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		ImGui::RenderNavHighlight(bb, id);
		ImGui::RenderFrame(bb.Min, bb.Max, col, true, 4.f/*style.FrameRounding*/);
		ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
		return pressed;
	}

	bool Config(const char* label, bool selected, ImVec2 size_arg, ImGuiButtonFlags flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		if ((size_arg.x && size_arg.y) == 0)
			size_arg = ImVec2(150, 30);

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const float out_sz = 5;

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
			flags |= ImGuiButtonFlags_Repeat;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

		//if (hovered)
		//	ImGui::SetMouseCursor(7);

		bool draw_outer = false;

		float t = draw_outer ? 1.0f : 0.0f;
		float ANIM_SPEED = 0.5f; // Bigger = Slower
		if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = draw_outer ? (t_anim) : (1.0f - t_anim);
		}

		if (pressed)
			draw_outer = !draw_outer;

		if ((t >= 0))
			ImGui::GetWindowDrawList()->AddRectFilled(
				bb.Min - ImVec2(out_sz * (1 - t), out_sz * (1 - t)),
				bb.Max + ImVec2(out_sz * (1 - t), out_sz * (1 - t)),
				ImColor(51, 153, 255, int(t * 255)),
				4.0f
			);

		const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);


		ImGui::RenderNavHighlight(bb, id);
		ImGui::RenderFrame(bb.Min, bb.Max, col, true, 4.f/*style.FrameRounding*/);
		if (selected)
			window->DrawList->AddRect(bb.Min, bb.Max, ImColor(240, 240, 240, 200), 4.f, 15, 3);

		ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
		return pressed;
	}

	bool SubTab(const char* label, bool selected, ImVec2 size1)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const float out_sz = 5;

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize(size1, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		ImGui::ItemAdd(bb, id);

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

		//if (hovered || held)
		//	ImGui::SetMouseCursor(7);
		if (selected)
			ImGui::RenderFrame({ bb.Min.x, bb.Max.y - 4 }, bb.Max, ImColor(51, 153, 255), true, 5);
		ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

		return pressed;
	}

	static float CalcMaxPopupHeightFromItemCount(int items_count)
	{
		ImGuiContext& g = *GImGui;
		if (items_count <= 0)
			return FLT_MAX;
		return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
	}

	bool BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
	{
		ImGuiContext& g = *GImGui;
		bool has_window_size_constraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
		g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : ImGui::GetFrameHeight();
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const float expected_w = ImGui::CalcItemWidth();
		const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : expected_w;
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f + 2));
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
		ImGui::ItemSize(total_bb, style.FramePadding.y);
		ImGui::ItemAdd(total_bb, id, &frame_bb);

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);
		bool popup_open = ImGui::IsPopupOpen(id, NULL);

		const ImU32 frame_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);
		ImGui::RenderNavHighlight(frame_bb, id);

		if (!(flags & ImGuiComboFlags_NoPreview))
			window->DrawList->AddRectFilled(frame_bb.Min, ImVec2(value_x2, frame_bb.Max.y), frame_col, 4, (flags & ImGuiComboFlags_NoArrowButton) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Left);

		if (!(flags & ImGuiComboFlags_NoArrowButton))
		{
			ImU32 bg_col = ImGui::GetColorU32((popup_open || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
			ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
			window->DrawList->AddRectFilled(ImVec2(value_x2, frame_bb.Min.y), frame_bb.Max, frame_col, 4, (w <= arrow_size) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Right);
			if (value_x2 + arrow_size - style.FramePadding.x <= frame_bb.Max.x)
				ImGui::RenderArrow(ImVec2(value_x2 + style.FramePadding.y, frame_bb.Min.y + style.FramePadding.y + 4), popup_open ? ImGuiDir_Up : ImGuiDir_Down, 0.6f);
		}

		if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
			ImGui::RenderTextClipped(frame_bb.Min + style.FramePadding, ImVec2(value_x2, frame_bb.Max.y + 1), preview_value, NULL, NULL, ImVec2(0.0f, 0.0f));
		if (label_size.x > 0)
			ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y + 1), label);

		if ((pressed || g.NavActivateId == id) && !popup_open)
		{
			if (window->DC.NavLayerCurrent == 0)
				window->NavLastIds[0] = id;
			ImGui::OpenPopupEx(id);
			popup_open = true;
		}

		if (!popup_open)
			return false;

		if (has_window_size_constraint)
		{
			g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSizeConstraint;
			g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
		}
		else
		{
			if ((flags & ImGuiComboFlags_HeightMask_) == 0)
				flags |= ImGuiComboFlags_HeightRegular;
			IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));    // Only one
			int popup_max_height_in_items = -1;
			if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
			else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
			else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
			ImGui::SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
		}

		char name[16];
		ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

		// Peak into expected window size so we can position it
		if (ImGuiWindow* popup_window = ImGui::FindWindowByName(name))
			if (popup_window->WasActive)
			{
				ImVec2 size_expected = ImGui::CalcWindowExpectedSize(popup_window);
				if (flags & ImGuiComboFlags_PopupAlignLeft)
					popup_window->AutoPosLastDirection = ImGuiDir_Left;
				ImRect r_outer = ImGui::GetWindowAllowedExtentRect(popup_window);
				ImVec2 pos = ImGui::FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);
				ImGui::SetNextWindowPos(pos);
			}

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImGui::ColorConvertU32ToFloat4(ImColor(51, 51, 55)));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 4.f);

		bool ret = ImGui::Begin(name, NULL, window_flags);

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();
		if (!ret) {
			ImGui::EndPopup();
			IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above	
			return false;
		}
		return true;
	}

	void EndCombo()
	{
		ImGui::EndPopup();
	}

	// Getter for the old Combo() API: const char*[]
	static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
	{
		const char* const* items = (const char* const*)data;
		if (out_text)
			*out_text = items[idx];
		return true;
	}

	// Getter for the old Combo() API: "item1\0item2\0item3\0"
	static bool Items_SingleStringGetter(void* data, int idx, const char** out_text)
	{
		// FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
		const char* items_separated_by_zeros = (const char*)data;
		int items_count = 0;
		const char* p = items_separated_by_zeros;
		while (*p)
		{
			if (idx == items_count)
				break;
			p += strlen(p) + 1;
			items_count++;
		}
		if (!*p)
			return false;
		if (out_text)
			*out_text = p;
		return true;
	}

	// Old API, prefer using BeginCombo() nowadays if you can.
	bool Combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
	{
		ImGuiContext& g = *GImGui;

		// Call the getter to obtain the preview string which is a parameter to BeginCombo()
		const char* preview_value = NULL;
		if (*current_item >= 0 && *current_item < items_count)
			items_getter(data, *current_item, &preview_value);

		// The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
		if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
			ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

		if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
			return false;

		// Display items
		// FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
		bool value_changed = false;
		for (int i = 0; i < items_count; i++)
		{
			ImGui::PushID((void*)(intptr_t)i);
			const bool item_selected = (i == *current_item);
			const char* item_text;
			if (!items_getter(data, i, &item_text))
				item_text = "[Unknown item]";
			if (ImGui::Selectable(item_text, item_selected))
			{
				value_changed = true;
				*current_item = i;
			}
			if (item_selected)
				ImGui::SetItemDefaultFocus();
			ImGui::PopID();
		}

		EndCombo();
		return value_changed;
	}

	// Combo box helper allowing to pass an array of strings.
	bool Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
	{
		const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
		return value_changed;
	}

	// Combo box helper allowing to pass all items in a single string literal holding multiple zero-terminated items "item1\0item2\0"
	bool Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
	{
		int items_count = 0;
		const char* p = items_separated_by_zeros;       // FIXME-OPT: Avoid computing this, or at least only when combo is open
		while (*p)
		{
			p += strlen(p) + 1;
			items_count++;
		}
		bool value_changed = Combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
		return value_changed;
	}

	struct ImGuiDataTypeInfo
	{
		size_t      Size;           // Size in byte
		const char* PrintFmt;       // Default printf format for the type
		const char* ScanFmt;        // Default scanf format for the type
	};

	static const ImGuiDataTypeInfo GDataTypeInfo[] =
	{
		{ sizeof(char),             "%d",   "%d"    },  // ImGuiDataType_S8
		{ sizeof(unsigned char),    "%u",   "%u"    },
		{ sizeof(short),            "%d",   "%d"    },  // ImGuiDataType_S16
		{ sizeof(unsigned short),   "%u",   "%u"    },
		{ sizeof(int),              "%d",   "%d"    },  // ImGuiDataType_S32
		{ sizeof(unsigned int),     "%u",   "%u"    },
	#ifdef _MSC_VER
		{ sizeof(ImS64),            "%I64d","%I64d" },  // ImGuiDataType_S64
		{ sizeof(ImU64),            "%I64u","%I64u" },
	#else
		{ sizeof(ImS64),            "%lld", "%lld"  },  // ImGuiDataType_S64
		{ sizeof(ImU64),            "%llu", "%llu"  },
	#endif
		{ sizeof(float),            "%f",   "%f"    },  // ImGuiDataType_Float (float are promoted to double in va_arg)
		{ sizeof(double),           "%f",   "%lf"   },  // ImGuiDataType_Double
	};

	static bool DataTypeApplyOpFromText(const char* buf, const char* initial_value_buf, ImGuiDataType data_type, void* data_ptr, const char* format)
	{
		while (ImCharIsBlankA(*buf))
			buf++;

		// We don't support '-' op because it would conflict with inputing negative value.
		// Instead you can use +-100 to subtract from an existing value
		char op = buf[0];
		if (op == '+' || op == '*' || op == '/')
		{
			buf++;
			while (ImCharIsBlankA(*buf))
				buf++;
		}
		else
		{
			op = 0;
		}
		if (!buf[0])
			return false;

		// Copy the value in an opaque buffer so we can compare at the end of the function if it changed at all.
		IM_ASSERT(data_type < ImGuiDataType_COUNT);
		int data_backup[2];
		IM_ASSERT(GDataTypeInfo[data_type].Size <= sizeof(data_backup));
		memcpy(data_backup, data_ptr, GDataTypeInfo[data_type].Size);

		if (format == NULL)
			format = GDataTypeInfo[data_type].ScanFmt;

		int arg1i = 0;
		if (data_type == ImGuiDataType_S32)
		{
			int* v = (int*)data_ptr;
			int arg0i = *v;
			float arg1f = 0.0f;
			if (op && sscanf(initial_value_buf, format, &arg0i) < 1)
				return false;
			// Store operand in a float so we can use fractional value for multipliers (*1.1), but constant always parsed as integer so we can fit big integers (e.g. 2000000003) past float precision
			if (op == '+') { if (sscanf(buf, "%d", &arg1i)) *v = (int)(arg0i + arg1i); }                   // Add (use "+-" to subtract)
			else if (op == '*') { if (sscanf(buf, "%f", &arg1f)) *v = (int)(arg0i * arg1f); }                   // Multiply
			else if (op == '/') { if (sscanf(buf, "%f", &arg1f) && arg1f != 0.0f) *v = (int)(arg0i / arg1f); }  // Divide
			else { if (sscanf(buf, format, &arg1i) == 1) *v = arg1i; }                           // Assign constant
		}
		else if (data_type == ImGuiDataType_U32 || data_type == ImGuiDataType_S64 || data_type == ImGuiDataType_U64)
		{
			// Assign constant
			// FIXME: We don't bother handling support for legacy operators since they are a little too crappy. Instead we may implement a proper expression evaluator in the future.
			sscanf(buf, format, data_ptr);
		}
		else if (data_type == ImGuiDataType_Float)
		{
			// For floats we have to ignore format with precision (e.g. "%.2f") because sscanf doesn't take them in
			format = "%f";
			float* v = (float*)data_ptr;
			float arg0f = *v, arg1f = 0.0f;
			if (op && sscanf(initial_value_buf, format, &arg0f) < 1)
				return false;
			if (sscanf(buf, format, &arg1f) < 1)
				return false;
			if (op == '+') { *v = arg0f + arg1f; }                    // Add (use "+-" to subtract)
			else if (op == '*') { *v = arg0f * arg1f; }                    // Multiply
			else if (op == '/') { if (arg1f != 0.0f) *v = arg0f / arg1f; } // Divide
			else { *v = arg1f; }                            // Assign constant
		}
		else if (data_type == ImGuiDataType_Double)
		{
			format = "%lf"; // scanf differentiate float/double unlike printf which forces everything to double because of ellipsis
			double* v = (double*)data_ptr;
			double arg0f = *v, arg1f = 0.0;
			if (op && sscanf(initial_value_buf, format, &arg0f) < 1)
				return false;
			if (sscanf(buf, format, &arg1f) < 1)
				return false;
			if (op == '+') { *v = arg0f + arg1f; }                    // Add (use "+-" to subtract)
			else if (op == '*') { *v = arg0f * arg1f; }                    // Multiply
			else if (op == '/') { if (arg1f != 0.0f) *v = arg0f / arg1f; } // Divide
			else { *v = arg1f; }                            // Assign constant
		}
		return memcmp(data_backup, data_ptr, GDataTypeInfo[data_type].Size) != 0;
	}

	const ImGuiDataTypeInfo* DataTypeGetInfo(ImGuiDataType data_type)
	{
		IM_ASSERT(data_type >= 0 && data_type < ImGuiDataType_COUNT);
		return &GDataTypeInfo[data_type];
	}

	int DataTypeFormatString(char* buf, int buf_size, ImGuiDataType data_type, const void* p_data, const char* format)
	{
		if (data_type == ImGuiDataType_S32 || data_type == ImGuiDataType_U32)
			return ImFormatString(buf, buf_size, format, *(const ImU32*)p_data);
		if (data_type == ImGuiDataType_S64 || data_type == ImGuiDataType_U64)
			return ImFormatString(buf, buf_size, format, *(const ImU64*)p_data);
		if (data_type == ImGuiDataType_Float)
			return ImFormatString(buf, buf_size, format, *(const float*)p_data);
		if (data_type == ImGuiDataType_Double)
			return ImFormatString(buf, buf_size, format, *(const double*)p_data);
		IM_ASSERT(0);
		return 0;
	}


	bool InputScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags)
	{
		IM_USE
			ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		ImGuiStyle& style = g.Style;

		if (format == NULL)
			format = DataTypeGetInfo(data_type)->PrintFmt;

		char buf[64];
		DataTypeFormatString(buf, IM_ARRAYSIZE(buf), data_type, p_data, format);

		bool value_changed = false;
		bool get_rekt = false;

		if ((flags & (ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsScientific)) == 0)
			flags |= ImGuiInputTextFlags_CharsDecimal;
		flags |= ImGuiInputTextFlags_AutoSelectAll;

		if (p_step != NULL)
		{
			const float button_size = GetFrameHeight();

			ImGui::BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
			ImGui::PushID(label);
			ImGui::PushItemWidth(ImMax(1.0f, CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 2));
			if (InputText("", buf, IM_ARRAYSIZE(buf), flags))
				value_changed = DataTypeApplyOpFromText(buf, g.InputTextState.InitialTextA.Data, data_type, p_data, format);
			ImGui::PopItemWidth();

			if (ImGui::IsItemActive())
				get_rekt = !get_rekt;

			// Step buttons
			const ImVec2 backup_frame_padding = style.FramePadding;

			style.FramePadding.x = style.FramePadding.y;

			ImGuiButtonFlags button_flags = ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups;
			if (flags & ImGuiInputTextFlags_ReadOnly)
				button_flags |= ImGuiButtonFlags_Disabled;

			float t = get_rekt ? 1.0f : 0.0f;
			float ANIM_SPEED = 0.23f; // Bigger = Slower
			if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
				float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
				t = get_rekt ? (t_anim) : (1.0f - t_anim);
			}

			ImVec4 check_col_vec4 = ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_CheckMark));
			ImVec4 col_bg = ImLerp(ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_TextDisabled)),
				ImVec4(check_col_vec4.x + 0.03,
					check_col_vec4.y + 0.03,
					check_col_vec4.z + 0.3, t), t);

			SameLine(0, -style.ItemInnerSpacing.x * 4);
			TextColored(col_bg, "\uf104");
			SameLine(0, style.ItemInnerSpacing.x);
			TextColored(col_bg, "\uf105");

			const char* label_end = FindRenderedTextEnd(label);
			if (label != label_end)
			{
				SameLine(0, style.ItemInnerSpacing.x + 10);
				ImGui::Text(label, label_end);
			}
			style.FramePadding = backup_frame_padding;

			PopID();
			EndGroup();
		}
		else
		{
			if (InputText(label, buf, IM_ARRAYSIZE(buf), flags))
				value_changed = DataTypeApplyOpFromText(buf, g.InputTextState.InitialTextA.Data, data_type, p_data, format);
		}

		return value_changed;
	}

	bool InputInt(const char* label, int* v, int step, int step_fast, ImGuiInputTextFlags flags)
	{
		const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
		return InputScalar(label, ImGuiDataType_S32, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
	}

	void TextWFont(const char* label, ImFont* font, ImColor clr) {
		IM_USE;
		PushFont(font);
		TextColored(clr, label);
		PopFont();
	}

	template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
	bool SliderBehaviorT(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, TYPE* v, const TYPE v_min, const TYPE v_max, const char* format, float power, ImGuiSliderFlags flags, ImRect* out_grab_bb)
	{
		IM_USE;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		const ImGuiAxis axis = (flags & ImGuiSliderFlags_Vertical) ? ImGuiAxis_Y : ImGuiAxis_X;
		const bool is_decimal = (data_type == ImGuiDataType_Float) || (data_type == ImGuiDataType_Double);
		const bool is_power = (power != 1.0f) && is_decimal;

		const float grab_padding = 2.0f;
		const float slider_sz = (bb.Max[axis] - bb.Min[axis]) - grab_padding * 2.0f;
		float grab_sz = style.GrabMinSize;
		SIGNEDTYPE v_range = (v_min < v_max ? v_max - v_min : v_min - v_max);
		if (!is_decimal && v_range >= 0)                                             // v_range < 0 may happen on integer overflows
			grab_sz = ImMax((float)(slider_sz / (v_range + 1)), style.GrabMinSize);  // For integer sliders: if possible have the grab size represent 1 unit
		grab_sz = ImMin(grab_sz, slider_sz);
		const float slider_usable_sz = slider_sz - grab_sz;
		const float slider_usable_pos_min = bb.Min[axis] + grab_padding + grab_sz * 0.5f;
		const float slider_usable_pos_max = bb.Max[axis] - grab_padding - grab_sz * 0.5f;

		// For power curve sliders that cross over sign boundary we want the curve to be symmetric around 0.0f
		float linear_zero_pos;   // 0.0->1.0f
		if (is_power && v_min * v_max < 0.0f)
		{
			// Different sign
			const FLOATTYPE linear_dist_min_to_0 = ImPow(v_min >= 0 ? (FLOATTYPE)v_min : -(FLOATTYPE)v_min, (FLOATTYPE)1.0f / power);
			const FLOATTYPE linear_dist_max_to_0 = ImPow(v_max >= 0 ? (FLOATTYPE)v_max : -(FLOATTYPE)v_max, (FLOATTYPE)1.0f / power);
			linear_zero_pos = (float)(linear_dist_min_to_0 / (linear_dist_min_to_0 + linear_dist_max_to_0));
		}
		else
		{
			// Same sign
			linear_zero_pos = v_min < 0.0f ? 1.0f : 0.0f;
		}

		// Process interacting with the slider
		bool value_changed = false;
		if (g.ActiveId == id)
		{
			bool set_new_value = false;
			float clicked_t = 0.0f;
			if (g.ActiveIdSource == ImGuiInputSource_Mouse)
			{
				if (!g.IO.MouseDown[0])
				{
					ClearActiveID();
				}
				else
				{
					const float mouse_abs_pos = g.IO.MousePos[axis];
					clicked_t = (slider_usable_sz > 0.0f) ? ImClamp((mouse_abs_pos - slider_usable_pos_min) / slider_usable_sz, 0.0f, 1.0f) : 0.0f;
					if (axis == ImGuiAxis_Y)
						clicked_t = 1.0f - clicked_t;
					set_new_value = true;
				}
			}
			else if (g.ActiveIdSource == ImGuiInputSource_Nav)
			{
				const ImVec2 delta2 = GetNavInputAmount2d(ImGuiNavDirSourceFlags_Keyboard | ImGuiNavDirSourceFlags_PadDPad, ImGuiInputReadMode_RepeatFast, 0.0f, 0.0f);
				float delta = (axis == ImGuiAxis_X) ? delta2.x : -delta2.y;
				if (g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated)
				{
					ClearActiveID();
				}
				else if (delta != 0.0f)
				{
					clicked_t = SliderCalcRatioFromValueT<TYPE, FLOATTYPE>(data_type, *v, v_min, v_max, power, linear_zero_pos);
					const int decimal_precision = is_decimal ? ImParseFormatPrecision(format, 3) : 0;
					if ((decimal_precision > 0) || is_power)
					{
						delta /= 100.0f;    // Gamepad/keyboard tweak speeds in % of slider bounds
						if (IsNavInputDown(ImGuiNavInput_TweakSlow))
							delta /= 10.0f;
					}
					else
					{
						if ((v_range >= -100.0f && v_range <= 100.0f) || IsNavInputDown(ImGuiNavInput_TweakSlow))
							delta = ((delta < 0.0f) ? -1.0f : +1.0f) / (float)v_range; // Gamepad/keyboard tweak speeds in integer steps
						else
							delta /= 100.0f;
					}
					if (IsNavInputDown(ImGuiNavInput_TweakFast))
						delta *= 10.0f;
					set_new_value = true;
					if ((clicked_t >= 1.0f && delta > 0.0f) || (clicked_t <= 0.0f && delta < 0.0f)) // This is to avoid applying the saturation when already past the limits
						set_new_value = false;
					else
						clicked_t = ImSaturate(clicked_t + delta);
				}
			}

			if (set_new_value)
			{
				TYPE v_new;
				if (is_power)
				{
					// Account for power curve scale on both sides of the zero
					if (clicked_t < linear_zero_pos)
					{
						// Negative: rescale to the negative range before powering
						float a = 1.0f - (clicked_t / linear_zero_pos);
						a = ImPow(a, power);
						v_new = ImLerp(ImMin(v_max, (TYPE)0), v_min, a);
					}
					else
					{
						// Positive: rescale to the positive range before powering
						float a;
						if (ImFabs(linear_zero_pos - 1.0f) > 1.e-6f)
							a = (clicked_t - linear_zero_pos) / (1.0f - linear_zero_pos);
						else
							a = clicked_t;
						a = ImPow(a, power);
						v_new = ImLerp(ImMax(v_min, (TYPE)0), v_max, a);
					}
				}
				else
				{
					// Linear slider
					if (is_decimal)
					{
						v_new = ImLerp(v_min, v_max, clicked_t);
					}
					else
					{
						// For integer values we want the clicking position to match the grab box so we round above
						// This code is carefully tuned to work with large values (e.g. high ranges of U64) while preserving this property..
						FLOATTYPE v_new_off_f = (v_max - v_min) * clicked_t;
						TYPE v_new_off_floor = (TYPE)(v_new_off_f);
						TYPE v_new_off_round = (TYPE)(v_new_off_f + (FLOATTYPE)0.5);
						if (!is_decimal && v_new_off_floor < v_new_off_round)
							v_new = v_min + v_new_off_round;
						else
							v_new = v_min + v_new_off_floor;
					}
				}

				// Round to user desired precision based on format string
				v_new = RoundScalarWithFormatT<TYPE, SIGNEDTYPE>(format, data_type, v_new);

				// Apply result
				if (*v != v_new)
				{
					*v = v_new;
					value_changed = true;
				}
			}
		}

		// Output grab position so it can be displayed by the caller
		float grab_t = SliderCalcRatioFromValueT<TYPE, FLOATTYPE>(data_type, *v, v_min, v_max, power, linear_zero_pos);
		if (axis == ImGuiAxis_Y)
			grab_t = 1.0f - grab_t;
		const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
		if (axis == ImGuiAxis_X)
			*out_grab_bb = ImRect(grab_pos - grab_sz * 0.5f, bb.Min.y + grab_padding, grab_pos + grab_sz * 0.5f, bb.Max.y - grab_padding);
		else
			*out_grab_bb = ImRect(bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f, bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f);

		return value_changed;
	}

	// For 32-bit and larger types, slider bounds are limited to half the natural type range.
	// So e.g. an integer Slider between INT_MAX-10 and INT_MAX will fail, but an integer Slider between INT_MAX/2-10 and INT_MAX/2 will be ok.
	// It would be possible to lift that limitation with some work but it doesn't seem to be worth it for sliders.
	bool SliderBehavior(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, void* p_v, const void* p_min, const void* p_max, const char* format, float power, ImGuiSliderFlags flags, ImRect* out_grab_bb)
	{
		IM_USE;

		switch (data_type)
		{
		case ImGuiDataType_S32:
			IM_ASSERT(*(const ImS32*)p_min >= vars::IM_S32_MIN / 2 && *(const ImS32*)p_max <= vars::IM_S32_MAX / 2);
			return SliderBehaviorT<ImS32, ImS32, float >(bb, id, data_type, (ImS32*)p_v, *(const ImS32*)p_min, *(const ImS32*)p_max, format, power, flags, out_grab_bb);
		case ImGuiDataType_U32:
			IM_ASSERT(*(const ImU32*)p_max <= vars::IM_U32_MAX / 2);
			return SliderBehaviorT<ImU32, ImS32, float >(bb, id, data_type, (ImU32*)p_v, *(const ImU32*)p_min, *(const ImU32*)p_max, format, power, flags, out_grab_bb);
		case ImGuiDataType_S64:
			IM_ASSERT(*(const ImS64*)p_min >= vars::IM_S64_MIN / 2 && *(const ImS64*)p_max <= vars::IM_S64_MAX / 2);
			return SliderBehaviorT<ImS64, ImS64, double>(bb, id, data_type, (ImS64*)p_v, *(const ImS64*)p_min, *(const ImS64*)p_max, format, power, flags, out_grab_bb);
		case ImGuiDataType_U64:
			IM_ASSERT(*(const ImU64*)p_max <= vars::IM_U64_MAX / 2);
			return SliderBehaviorT<ImU64, ImS64, double>(bb, id, data_type, (ImU64*)p_v, *(const ImU64*)p_min, *(const ImU64*)p_max, format, power, flags, out_grab_bb);
		case ImGuiDataType_Float:
			IM_ASSERT(*(const float*)p_min >= -FLT_MAX / 2.0f && *(const float*)p_max <= FLT_MAX / 2.0f);
			return SliderBehaviorT<float, float, float >(bb, id, data_type, (float*)p_v, *(const float*)p_min, *(const float*)p_max, format, power, flags, out_grab_bb);
		case ImGuiDataType_Double:
			IM_ASSERT(*(const double*)p_min >= -DBL_MAX / 2.0f && *(const double*)p_max <= DBL_MAX / 2.0f);
			return SliderBehaviorT<double, double, double>(bb, id, data_type, (double*)p_v, *(const double*)p_min, *(const double*)p_max, format, power, flags, out_grab_bb);
		case ImGuiDataType_COUNT: break;
		}
		IM_ASSERT(0);
		return false;
	}

	bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, float power)
	{
		IM_USE;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		ImVec4 check_col_vec4 = ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_CheckMark));

		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, id, &frame_bb))
			return false;

		// Default format string when passing NULL
		if (format == NULL)
			format = DataTypeGetInfo(data_type)->PrintFmt;
		else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
			format = PatchFormatStringFloatToInt(format);

		// Tabbing or CTRL-clicking on Slider turns it into an input box
		const bool hovered = ItemHoverable(frame_bb, id);
		bool temp_input_is_active = TempInputIsActive(id);
		bool temp_input_start = false;
		if (!temp_input_is_active)
		{
			const bool focus_requested = FocusableItemRegister(window, id);
			const bool clicked = (hovered && g.IO.MouseClicked[0]);
			if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
			{
				SetActiveID(id, window);
				SetFocusID(id, window);
				FocusWindow(window);
				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
				if (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id)
				{
					temp_input_start = true;
					FocusableItemUnregister(window);
				}
			}
		}

		// Our current specs do NOT clamp when using CTRL+Click manual input, but we should eventually add a flag for that..
		if (temp_input_is_active || temp_input_start)
			return TempInputScalar(frame_bb, id, label, data_type, p_data, format);// , p_min, p_max);

		//if (hovered)
		//	ImGui::SetMouseCursor(7);

		// Draw BG frame
		/*const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		RenderNavHighlight(frame_bb, id);
		RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);*/

		// Slider behavior
		ImRect grab_bb;
		const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, power, ImGuiSliderFlags_None, &grab_bb);
		if (value_changed)
			MarkItemEdited(id);

		bool sld_act = false;
		if (IsItemClicked())
			sld_act = !sld_act;

		float t = sld_act ? 1.0f : 0.0f;
		float ANIM_SPEED = 2.f;
		if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = sld_act ? (t_anim) : (1.0f - t_anim);
		}

		// Render grab
		/*if (grab_bb.Max.x > grab_bb.Min.x)
			window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);*/

			// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
			//char value_buf[64];
			//const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

		char value_buf[3][64];
		const char* value_buf_end[3];
		value_buf_end[0] = value_buf[0] + DataTypeFormatString(value_buf[0], IM_ARRAYSIZE(value_buf[0]), data_type, p_data, format);
		value_buf_end[1] = value_buf[1] + DataTypeFormatString(value_buf[1], IM_ARRAYSIZE(value_buf[1]), data_type, p_min, format);
		value_buf_end[2] = value_buf[2] + DataTypeFormatString(value_buf[2], IM_ARRAYSIZE(value_buf[2]), data_type, p_max, format);

		const float del = 1.5; //was 2.f
		RenderText(ImVec2(grab_bb.Min.x, grab_bb.Min.y + ((grab_bb.Max.y - grab_bb.Min.y) / 2.f)) + ImVec2(-3, 7), value_buf[0], value_buf_end[0]);
		RenderFrame(ImVec2(frame_bb.Min.x, frame_bb.Min.y + ((frame_bb.Max.y - frame_bb.Min.y) / del)) - ImVec2(0, 3), ImVec2(frame_bb.Max.x - grab_bb.Max.x + grab_bb.Min.x, frame_bb.Max.y - ((frame_bb.Max.y - frame_bb.Min.y) / del)) + ImVec2(0, 3), ImGui::GetColorU32(ImGuiCol_FrameBg), true, 4.f);
		RenderFrame(ImVec2(frame_bb.Min.x, frame_bb.Min.y + ((frame_bb.Max.y - frame_bb.Min.y) / del)) - ImVec2(0, 3), ImVec2(grab_bb.Min.x, frame_bb.Max.y - ((frame_bb.Max.y - frame_bb.Min.y) / del)) + ImVec2(0, 3), ImGui::GetColorU32(ImGuiCol_CheckMark), true, 4.f);
		if (t != 0)
			window->DrawList->AddCircleFilled(ImVec2(grab_bb.Min.x, grab_bb.Min.y + ((grab_bb.Max.y - grab_bb.Min.y) / 2.f)), (t + 1) * 7.f, ImColor(63, 140, 254, int(t * 255 / 3)), 12 * 3);
		window->DrawList->AddCircleFilled(ImVec2(grab_bb.Min.x, grab_bb.Min.y + ((grab_bb.Max.y - grab_bb.Min.y) / 2.f)), 5.f, ImColor(255, 255, 255), 12);
		window->DrawList->AddCircleFilled(ImVec2(grab_bb.Min.x, grab_bb.Min.y + ((grab_bb.Max.y - grab_bb.Min.y) / 2.f)), 3.f, ImGui::ColorConvertFloat4ToU32(ImVec4(0.26f, 0.59f, 0.98f, t)), 12);



		// Display value using user-provided display format so user can add prefix/suffix/decorations to the value. (grab_bb.Max.x, ��� ������� ������ ��� �� ������ ����� ����� �� �������� grab_bb.max
		if (label_size.x > 0.0f)
			RenderText(ImVec2(frame_bb.Max.x - grab_bb.Max.x + grab_bb.Min.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), /*std::to_string(g.LastActiveIdTimer).c_str()*/label);

		/*RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

		if (label_size.x > 0.0f)
			RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);*/

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
		return value_changed;
	}

	bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, float power)
	{
		return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, power);
	}

	bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format)
	{
		return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, 1);
	}
}