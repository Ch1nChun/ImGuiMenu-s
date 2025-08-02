#include "settings/functions.h"
#include "shader/blur.hpp"

void c_gui::render()
{

	gui->new_frame();
	{
		notify->setup_notify();

		draw->add_image(GetBackgroundDrawList(), set->c_texture.bg, { 0, 0 }, { 1920, 1080 }, { 0, 0 }, { 1, 1 }, gui->get_clr(clr->c_other_clr.white_clr));

		gui->set_next_window_size(SCALE(set->c_window.window_size));

		gui->begin({ "NAME" }, { 0 }, set->c_window.window_flags);
		{
			const ImVec2 pos = GetWindowPos();
			const ImVec2 size = GetWindowSize();

			ImDrawList* draw_list = GetWindowDrawList();
			ImGuiStyle* style = &GetStyle();

			{
				style->WindowBorderSize = SCALE(set->c_window.border_size);
				style->WindowRounding = SCALE(set->c_window.rounding);
				style->WindowPadding = SCALE(set->c_window.padding);

				style->ScrollbarSize = SCALE(set->c_window.scrollbar_size);
				style->ItemSpacing = SCALE(set->c_window.item_spacing);
			}

			draw_background_blur(draw_list, g_pSwapChain, g_pd3dDevice, g_pd3dDeviceContext, GetWindowPos(), GetWindowPos() + GetWindowSize(), style->WindowRounding);
			
			draw->add_rect_filled(draw_list, { pos.x, pos.y }, { pos.x + size.x, pos.y + size.y }, gui->get_clr(clr->c_window.general_layout), SCALE(set->c_window.general_rounding));
			draw->add_rect(draw_list, { pos.x, pos.y }, { pos.x + size.x, pos.y + size.y }, gui->get_clr(clr->c_window.general_stroke), SCALE(set->c_window.general_rounding));

			draw->add_rect_filled(draw_list, { pos.x + SCALE(110), pos.y + SCALE(15) }, { pos.x + (size.x - SCALE(15)), pos.y + (size.y - SCALE(15)) }, gui->get_clr(clr->c_window.layout), SCALE(set->c_window.rounding));
			draw->add_rect(draw_list, { pos.x + SCALE(110), pos.y + SCALE(15) }, { pos.x + (size.x - SCALE(15)), pos.y + (size.y - SCALE(15)) }, gui->get_clr(clr->c_window.stroke), SCALE(set->c_window.rounding));

			draw->rect_filled_multi_color(draw_list, { pos.x + size.x / 2, pos.y }, { pos.x + size.x, pos.y + 1 }, gui->get_clr(clr->c_other_clr.accent_clr, 0.2f), gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr, 0.2f));
			draw->rect_filled_multi_color(draw_list, { pos.x, pos.y }, { pos.x + size.x / 2, pos.y + 1 }, gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr, 0.2f), gui->get_clr(clr->c_other_clr.accent_clr, 0.2f), gui->get_clr(clr->c_other_clr.accent_clr, 0.f));

			draw->rect_filled_multi_color(draw_list, { pos.x + size.x / 2, pos.y + size.y - 1 }, { pos.x + size.x, pos.y + size.y }, gui->get_clr(clr->c_other_clr.accent_clr, 0.2f), gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr, 0.2f));
			draw->rect_filled_multi_color(draw_list, { pos.x, pos.y + size.y - 1 }, { pos.x + size.x / 2, pos.y + size.y }, gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr, 0.2f), gui->get_clr(clr->c_other_clr.accent_clr, 0.2f), gui->get_clr(clr->c_other_clr.accent_clr, 0.f));

			draw->render_text(draw_list, set->c_font.icon[2], { pos.x, pos.y }, { pos.x + SCALE(110), pos.y + size.y }, gui->get_clr(clr->c_other_clr.accent_clr), "B", 0, 0, { 0.5, 0.5 });

			gui->push_font(set->c_font.name);

			const int vtx_start_one = draw_list->VtxBuffer.Size;
			gui->rotate_start();
			draw->render_text_with_spacing(set->c_window.name.c_str(), SCALE(3.f), pos, pos + ImVec2(SCALE(110), size.y / 2), ImColor(255, 255, 255), true);
			gui->rotate_end(gui->deg_to_rad(-90));
			const int vtx_end_one = draw_list->VtxBuffer.Size;
			draw->set_linear_color_alpha(draw_list, vtx_start_one, vtx_end_one, pos + ImVec2(0, size.y / 6), pos + ImVec2(0, size.y / 2), gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr));

			const int vtx_start_two = draw_list->VtxBuffer.Size;
			draw->render_text_with_spacing(set->c_window.name.c_str(), SCALE(3.f), pos + ImVec2(0, size.y / 2), pos + ImVec2(SCALE(110), size.y), ImColor(255, 255, 255), true);
			const int vtx_end_two = draw_list->VtxBuffer.Size;
			draw->set_linear_color_alpha(draw_list, vtx_start_two, vtx_end_two, pos + ImVec2(0, size.y / 2), pos + ImVec2(0, size.y / 2 + size.y / 3), gui->get_clr(clr->c_other_clr.accent_clr), gui->get_clr(clr->c_other_clr.accent_clr, 0.f));

			gui->pop_font();

			gui->set_cursor_pos(SCALE(110, 15));

			var->c_selection.selection_alpha = ImClamp(var->c_selection.selection_alpha + (4.f * ImGui::GetIO().DeltaTime * (var->c_selection.selection == var->c_selection.selection_active ? 1.f : -1.f)), 0.f, 1.f);
			if (var->c_selection.selection_alpha == 0.f && var->c_selection.selection_add == 0.f) var->c_selection.selection_active = var->c_selection.selection;

			gui->push_style_var(ImGuiStyleVar_Alpha, var->c_selection.selection_alpha * style->Alpha);

			gui->begin_content("content", GetContentRegionAvail() - SCALE(15, 15), { 15, 15 }, { 15, 15 });
			{
				if (var->c_selection.selection_active == 0)
				{
					gui->begin_group();
					{

						gui->begin_child("ragebot");
						{

							if (widget->checkbox_with_key("Enable ragebot", &var->c_ragebot.ragebot, &var->c_ragebot.rage_key, &var->c_ragebot.rage_holding, &var->c_ragebot.rage_value, &var->c_ragebot.rage_show_binds))
							{
								notify->add_notify("You have successfully summoned a notification!", 15, static_cast<notify_position>(var->c_notify.notify_position));
							};

							widget->separator();

							widget->checkbox("Silent aimbot", &var->c_ragebot.silent_aimbot);

							widget->separator();

							widget->checkbox("Hit chance", &var->c_ragebot.hit_chance);

							widget->separator();

							widget->slider_int("Field of view", &var->c_ragebot.fov, -180, 180, 1, "%d\xC2\xB0");
						}
						gui->end_child();

						gui->begin_child("other");
						{

							widget->dropdown("Body aimbot", &var->c_other.body_selection, var->c_other.body_list, var->c_other.body_list.size());

							widget->separator();

							widget->dropdown("Safe points", &var->c_other.points_selection, var->c_other.points_list, var->c_other.points_list.size());

						}
						gui->end_child();

						gui->begin_child("recoil");
						{

							widget->checkbox_with_key("Enable recoil", &var->c_recoil.enable_recoil, &var->c_recoil.recoil_key, &var->c_recoil.recoil_holding, &var->c_recoil.recoil_value, &var->c_recoil.recoil_show_binds);

							widget->separator();

							widget->slider_int("Smoothness", &var->c_recoil.smoothness, 0, 100, 1, "%d%%");

						}
						gui->end_child();

					}
					gui->end_group();

					gui->sameline();

					gui->begin_group();
					{

						gui->begin_child("move");
						{

							widget->slider_int("Hit chance", &var->c_move.hit_chance, 0, 100, 1, "%d%%");
							{
								widget->set_tooltip("Hit chance", "It displays a tooltip when you hover over an item, a very handy GUI utility\nwith many functions that are not always easy to understand by name.");
							}

							widget->separator();

							widget->slider_int("Max misses", &var->c_move.max_misses, 0, 100, 1, "%d%%");

							widget->separator();

							widget->checkbox_with_key("Static point scale", &var->c_move.point_scale, &var->c_move.point_key, &var->c_move.point_holding, &var->c_move.point_value, &var->c_move.point_show_binds);

							widget->separator();

							widget->checkbox("Head safety if lethal", &var->c_move.head_safety);

						}
						gui->end_child();

						gui->begin_child("trigger");
						{

							widget->checkbox_with_key("Enable triggerbot", &var->c_trigger.enable_trigger, &var->c_trigger.trigger_key, &var->c_trigger.trigger_holding, &var->c_trigger.trigger_value, &var->c_trigger.trigger_show_binds);

							widget->separator();

							widget->checkbox("Enable trigger in smoke", &var->c_trigger.trigger_in_smoke);

							widget->separator();

							widget->button("Button", { GetContentRegionAvail().x, SCALE(35) });

						}
						gui->end_child();

						gui->begin_child("settings");
						{

							widget->slider_float("Pitch", &var->c_settings.pitch, 0.f, 1.f, 0.1f, "%.3f");

							widget->separator();

							widget->slider_float("Yaw", &var->c_settings.yaw, 0.f, 1.f, 0.1f, "%.3f");

							widget->separator();

							widget->checkbox_with_key("Static point scale", &var->c_settings.point_scale, &var->c_settings.point_key, &var->c_settings.point_holding, &var->c_settings.point_value, &var->c_settings.point_show_binds);

							widget->separator();

							widget->checkbox("Head safety if lethal", &var->c_settings.head_safety);
						}
						gui->end_child();

					}
					gui->end_group();
				}
				else if (var->c_selection.selection_active == 3)
				{
					gui->begin_group();
					{

						gui->begin_child("esp");
						{
							widget->checkbox_with_key("Enable ESP", &var->c_esp.esp, &var->c_esp.esp_key, &var->c_esp.esp_holding, &var->c_esp.esp_value, &var->c_esp.esp_show_binds);

							widget->separator();

							widget->checkbox("Through walls", &var->c_esp.through_walls);

							widget->separator();

							widget->dropdown("Dynamic tracer", &var->c_esp.tracers_selection, var->c_esp.tracers_list, var->c_esp.tracers_list.size());

							widget->separator();

							widget->checkbox("Dynamic boxes", &var->c_esp.dynamic_boxes);

							widget->separator();

							widget->checkbox_with_color("In-Game radar", &var->c_esp.ingame_radar, var->c_esp.inagame_color, true);
						}
						gui->end_child();

						gui->begin_child("glow");
						{
							widget->checkbox_with_key("Enable glow", &var->c_glow.glow, &var->c_glow.glow_key, &var->c_glow.glow_holding, &var->c_glow.glow_value, &var->c_glow.glow_show_binds);

							widget->separator();

							widget->slider_int("The power of brightness", &var->c_glow.power, 0, 100, 1, "%d%$");
						}
						gui->end_child();

						gui->begin_child("attachments");
						{
							widget->checkbox_with_color("Attachments", &var->c_attachments.attachments, var->c_attachments.attachments_color, true);

							widget->separator();

							widget->checkbox_with_key("Visible teammates", &var->c_attachments.teammates, &var->c_attachments.teammates_key, &var->c_attachments.teammates_holding, &var->c_attachments.teammates_value, &var->c_attachments.teammates_show_binds);
						}
						gui->end_child();

					}
					gui->end_group();

					gui->sameline();

					gui->begin_group();
					{

						gui->begin_child("chams");
						{
							widget->checkbox_with_key("Enable chams", &var->c_chams.chams, &var->c_chams.chams_key, &var->c_chams.chams_holding, &var->c_chams.chams_value, &var->c_chams.chams_show_binds);

							widget->separator();

							widget->checkbox_with_color("Backtrack", &var->c_chams.backtrack, var->c_chams.backtrack_color, true);

							widget->separator();

							widget->checkbox_with_color("On shot", &var->c_chams.onshot, var->c_chams.onshot_color, true);

							widget->separator();

							widget->checkbox_with_color("Ragdolls", &var->c_chams.ragdolls, var->c_chams.ragdolls_color, true);

						}
						gui->end_child();

						gui->begin_child("skeleton");
						{
							widget->checkbox("Skeleton", &var->c_skeleton.skeleton);

							widget->separator();

							widget->dropdown("Snaplines", &var->c_skeleton.snaplines_selection, var->c_skeleton.snaplines_list, var->c_skeleton.snaplines_list.size());
						
							widget->separator();

							widget->checkbox_with_color("Weapon", &var->c_skeleton.weapon, var->c_skeleton.weapon_color, true);

							widget->separator();

							widget->checkbox_with_color("Nickname", &var->c_skeleton.nickname, var->c_skeleton.nickname_color, true);
						}
						gui->end_child();

					}
					gui->end_group();
				}
				else if (var->c_selection.selection_active == 4)
				{

					draw->add_line(GetWindowDrawList(), GetWindowPos() + ImVec2(GetStyle().WindowPadding.x, SCALE(64)), GetWindowPos() + ImVec2(GetWindowWidth() - GetStyle().WindowPadding.x, SCALE(64)), gui->get_clr(clr->c_child.stroke), SCALE(1.f));

					widget->tool_dropdown("Sort", &var->c_lua.sort_selection, var->c_lua.sort_list, var->c_lua.sort_list.size());

					gui->sameline();
					
					if (widget->tool_button("Create", "A", SCALE(90, 36)))
						var->c_lua.create = true;
					
					if (var->c_lua.create)
					{
						gui->set_next_window_size(SCALE(310, 80));
						gui->set_next_window_pos(GetWindowPos() + (GetWindowSize() / 2 - SCALE(310, 80) / 2));
						gui->push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
						gui->begin("Create", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
						{
							draw->add_rect_filled(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), gui->get_clr(clr->c_child.layout), SCALE(set->c_child.rounding));
							draw->add_rect(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), gui->get_clr(clr->c_child.stroke), SCALE(set->c_child.rounding), 0, SCALE(1.f));

							gui->set_cursor_pos(SCALE(20, 20));
							gui->begin_group();
							{
								widget->text_field("Lua Name", "M", var->c_lua.name, 128, SCALE(180, 40));
								gui->sameline();
								ImGui::PushID("Create Lua");
								if (widget->button("Create", SCALE(75, 40)))
								{
									std::string name(var->c_lua.name);
									var->c_lua.data.insert(var->c_lua.data.begin(), { name, gui->get_current_date(), false });
									var->c_lua.create = false;
								}
								ImGui::PopID();
							}
							gui->end_group();

							if (!IsMouseHoveringRect(GetWindowPos(), GetWindowPos() + GetWindowSize()) && (IsMouseClicked(0) || IsMouseClicked(1)))
								var->c_lua.create = false;

						}
						gui->end();
						gui->pop_style_var();
					}

					gui->set_cursor_pos_y(SCALE(80));
					gui->begin_group();
					{
						for (int i = 0; i < var->c_lua.data.size(); i++)
						{
							if (var->c_lua.sort_selection == 0)
							{
								if (widget->lua_selectable(&var->c_lua.data.at(i), i))
								{
									var->c_lua.editable = var->c_lua.data.at(i).name;
									var->c_lua.opened = true;
								}
							}
							else if (var->c_lua.sort_selection == 1)
							{
								if (widget->lua_selectable(&var->c_lua.data.at(var->c_lua.data.size() - i - 1), i))
								{
									var->c_lua.editable = var->c_lua.data.at(i).name;
									var->c_lua.opened = true;
								}
							}
						}
					}
					gui->end_group();

					if (var->c_lua.opened)
					{
						gui->set_next_window_size(SCALE(500, 520));
						gui->set_next_window_pos(pos + ImVec2(size.x + SCALE(20), 0));
						gui->push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
						gui->begin("Text Editor", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
						{
							draw->add_rect_filled(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), gui->get_clr(clr->c_window.general_layout), SCALE(set->c_window.rounding));
							draw->add_rect(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), gui->get_clr(clr->c_window.general_stroke), SCALE(set->c_window.rounding), 0, SCALE(1.f));
							draw->add_line(GetWindowDrawList(), GetWindowPos() + SCALE(0, 40), GetWindowPos() + ImVec2(GetWindowSize().x, SCALE(40)), gui->get_clr(clr->c_window.general_stroke), SCALE(1.f));
							draw->add_line(GetWindowDrawList(), GetWindowPos() + ImVec2(0, GetWindowSize().y - SCALE(40)), GetWindowPos() + ImVec2(GetWindowSize().x, GetWindowSize().y - SCALE(40)), gui->get_clr(clr->c_window.general_stroke), SCALE(1.f));
							draw->render_text(GetWindowDrawList(), set->c_font.inter_medium[0], GetWindowPos() + SCALE(40, 0), GetWindowPos() + ImVec2(GetWindowWidth(), SCALE(40)), gui->get_clr(clr->c_text.text_active), (std::stringstream{} << "Lua Editor - " << var->c_lua.editable).str().c_str(), NULL, NULL, ImVec2(0.f, 0.5f));
							draw->render_text(GetWindowDrawList(), set->c_font.icon[4], GetWindowPos() + SCALE(15, 1), GetWindowPos() + ImVec2(GetWindowWidth(), SCALE(40)), gui->get_clr(clr->c_other_clr.accent_clr), "B", NULL, NULL, ImVec2(0.f, 0.5f));

							gui->set_cursor_pos(ImVec2(GetWindowWidth() - SCALE(30), 13));
							if (widget->lua_tool_button("E", "Close Button"))
								var->c_lua.opened = false;

							gui->set_cursor_pos(SCALE(10, 50));
							text_editor->render("Text Editor", SCALE(480, 420));

							gui->set_cursor_pos(GetWindowSize() - SCALE(122, 34));
							if (widget->tool_button("Save", "", SCALE(54, 28)))
							{

							}

							gui->set_cursor_pos(GetWindowSize() - SCALE(61, 34));
							if (widget->tool_button("Run", "", SCALE(54, 28)))
							{

							}
						}
						gui->end();
						gui->pop_style_var();
					}
				}
				else if (var->c_selection.selection_active == 5)
				{

					draw->add_line(GetWindowDrawList(), GetWindowPos() + ImVec2(GetStyle().WindowPadding.x, SCALE(64)), GetWindowPos() + ImVec2(GetWindowWidth() - GetStyle().WindowPadding.x, SCALE(64)), gui->get_clr(clr->c_child.stroke), SCALE(1.f));
					widget->tool_dropdown("Sort", &var->c_config.sort_selection, var->c_config.sort_list, var->c_config.sort_list.size());

					gui->sameline();

					if (widget->tool_button("Create", "A", SCALE(90, 36)))
						var->c_config.create = true;

					if (var->c_config.create)
					{
						gui->set_next_window_size(SCALE(310, 80));
						gui->set_next_window_pos(GetWindowPos() + (GetWindowSize() / 2 - SCALE(310, 80) / 2));
						gui->push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
						gui->begin("Create", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
						{
							draw->add_rect_filled(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), gui->get_clr(clr->c_child.layout), SCALE(set->c_child.rounding));
							draw->add_rect(GetWindowDrawList(), GetWindowPos(), GetWindowPos() + GetWindowSize(), gui->get_clr(clr->c_child.stroke), SCALE(set->c_child.rounding), 0, SCALE(1.f));

							gui->set_cursor_pos(SCALE(20, 20));
							gui->begin_group();
							{
								widget->text_field("Config Name", "M", var->c_config.name, 128, SCALE(180, 40));
								gui->sameline();
								ImGui::PushID("Create Config");
								if (widget->button("Create", SCALE(75, 40)))
								{
									std::string name(var->c_config.name);
									var->c_config.data.insert(var->c_config.data.begin(), { name, gui->get_current_date() });
									var->c_config.create = false;
								}
								ImGui::PopID();
							}
							gui->end_group();

							if (!IsMouseHoveringRect(GetWindowPos(), GetWindowPos() + GetWindowSize()) && (IsMouseClicked(0) || IsMouseClicked(1)))
								var->c_config.create = false;

						}
						gui->end();
						gui->pop_style_var();
					}

					gui->set_cursor_pos_y(SCALE(80));
					gui->begin_group();
					{
						for (int i = 0; i < var->c_config.data.size(); i++)
						{
							if (var->c_config.sort_selection == 0)
								widget->config_selectable(&var->c_config.data.at(i), i, var->c_config.active);
							else if (var->c_config.sort_selection == 1)
								widget->config_selectable(&var->c_config.data.at(var->c_config.data.size() - i - 1), var->c_config.data.size() - i - 1, var->c_config.active);
						}
					}
					gui->end_group();
				}
				else if (var->c_selection.selection_active == 6)
				{
					gui->begin_group();
					{

						gui->begin_child("oth");
						{
							static char buf[128];
							widget->text_field("Enter your text", "M", buf, 128, { GetContentRegionAvail().x, SCALE(35) });

							widget->separator();

							widget->button("Button", { GetContentRegionAvail().x, SCALE(35) });

							widget->separator();

							const float width = GetContentRegionAvail().x;

							widget->button("Press", { (width - style->ItemSpacing.x) / 2, SCALE(35) });

							gui->sameline();

							widget->button("Click", { (width - style->ItemSpacing.x) / 2, SCALE(35) });
						}
						gui->end_child();

					}
					gui->end_group();

					gui->sameline();

					gui->begin_group();
					{

						gui->begin_child("dpi");
						{
							widget->slider_int("DPI", &var->c_dpi.dpi_saved, 100, 200, 1, "%d%%");

							if (var->c_dpi.dpi != var->c_dpi.dpi_saved / 100.f && IsMouseReleased(ImGuiMouseButton_Left)) {

								notify->add_notify("You have successfully set the menu size", 15, static_cast<notify_position>(var->c_notify.notify_position));
								var->c_dpi.dpi_changed = true;

							}
						}
						gui->end_child();

					}
					gui->end_group();
				}
			}
			gui->end_content();

			gui->pop_style_var(2);

		}
		gui->end();

		gui->push_style_var(ImGuiStyleVar_WindowPadding, SCALE(15, 15));
		gui->push_style_var(ImGuiStyleVar_ItemSpacing, SCALE(4, 0));
		{
			gui->set_next_window_pos(SCALE(20, 20));
			gui->begin({ "SELECTION" }, { 0 }, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
			{
				const ImVec2 pos = GetWindowPos();
				const ImVec2 size = GetWindowSize();

				ImDrawList* draw_list = GetBackgroundDrawList();

				gui->set_cursor_pos_x(SCALE(54));
				gui->begin_group();
				{
					widget->selection(var->c_selection.selection_icon[0].data(), ImVec2(74, 76), 0, var->c_selection.selection);

					gui->sameline();

					widget->selection(var->c_selection.selection_icon[1].data(), ImVec2(74, 76), 1, var->c_selection.selection);
				}
				gui->end_group();

				gui->set_cursor_pos_y(get_cursor_pos_y() - SCALE(5));
				gui->begin_group();
				{
					widget->selection(var->c_selection.selection_icon[2].data(), ImVec2(74, 76), 2, var->c_selection.selection);

					gui->sameline();

					widget->selection(var->c_selection.selection_icon[3].data(), ImVec2(74, 76), 3, var->c_selection.selection);

					gui->sameline();

					widget->selection(var->c_selection.selection_icon[4].data(), ImVec2(74, 76), 4, var->c_selection.selection);
				}
				gui->end_group();

				gui->set_cursor_pos(get_cursor_pos() + SCALE(39, -5));
				gui->begin_group();
				{
					widget->selection(var->c_selection.selection_icon[5].data(), ImVec2(74, 76), 5, var->c_selection.selection);

					gui->sameline();

					widget->selection(var->c_selection.selection_icon[6].data(), ImVec2(74, 76), 6, var->c_selection.selection);
				}
				gui->end_group();

			}
			gui->end();
		}
		gui->pop_style_var(2);

		gui->water_mark("watermark", var->c_watermark.watermark_content, static_cast<watermark_position>(var->c_watermark.watermark_position), &var->c_watermark.watermark);

	}
	gui->end_frame();

}