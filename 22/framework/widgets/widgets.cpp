#include "../headers/includes.h"

struct color_picker_state
{
	bool init_val;
	float h, s, v;
};

bool c_widgets::color_picker(std::string name, c_vec4* color)
{
	c_window* window = gui->get_window();
	c_id id = window->GetID(name.data());
	color_picker_state* state = gui->anim_container<color_picker_state>(id);

	if (!state->init_val)
	{
		ImGui::ColorConvertRGBtoHSV(color->x, color->y, color->z, state->h, state->s, state->v);
		state->init_val = true;
	}

	c_vec4 old_color = *color;
	c_rect rect;

	gui->begin_content(name, c_vec2(gui->content_avail().x, 0), s_(0, 10), s_(10, 10), 0, child_flags_always_auto_resize | child_flags_auto_resize_y);
	{
		rect = gui->get_window()->Rect();

		c_vec2 name_size = gui->text_size(font->get(inter_semibold, 12), name.data());
		gui->dummy(name_size);
		draw->text_clipped(window->DrawList, font->get(inter_semibold, 12), gui->get_item_rect().Min, gui->get_item_rect().Max,
			draw->get_clr(clr->white), name.data(), 0, 0, { 0, 0 });

		float width1 = (gui->content_avail().x - s_(10)) / 2;
		float width2 = (gui->content_avail().x - width1 - s_(20)) / 2;

		{
			custom_slider_t data = widgets->custom_slider("Saturation", &state->s, 0.f, 1.f, width2);
			draw->rect_filled_multi_color(window->DrawList, data.rect.Min, data.rect.Max, draw->get_clr(ImColor(255, 255, 255)),
				draw->get_clr(ImColor::HSV(state->h, 1.f, 1.f)), draw->get_clr(ImColor::HSV(state->h, 1.f, 1.f)), draw->get_clr(ImColor(255, 255, 255)), s_(999));
			draw->shadow_circle(window->DrawList, data.circle_pos, s_(4), draw->get_clr(clr->black), s_(3), c_vec2(0, 0));
			draw->circle(window->DrawList, data.circle_pos, s_(3), draw->get_clr(clr->white), s_(999), s_(2));
		}

		gui->sameline();

		{
			custom_slider_t data = widgets->custom_slider("Brightness", &state->v, 0.f, 1.f, width2);
			draw->rect_filled_multi_color(window->DrawList, data.rect.Min, data.rect.Max, draw->get_clr(ImColor(0, 0, 0)),
				draw->get_clr(ImColor(255, 255, 255)), draw->get_clr(ImColor(255, 255, 255)), draw->get_clr(ImColor(0, 0, 0)), s_(999));
			draw->shadow_circle(window->DrawList, data.circle_pos, s_(4), draw->get_clr(clr->black), s_(3), c_vec2(0, 0));
			draw->circle(window->DrawList, data.circle_pos, s_(3), draw->get_clr(clr->white), s_(999), s_(2));
		}

		gui->sameline();

		{
			custom_slider_t data = widgets->custom_slider("Hue", &state->h, 0.f, 1.f, width1);

			const int num_colors = 7;
			ImColor col_hues[num_colors];
			for (int i = 0; i < num_colors; ++i) {
				float current_hue = i / float(num_colors - 1);
				col_hues[i] = ImColor::HSV(current_hue, state->s, state->v);
			}

			for (int i = 0; i < num_colors - 1; ++i) {
				float rounding = (i == 0) || (i == num_colors - 2) ? s_(999) : 0;
				draw_flags flags = (i == 0) ? draw_flags_round_corners_left : (i == num_colors - 2) ? draw_flags_round_corners_right : 0;

				float x1 = roundf(data.rect.Min.x + i * (data.rect.GetWidth() / (num_colors - 1)));
				float x2 = roundf(data.rect.Min.x + (i + 1) * (data.rect.GetWidth() / (num_colors - 1)));

				draw->rect_filled_multi_color(window->DrawList, ImVec2(x1, data.rect.Min.y), ImVec2(x2, data.rect.Max.y), draw->get_clr(col_hues[i]), draw->get_clr(col_hues[i + 1]), draw->get_clr(col_hues[i + 1]), draw->get_clr(col_hues[i]), rounding, flags);
			}

			draw->shadow_circle(window->DrawList, data.circle_pos, s_(4), draw->get_clr(clr->black), s_(3), c_vec2(0, 0));
			draw->circle(window->DrawList, data.circle_pos, s_(3), draw->get_clr(clr->white), s_(999), s_(2));
		}

	}
	gui->end_content();

	if (gui->content_avail().y > 0)
	{
		draw->line(window->DrawList, rect.GetBL(), rect.GetBR(), draw->get_clr(clr->border));
	}

	ImGui::ColorConvertHSVtoRGB(state->h, state->s, state->v, color->x, color->y, color->z);

	return (old_color.x != color->x || old_color.y != color->y || old_color.z != color->z);
}


struct slider_state
{
	float normalized;
};

custom_slider_t c_widgets::custom_slider(std::string name, float* callback, float vmin, float vmax, float width)
{
	custom_slider_t data;

	c_window* window = gui->get_window();
	if (window->SkipItems)
		return data;

	c_id id = window->GetID(name.data());
	slider_state* state = gui->anim_container<slider_state>(id);

	c_vec2 pos = window->DC.CursorPos;
	c_vec2 size = c_vec2(width, s_(12));
	c_rect rect = c_rect(pos, pos + size);
	data.rect = rect;

	gui->item_size(rect);
	if (!gui->item_add(rect, id))
		return data;

	bool held, pressed = gui->button_behavior(rect, id, nullptr, &held);
	data.held = held;

	const float normalized = ImSaturate((gui->mouse_pos().x - rect.Min.x) / rect.GetWidth());

	if (held)
	{
		*callback = vmin + normalized * (vmax - vmin);
	}

	gui->easing(state->normalized, ImSaturate((*callback - vmin) / (vmax - vmin)), 24.f, dynamic_easing);

	float x = rect.GetWidth() * state->normalized;
	x = ImClamp(x, s_(12), rect.GetWidth());

	data.active = c_rect(rect.Min, rect.Min + c_vec2(x, rect.GetHeight()));
	data.circle_pos = c_vec2(rect.Min.x + x - s_(6), rect.GetCenter().y);

	return data;
}

bool c_widgets::slider(std::string name, std::string description, float* callback, float vmin, float vmax, std::string format)
{
	c_window* window = gui->get_window();
	if (window->SkipItems)
		return false;

	c_id id = window->GetID(name.data());
	slider_state* state = gui->anim_container<slider_state>(id);

	c_vec2 pos = window->DC.CursorPos;
	c_vec2 size = c_vec2(gui->content_avail().x, s_(68));
	c_rect rect = c_rect(pos, pos + size);
	c_rect inner = c_rect(rect.Min + s_(0, 10), rect.Max - s_(0, 10));
	c_rect button = c_rect(inner.GetBL() - s_(0, 12), inner.GetBR());

	gui->item_size(rect);
	if (!gui->item_add(rect, id))
		return false;

	bool held, pressed = gui->button_behavior(button, id, nullptr, &held);

	const float normalized = ImSaturate((gui->mouse_pos().x - button.Min.x) / button.GetWidth());

	if (held) 
	{
		*callback = vmin + normalized * (vmax - vmin);
	}

	gui->easing(state->normalized, ImSaturate((*callback - vmin) / (vmax - vmin)), 24.f, dynamic_easing);

	draw->text_clipped(window->DrawList, font->get(inter_semibold, 12), inner.Min, inner.Max, draw->get_clr(clr->white),
		name.data(), 0, 0, { 0, 0 });
	draw->text_clipped(window->DrawList, font->get(inter_medium, 11), inner.Min + s_(0, 17), inner.Max, draw->get_clr(clr->text),
		description.data(), 0, 0, { 0, 0 });

	char value_buf[64];
	gui->get_fmt(value_buf, callback, format);
	draw->text_clipped(window->DrawList, font->get(inter_semibold, 12), inner.Min, inner.Max, draw->get_clr(clr->white),
		value_buf, 0, 0, { 1, 0 });

	draw->rect_filled(window->DrawList, button.Min, button.Max, draw->get_clr(clr->widget), s_(999));
	{
		float x = button.GetWidth() * state->normalized;
		x = ImClamp(x, s_(12), button.GetWidth());

		draw->rect_filled(window->DrawList, button.Min, button.Min + c_vec2(x, button.GetHeight()),
			draw->get_clr(clr->accent), s_(999));

		draw->circle_filled(window->DrawList, c_vec2(button.Min.x + x - s_(6), button.GetCenter().y), s_(4), draw->get_clr(clr->black), s_(999));
	}

	if (gui->content_avail().y > 0)
	{
		draw->line(window->DrawList, rect.GetBL(), rect.GetBR(), draw->get_clr(clr->border));
	}

	return held;
}

struct selectable_state
{
	float alpha;
	c_vec4 text;
};

bool selectable_ex(std::string name, bool is_selected)
{
	c_window* window = gui->get_window();
	if (window->SkipItems)
		return false;

	c_id id = window->GetID(name.data());
	selectable_state* state = gui->anim_container< selectable_state>(id);

	c_vec2 pos = window->DC.CursorPos;
	c_vec2 size = c_vec2(gui->content_avail().x, s_(29));
	c_rect rect = c_rect(pos, pos + size);

	gui->item_size(rect);
	if (!gui->item_add(rect, id))
		return false;

	bool pressed = gui->button_behavior(rect, id, nullptr, nullptr);

	gui->easing(state->text, is_selected ? clr->white.Value : clr->text.Value, 24.f, dynamic_easing);
	gui->easing(state->alpha, is_selected ? 1.f : 0.f, 7.f, static_easing);

	if (gui->content_avail().y > 0)
	{
		draw->line(window->DrawList, rect.GetBL() + s_(10, 0), rect.GetBR() - s_(10, 0), draw->get_clr(clr->border));
	}

	//draw->rect_filled(window->DrawList, rect.Min, rect.Max, draw->get_clr(clr->cirlce, state->alpha), s_(2));

	draw->text_clipped(window->DrawList, font->get(inter_medium, 11), rect.Min + s_(10, 0), rect.Max, draw->get_clr(state->text),
		name.data(), 0, 0, { 0, 0.5 });


	return pressed;
}

struct dropdown_state
{
	bool open;
	float alpha;
};

bool dropdown_ex(std::string name, std::string description, std::string preview)
{
	c_window* window = gui->get_window();
	if (window->SkipItems)
		return false;

	c_id id = window->GetID(name.data());
	dropdown_state* state = gui->anim_container<dropdown_state>(id);

	c_vec2 pos = window->DC.CursorPos;
	c_vec2 size = c_vec2(gui->content_avail().x, s_(84));
	c_rect rect = c_rect(pos, pos + size);
	c_rect inner = c_rect(rect.Min + s_(0, 10), rect.Max - s_(0, 10));
	c_rect button = c_rect(inner.GetBL() - s_(0, 29), inner.GetBR());

	gui->item_size(rect);
	if (!gui->item_add(rect, id))
		return false;

	bool pressed = gui->button_behavior(button, id, nullptr, nullptr);

	if (pressed)
		state->open = true;

	draw->text_clipped(window->DrawList, font->get(inter_semibold, 12), inner.Min, inner.Max, draw->get_clr(clr->white),
		name.data(), 0, 0, { 0, 0 });
	draw->text_clipped(window->DrawList, font->get(inter_medium, 11), inner.Min + s_(0, 17), inner.Max, draw->get_clr(clr->text),
		description.data(), 0, 0, { 0, 0 });

	draw->rect_filled(window->DrawList, button.Min, button.Max, draw->get_clr(clr->widget), s_(2));
	draw->text_clipped(window->DrawList, font->get(inter_medium, 11), button.Min + s_(10, 0), button.Max, draw->get_clr(clr->white),
		preview.data(), 0, 0, { 0, 0.5 });
	draw->text_clipped(window->DrawList, font->get(icon_font, 12), button.Min, button.Max - s_(10, 0), draw->get_clr(clr->text),
		"A", 0, 0, {1, 0.5});

	if (gui->content_avail().y > 0)
	{
		draw->line(window->DrawList, rect.GetBL(), rect.GetBR(), draw->get_clr(clr->border));
	}

	gui->easing(state->alpha, state->open ? 1.f : 0.f, 7.f, static_easing);

	if (state->alpha > 0)
	{
		gui->push_var(style_var_alpha, state->alpha);
		gui->push_var(style_var_window_padding, s_(0, 0));
		gui->push_var(style_var_item_spacing, s_(0, 0));

		gui->set_next_window_pos(button.Min + s_(0, 10) * (1.f - state->alpha) + c_vec2(0, button.GetHeight() + s_(10)));
		gui->set_next_window_size(c_vec2(button.GetWidth(), 0));

		gui->begin(name, nullptr, window_flags_always_auto_resize | window_flags_no_decoration | window_flags_no_scrollbar | window_flags_no_scroll_with_mouse | window_flags_no_move | window_flags_no_background);
		{

			gui->set_window_focus();

			window = gui->get_window();
			rect = window->Rect();

			draw->rect_filled(window->DrawList, rect.Min, rect.Max, draw->get_clr(clr->widget), s_(2));

			if (!rect.Contains(gui->mouse_pos()) && gui->mouse_clicked(0))
				state->open = false;
		}

		return true;
	}

	return false;
}

void end_dropdown_ex()
{
	gui->end();
	gui->pop_var(3);
}

bool c_widgets::dropdown(std::string name, std::string description, int* callback, std::vector<std::string> variants)
{
	bool changed = false;

	if (dropdown_ex(name, description, variants[*callback]))
	{
		for (int i = 0; i < variants.size(); i++)
		{
			if (selectable_ex(variants[i], i == *callback))
			{
				if (*callback != i)
					changed = true;
				*callback = i;
			}
		}

		end_dropdown_ex();
	}

	return changed;
}


struct checkbox_state
{
	c_vec4 text;
	c_vec4 background;
	c_vec4 circle;
	float pos;
};

bool c_widgets::checkbox(std::string name, std::string description, bool* callback)
{
	c_window* window = gui->get_window();
	if (window->SkipItems)
		return false;

	c_id id = window->GetID(name.data());
	checkbox_state* state = gui->anim_container<checkbox_state>(id);

	c_vec2 pos = window->DC.CursorPos;
	c_vec2 size = c_vec2(gui->content_avail().x, s_(45));
	c_rect rect = c_rect(pos, pos + size);
	c_rect inner = c_rect(rect.Min + s_(0, 10), rect.Max - s_(0, 10));
	c_rect button = c_rect(c_vec2(inner.Max.x - s_(18), inner.GetCenter().y - s_(6)), c_vec2(inner.Max.x, inner.GetCenter().y + s_(6)));

	gui->item_size(rect);
	if (!gui->item_add(rect, id))
		return false;

	bool pressed = gui->button_behavior(inner, id, nullptr, nullptr);

	if (pressed)
	{
		*callback = !*callback;
	}

	gui->easing(state->pos, *callback ? 3.f : -3.f, 24.f, dynamic_easing);
	gui->easing(state->text, *callback ? clr->white.Value : clr->text.Value, 24.f, dynamic_easing);
	gui->easing(state->background, *callback ? clr->accent.Value : clr->widget.Value, 24.f, dynamic_easing);
	gui->easing(state->circle, *callback ? clr->black.Value : clr->cirlce.Value, 24.f, dynamic_easing);

	draw->text_clipped(window->DrawList, font->get(inter_semibold, 12), inner.Min, inner.Max, draw->get_clr(state->text),
		name.data(), 0, 0, { 0, 0 });
	draw->text_clipped(window->DrawList, font->get(inter_medium, 11), inner.Min, inner.Max, draw->get_clr(clr->text),
		description.data(), 0, 0, { 0, 1 });

	draw->rect_filled(window->DrawList, button.Min, button.Max, draw->get_clr(state->background), s_(999));
	draw->circle_filled(window->DrawList, c_vec2(button.GetCenter().x + s_(state->pos), button.GetCenter().y), s_(4), draw->get_clr(state->circle), s_(999));

	if (gui->content_avail().y > 0)
	{
		draw->line(window->DrawList, rect.GetBL(), rect.GetBR(), draw->get_clr(clr->border));
	}

	return pressed;
}

bool c_widgets::child(std::string name)
{
	c_window* window = gui->get_window();
	if (window->SkipItems)
		return false;

	c_id id = window->GetID(name.data());

	gui->begin_content(name, c_vec2(elements->child_width, 0), s_(10, 0), s_(0, 0), window_flags_no_scrollbar | window_flags_no_scroll_with_mouse | window_flags_no_move, child_flags_always_auto_resize | child_flags_auto_resize_y);
	{
		window = gui->get_window();

		draw->rect_filled(window->DrawList, window->Rect().Min, window->Rect().Max, draw->get_clr(clr->child), s_(2));
	}
	
	return true;
}

void c_widgets::end_child()
{
	gui->end_content();
}

struct tab_button_state
{
	float width;
	c_vec4 icon;
	c_vec4 background;
	float alpha;
};

bool c_widgets::tab_button(std::string name, std::string icon, int tab)
{
	c_window* window = gui->get_window();
	if (window->SkipItems)
		return false;

	c_id id = window->GetID(name.data());
	tab_button_state* state = gui->anim_container<tab_button_state>(id);

	bool is_selected = tab == var->gui.tab_stored;
	c_vec2 name_size = gui->text_size(font->get(inter_semibold, 12), name.data());
	gui->easing(state->width, is_selected ? s_(53) + name_size.x : s_(40), 24.f, dynamic_easing);

	c_vec2 pos = window->DC.CursorPos;
	c_vec2 size = c_vec2(round(state->width), s_(40));
	c_rect rect = c_rect(pos, pos + size);

	gui->item_size(rect);
	if (!gui->item_add(rect, id))
		return false;

	bool pressed = gui->button_behavior(rect, id, nullptr, nullptr);

	if (pressed)
	{
		if (tab == var->gui.tab)
		{
			var->gui.tab_stored = 0;
		}
		else
		{
			var->gui.tab_stored = tab;
		}
	}

	gui->easing(state->alpha, is_selected ? 1.f : 0.f, 7.f, static_easing);
	gui->easing(state->background, is_selected ? clr->widget.Value : clr->child.Value, 24.f, dynamic_easing);
	gui->easing(state->icon, is_selected ? clr->white.Value : clr->text.Value, 24.f, dynamic_easing);

	draw->rect_filled(window->DrawList, rect.Min, rect.Max, draw->get_clr(state->background), s_(2));
	draw->text_clipped(window->DrawList, font->get(icon_font, 12), rect.Min, rect.Min + s_(40, 40),
		draw->get_clr(state->icon), icon.data(), 0, 0, { 0.5, 0.5 });
	draw->text_clipped(window->DrawList, font->get(inter_semibold, 12), rect.Min + s_(40, 0), rect.Max - s_(13, 0),
		draw->get_clr(clr->white, state->alpha), name.data(), 0, 0, { 1, 0.5 });

	return pressed;
}

struct category_button_state
{
	c_vec4 text;
	c_vec4 background;
	c_vec4 button_text;
};

bool c_widgets::category_button(std::string name, std::string description, bool* callback)
{
	c_window* window = gui->get_window();
	if (window->SkipItems)
		return false;

	c_id id = window->GetID(name.data());
	category_button_state* state = gui->anim_container<category_button_state>(id);

	c_vec2 pos = window->DC.CursorPos;
	c_vec2 size = c_vec2(elements->tab_window_width, s_(82));
	c_rect rect = c_rect(pos, pos + size);
	c_rect inner = c_rect(rect.Min + s_(elements->padding), rect.Max - s_(elements->padding));
	c_rect button = c_rect(inner.GetBL() - s_(0, 27), inner.GetBR());

	gui->item_size(rect);
	if (!gui->item_add(rect, id))
		return false;

	bool pressed = gui->button_behavior(button, id, nullptr, nullptr);

	if (pressed)
	{
		*callback = !*callback;
	}

	gui->easing(state->text, *callback ? clr->white.Value : clr->text.Value, 24.f, dynamic_easing);
	gui->easing(state->background, *callback ? clr->accent.Value : clr->widget.Value, 24.f, dynamic_easing);
	gui->easing(state->button_text, *callback ? clr->black.Value : clr->white.Value, 24.f, dynamic_easing);

	draw->rect_filled(window->DrawList, rect.Min, rect.Max, draw->get_clr(clr->child), s_(2));

	draw->text_clipped(window->DrawList, font->get(inter_semibold, 12), inner.Min, inner.Max, draw->get_clr(state->text),
		name.data(), 0, 0, { 0, 0 });
	draw->text_clipped(window->DrawList, font->get(inter_medium, 11), inner.Min + s_(0, 16), inner.Max, draw->get_clr(clr->text),
		description.data(), 0, 0, { 0, 0 });

	draw->rect_filled(window->DrawList, button.Min, button.Max, draw->get_clr(state->background), s_(2));
	draw->text_clipped(window->DrawList, font->get(inter_medium, 11), button.Min, button.Max, draw->get_clr(state->button_text),
		*callback ? "Enabled" : "Disabled", 0, 0, {0.5, 0.5});

	return pressed;
}