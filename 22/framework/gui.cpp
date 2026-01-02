#include "headers/includes.h"

void c_gui::render()
{
	gui->initialize();

	gui->easing(var->gui.tab_alpha, var->gui.tab != var->gui.tab_stored ? 0.f : 1.f, 7.f, static_easing);
	if (var->gui.tab_alpha == 0)
		var->gui.tab = var->gui.tab_stored;

	static std::vector<std::vector<bool>> cc = {
		{ false, false, false },
		{ false, false, false },
		{ false, false, false },
		{ }
	};

	c_vec2 size = c_vec2(0, 0);

	if (var->gui.tab_stored == 0)
	{
		size.x = elements->tab_window_width + s_(20);
		size.y = s_(60);
	}
	else
	{
		size.y = s_(338);
		if (cc[var->gui.tab_stored - 1].size() > 0)
		{
			bool is_category_active = false;
			for (int i = 0; i < cc[var->gui.tab_stored - 1].size(); i++)
			{
				if (cc[var->gui.tab_stored - 1][i])
				{
					is_category_active = true;
					break;
				}
			}
			if (is_category_active)
			{
				size.x = s_(700);
			}
			else
			{
				size.x = elements->tab_window_width + s_(20);
			}
		}
		else
		{
			size.x = s_(700);
		}
	}

	gui->easing(elements->window.size.x, size.x, 24.f, dynamic_easing);
	gui->easing(elements->window.size.y, size.y, 24.f, dynamic_easing);

	gui->set_next_window_size(elements->window.size);

	gui->begin(elements->window.name, nullptr, window_flags_no_scrollbar | window_flags_no_scroll_with_mouse | window_flags_no_bring_to_front_on_focus | window_flags_no_focus_on_appearing | window_flags_no_background | window_flags_no_decoration);
	{
		gui->set_style();
		gui->draw_decorations();

		c_window* window = gui->get_window();

		gui->begin_group();

		gui->begin_content("Tabs", s_(0, 40), s_(0, 0), s_(10, 10), 0, child_flags_always_auto_resize | child_flags_auto_resize_x);
		{
			widgets->tab_button("Combat", "E", 1);
			gui->sameline();
			widgets->tab_button("Visuals", "F", 2);
			gui->sameline();
			widgets->tab_button("Misc", "B", 3);
			gui->sameline();
			widgets->tab_button("Settings", "C", 4);

			gui->easing(elements->tab_window_width, gui->get_window()->Size.x, 24.f, dynamic_easing);
		}
		gui->end_content();
		
		gui->push_var(style_var_alpha, var->gui.tab_alpha);

		gui->set_pos(gui->get_pos().y + s_(10) * (1.f - var->gui.tab_alpha), pos_y);
		gui->begin_content("Categories", s_(0, 0), s_(0, 0), s_(elements->padding), window_flags_no_scrollbar | window_flags_no_scroll_with_mouse, child_flags_always_auto_resize | child_flags_auto_resize_y | child_flags_auto_resize_x);

		if (var->gui.tab == 1)
		{
			{
				bool enabled = cc[0][0];
				widgets->category_button("Enable aimbot", "Enable all aimbot features", &enabled);
				cc[0][0] = enabled;
			}

			{
				bool enabled = cc[0][1];
				widgets->category_button("Enable triggerbot", "Automatic fire", &enabled);
				cc[0][1] = enabled;
			}

			{
				bool enabled = cc[0][2];
				widgets->category_button("Enable RCS", "Recoil control", &enabled);
				cc[0][2] = enabled;
			}
		}

		if (var->gui.tab == 2)
		{
			{
				bool enabled = cc[1][0];
				widgets->category_button("Enable world customization", "Nightmode, modulation ...", &enabled);
				cc[1][0] = enabled;
			}

			{
				bool enabled = cc[1][1];
				widgets->category_button("Enable esp", "Players screen info", &enabled);
				cc[1][1] = enabled;
			}

			{
				bool enabled = cc[1][2];
				widgets->category_button("Enable chams", "Players model texture", &enabled);
				cc[1][2] = enabled;
			}
		}

		if (var->gui.tab == 3)
		{
			{
				bool enabled = cc[2][0];
				widgets->category_button("Enable movement", "Edge jump, jump bug, bhop ...", &enabled);
				cc[2][0] = enabled;
			}

			{
				bool enabled = cc[2][1];
				widgets->category_button("Enable helpers", "Fast reload, no ragdoll ...", &enabled);
				cc[2][1] = enabled;
			}

			{
				bool enabled = cc[2][2];
				widgets->category_button("Enable radio", "Play with music", &enabled);
				cc[2][2] = enabled;
			}
		}

		gui->end_content();

		gui->end_group();

		gui->sameline();

		if (var->gui.tab != 0)
		{
			gui->set_pos(gui->get_pos().y + s_(10) * (1.f - var->gui.tab_alpha), pos_y);
			gui->begin_content("Features", s_(670, 316) - c_vec2(elements->tab_window_width, 0), s_(0, 0), s_(elements->padding));
			{
				gui->easing(elements->child_width, (gui->content_avail().x - s_(10)) / 2, 24.f, dynamic_easing);

				if (var->gui.tab == 1)
				{
					gui->begin_group();
					{
						if (widgets->child("1"))
						{
							static bool callback1;
							widgets->checkbox("Silent aimbot", "Doesn't change your review", &callback1);

							static float callback2;
							widgets->slider("Max distance", "Maximal aimbot distance", &callback2, 0.f, 100.f);

							static float callback3;
							widgets->slider("Field of view", "Game fov", &callback3, 0.f, 180.f);

							static bool callback4;
							widgets->checkbox("Auto shoot", "Automatic fire when aimbot working", &callback4);

							static bool callback5;
							widgets->checkbox("Ignore tean", "Enable team fire", &callback5);

							static int callback6;
							widgets->dropdown("Bone", "Bone for aimbot target", &callback6, { "Head", "Chest", "Stomach", "Arms", "Legs" });

							widgets->end_child();
						}

						if (widgets->child("2"))
						{
							static bool callback1;
							static bool callback2;
							static bool callback3;

							widgets->checkbox("Aim through smoke", "Ignore smoke", &callback1);
							widgets->checkbox("Aim while flash", "Ignore flash", &callback2);
							widgets->checkbox("Standalone RCS", "Recoil control", &callback3);

							widgets->end_child();
						}

						if (widgets->child("6"))
						{
							static bool callback1;

							widgets->checkbox("Smart aimbot", "Humanized aimbot mode", &callback1);

							widgets->end_child();
						}
					}
					gui->end_group();

					gui->sameline();

					gui->begin_group();
					{
						if (widgets->child("4"))
						{
							static float callback1;
							widgets->slider("Aimbot fov", "Aimbot target radius", &callback1, 0.f, 180.f);

							static float callback2;
							widgets->slider("Smoothness", "Aimbot smooth", &callback2, 0.f, 100.f);

							static float callback3;
							widgets->slider("Rcs x", "Recoil control on x", &callback3, 0.f, 100.f);

							static float callback4;
							widgets->slider("Rcs y", "Recoil control on y", &callback4, 0.f, 100.f);

							static bool callback5;
							widgets->checkbox("Recoil system based FOV", "Recoil on fov based", &callback5);

							static int callback6;
							widgets->dropdown("Aimbot mode", "Aimbot mode", &callback6, { "Nearest screen", "Nearest crosshair", "Low HP" });

							widgets->end_child();
						}

						if (widgets->child("5"))
						{
							static bool callback1;
							widgets->checkbox("Adaptive recoil", "Humanized recoil control", &callback1);

							static float callback2;
							widgets->slider("Vectical efficency", "Vectical efficency", &callback2, 0.f, 100.f);

							static float callback3;
							widgets->slider("Horizontal efficency", "Horizontal efficency", &callback3, 0.f, 100.f);

							static float callback4;
							widgets->slider("Variance", "Recoil variance", &callback4, 0.f, 100.f);

							static bool callback5;
							widgets->checkbox("Standalone mode", "Standalone mode", &callback5);

							static bool callback6;
							widgets->checkbox("Override verticaly", "Override verticaly", &callback6);

							widgets->end_child();
						}
					}
					gui->end_group();

				}

				if (var->gui.tab == 2)
				{
					gui->begin_group();
					{
						if (widgets->child("1"))
						{
							static bool callback1;
							widgets->checkbox("Animated esp", "Smooth player targeting", &callback1);

							static bool callback2;
							widgets->checkbox("Only visible", "Esp when player is visible", &callback2);

							static float callback3;
							widgets->slider("Max distance", "Esp maximal distance", &callback3, 0.f, 1000.f);

							static bool callback4;
							widgets->checkbox("3D esp", "3D mode", &callback4);

							widgets->end_child();
						}

						if (widgets->child("2"))
						{
							static bool callback1;
							widgets->checkbox("Enable box", "Box", &callback1);

							static c_vec4 callback2 = clr->accent.Value;
							widgets->color_picker("Box color", &callback2);

							static bool callback3;
							widgets->checkbox("Enable name", "Name", &callback3);

							static c_vec4 callback4 = clr->accent.Value;
							widgets->color_picker("Name color", &callback4);

							widgets->end_child();
						}
					}
					gui->end_group();

					gui->sameline();

					gui->begin_group();
					{
						if (widgets->child("4"))
						{
							static bool callback5;
							widgets->checkbox("Enable chams", "Chams", &callback5);

							static c_vec4 callback6 = clr->accent.Value;
							widgets->color_picker("Chams color", &callback6);

							static int callback7;
							widgets->dropdown("Chams type", "Chams material type", &callback7, { "Flat", "Glow", "Glass", "Material", "Custom" });

							widgets->end_child();
						}

						if (widgets->child("5"))
						{
							static bool callback5;
							widgets->checkbox("Enable skeleton", "Skeleton", &callback5);

							static c_vec4 callback6 = clr->accent.Value;
							widgets->color_picker("Skeleton color", &callback6);

							widgets->end_child();
						}
					}
					gui->end_group();
				}

				if (var->gui.tab == 3)
				{
					gui->begin_group();
					{
						if (widgets->child("1"))
						{
							static char buf[64];
							widgets->text_field("Clan tag", buf, sizeof(buf));

							static c_vec4 accent_color = clr->accent.Value;
							widgets->color_picker("Accent color", &accent_color);
							clr->accent.Value = accent_color;

							static float callback6 = 100.f;
							if (!widgets->slider("Scale", "Interface dpi scale", &callback6, 100.f, 200.f, "%.f%"))
							{
								if (gui->mouse_released(0))
								{
									var->gui.stored_dpi = callback6;
									var->gui.dpi_changed = true;
								}
							}

							widgets->end_child();
						}
					}
					gui->end_group();
				}
			}
			gui->end_content();

		}
		gui->pop_var();

	}
	gui->end();
}