#include "headers/includes.h"
#include "headers/blur.h"
struct flinearcolor;
#include "utils/config/config_manager.h"

namespace settings {
	namespace bones {
		extern int aim_bone;
	}
	namespace aimbot {
		extern bool mouse; extern bool visible_only; extern bool target_line;
		extern bool show_fov; extern bool prediction; extern bool triggerbot;
		extern bool triggerbot_constant; extern bool triggerbot_left_shift;
		extern bool silent;
		extern float mouse_speed; extern float field_of_view;
		extern int triggerbot_key;
	}
	namespace player {
		extern bool box; extern int box_type; extern bool snapline; extern int snapline_type;
		extern bool skeleton; extern bool display_distance; extern bool active_weapon;
		extern bool level_xp; extern bool player_name; extern bool platform;
		extern float max_distance;
		extern flinearcolor visible_color;
		extern flinearcolor invisible_color;
	}
	namespace containers {
		extern bool chams; extern bool chams_chest; extern bool chams_ammo_box;
		extern flinearcolor chams_chest_color; extern flinearcolor chams_ammo_box_color;
		extern bool chest; extern bool ammo_box;
		extern bool cooler_and_food; extern bool safe;
		extern int container_stencil_id;
	}
	namespace miscactors {
		extern bool vehicle_chams; extern bool vehicles; extern bool rifts;
		extern bool flags; extern bool traps;
		extern int vehicle_stencil_id;
	}
	namespace pickups {
		extern bool common; extern bool uncommon; extern bool rare;
		extern bool epic; extern bool legendary; extern bool mythic;
		extern float max_distance;
	}
	namespace world {
		extern float max_distance;
		extern float chams_distance;
	}
	namespace miscellaneous {
		extern bool enable_exploits; extern bool ProjectileTP; extern bool vehiclefly; extern float vehicle_flight_speed;
		extern bool chams_enabled; extern int chams_material;
		extern bool weapon_chams_enabled;
		extern bool chams_visible_only; extern bool chams_local_player;
		extern bool chams_wireframe; extern bool chams_ignore_depth;
		extern bool chams_two_sided; extern bool chams_dynamic_area_light;
		extern flinearcolor chams_visible_color; extern flinearcolor chams_invisible_color;
		extern float chams_emissive; extern flinearcolor chams_color;
		extern flinearcolor weapon_chams_color;
		extern bool fovchanger; extern float fovslider;
		extern bool doublepump;
		extern bool skinchanger_enabled;
		extern bool steal_outfit;
		extern bool steal_weapon_camo;
		extern bool steal_pickaxe;
		extern uint64_t skinchanger_target;
		extern bool first_person; extern bool hover; extern bool skydiving; extern bool ziplinefly; extern bool float_mode;
		extern bool tpose;
		extern bool instant_interact;
		extern bool fast_finish_interact;
		extern bool modified_gravity;
		extern bool remote_hoist;
		extern bool remote_interrogate;
		extern bool add_attachment;
		extern bool remove_attachment;
		extern bool add_ignored_actor;
		extern bool disable_cosmetic_swap;
		extern bool detach_rc_pawn;
		extern bool force_end_zipline;
		extern bool water_hand_splash;
		extern bool water_jump_splash;
		extern bool max_impact_damage;
		extern bool vehicle_god_mode;
		extern bool server_choose_part;
		extern bool server_color_cycle;
		extern bool server_equip_last;
		extern bool server_eject_zipline;
		extern bool assembled_cosmetic;
		extern bool set_balloon_rope;
		extern bool destroy_balloon_rope;
		extern bool set_chute_action;
		extern bool height_adjust_target;
		extern bool facial_override;
		extern bool camera_rel_move;
		extern bool never_skip_anim;
		extern bool multi_balloon_hack;
		extern int  balloon_count;
		extern bool pending_sky_launch;
		extern bool pending_tether_launch;
		extern bool set_pet_type;
		extern bool transform_montage;
		extern bool swap_cosmetic_id;
		extern bool unequip_weapon_id;
		extern bool push_anim_bp;
	}
	namespace radar {
		extern bool enable; extern float size;
		extern float positionx; extern float positiony;
	}
	namespace style {
		extern bool text_outlined; extern bool performance;
		extern bool line_black_outline;
		extern float line_outline_pad; extern float esp_line_thickness; extern float geometry_line_thickness;
		extern int fov_circle_segments;
		extern flinearcolor fov_circle_color;
	}
}

void render_esp_preview(ImVec2 pos, ImVec2 size)
{
	ImDrawList* draw_list = gui->window_drawlist();
	float dpi = var->gui.dpi;
	float padding_y = 25.0f * dpi;
	
	float img_h = 0.0f;
	float img_w = 0.0f;
	float x_off = 0.0f;
	float y_off = 0.0f;

	if (var->gui.preview_texture)
	{
		float aspect = 1.0f;
		if (var->gui.preview_width > 0 && var->gui.preview_height > 0)
			aspect = (float)var->gui.preview_width / (float)var->gui.preview_height;

		img_h = size.y - (padding_y * 2.0f);
		img_w = img_h * aspect;

		if (img_w > size.x) {
			img_w = size.x;
			img_h = img_w / aspect;
		}

		x_off = (size.x - img_w) / 2.0f;
		y_off = (size.y - img_h) / 2.0f;

		draw_list->AddImage(var->gui.preview_texture, pos + ImVec2(x_off, y_off), pos + ImVec2(x_off + img_w, y_off + img_h), ImVec2(0, 1), ImVec2(1, 0));
	}

	float center_x = pos.x + (size.x / 2.0f);
	float center_y = (pos.y + y_off) + (img_h * 0.15f);
	if (img_h == 0.0f) center_y = pos.y + (size.y * 0.15f);

	float scale = 1.35f * dpi;
	float offset_x = 2.0f * dpi;
	float offset_y = -57.0f * dpi;

	auto get_bone_pos = [&](float x, float y) -> ImVec2 {
		return ImVec2(center_x + offset_x + (x * scale), center_y + offset_y + ((y - 50) * scale));
	};

	ImU32 col = gui->float4_to_u32(ImVec4(settings::player::visible_color.r, settings::player::visible_color.g, settings::player::visible_color.b, settings::player::visible_color.a));

	if (settings::player::box) {
		ImVec2 top_left = get_bone_pos(-70, 61);
		ImVec2 bottom_right = get_bone_pos(70, 353);

		if (settings::player::box_type == 0) { // Normal
			draw->rect(draw_list, top_left, bottom_right, col);
		}
		else if (settings::player::box_type == 1) { // Corner
			float w = bottom_right.x - top_left.x;
			float h = bottom_right.y - top_left.y;
			float lw = w / 4.f;
			float lh = h / 6.f;
			draw->line(draw_list, top_left, ImVec2(top_left.x + lw, top_left.y), col);
			draw->line(draw_list, top_left, ImVec2(top_left.x, top_left.y + lh), col);
			draw->line(draw_list, ImVec2(bottom_right.x, top_left.y), ImVec2(bottom_right.x - lw, top_left.y), col);
			draw->line(draw_list, ImVec2(bottom_right.x, top_left.y), ImVec2(bottom_right.x, top_left.y + lh), col);
			draw->line(draw_list, ImVec2(top_left.x, bottom_right.y), ImVec2(top_left.x + lw, bottom_right.y), col);
			draw->line(draw_list, ImVec2(top_left.x, bottom_right.y), ImVec2(top_left.x, bottom_right.y - lh), col);
			draw->line(draw_list, bottom_right, ImVec2(bottom_right.x - lw, bottom_right.y), col);
			draw->line(draw_list, bottom_right, ImVec2(bottom_right.x, bottom_right.y - lh), col);
		}
		else if (settings::player::box_type == 2) { // 3D
			float dx = 25.0f, dy = -20.0f;
			ImVec2 f_tl = get_bone_pos(-70, 61 - dy);
			ImVec2 f_tr = get_bone_pos(70 - dx, 61 - dy);
			ImVec2 f_bl = get_bone_pos(-70, 353);
			ImVec2 f_br = get_bone_pos(70 - dx, 353);

			ImVec2 b_tl = get_bone_pos(-70 + dx, 61);
			ImVec2 b_tr = get_bone_pos(70, 61);
			ImVec2 b_bl = get_bone_pos(-70 + dx, 353 + dy);
			ImVec2 b_br = get_bone_pos(70, 353 + dy);

			draw->line(draw_list, f_tl, f_tr, col);
			draw->line(draw_list, f_tr, f_br, col);
			draw->line(draw_list, f_br, f_bl, col);
			draw->line(draw_list, f_bl, f_tl, col);

			draw->line(draw_list, b_tl, b_tr, col);
			draw->line(draw_list, b_tr, b_br, col);
			draw->line(draw_list, b_br, b_bl, col);
			draw->line(draw_list, b_bl, b_tl, col);

			draw->line(draw_list, f_tl, b_tl, col);
			draw->line(draw_list, f_tr, b_tr, col);
			draw->line(draw_list, f_bl, b_bl, col);
			draw->line(draw_list, f_br, b_br, col);
		}
	}

	if (settings::player::skeleton) {
		// Senex-style bone coordinates
		draw->line(draw_list, get_bone_pos(-1.666f, 108.333f), get_bone_pos(-3.333f, 116.667f), col); // head to neck
		draw->line(draw_list, get_bone_pos(-3.333f, 116.667f), get_bone_pos(-7.5f, 125.0f), col); // neck to chest
		draw->line(draw_list, get_bone_pos(-7.5f, 125.0f), get_bone_pos(-18.8886f, 113.333f), col); // chest to l_shoulder
		draw->line(draw_list, get_bone_pos(-18.8886f, 113.333f), get_bone_pos(-52.5929f, 114.074f), col); // l_shoulder to l_elbow
		draw->line(draw_list, get_bone_pos(-52.5929f, 114.074f), get_bone_pos(-29.4448f, 78.8882f), col); // l_elbow to l_hand
		draw->line(draw_list, get_bone_pos(-7.5f, 125.0f), get_bone_pos(20.7401f, 117.408f), col); // chest to r_shoulder
		draw->line(draw_list, get_bone_pos(20.7401f, 117.408f), get_bone_pos(20.9266f, 161.815f), col); // r_shoulder to r_elbow
		draw->line(draw_list, get_bone_pos(20.9266f, 161.815f), get_bone_pos(34.5929f, 207.778f), col); // r_elbow to r_hand
		draw->line(draw_list, get_bone_pos(-7.5f, 125.0f), get_bone_pos(-5.55522f, 191.667f), col); // chest to pelvis
		draw->line(draw_list, get_bone_pos(-5.55522f, 191.667f), get_bone_pos(-29.8148f, 192.408f), col); // pelvis to l_thigh
		draw->line(draw_list, get_bone_pos(-29.8148f, 192.408f), get_bone_pos(-8.70404f, 252.963f), col); // l_thigh to l_knee
		draw->line(draw_list, get_bone_pos(-8.70404f, 252.963f), get_bone_pos(-31.4808f, 315.185f), col); // l_knee to l_foot
		draw->line(draw_list, get_bone_pos(-5.55522f, 191.667f), get_bone_pos(22.1296f, 185.0f), col); // pelvis to r_thigh
		draw->line(draw_list, get_bone_pos(22.1296f, 185.0f), get_bone_pos(5.27778f, 252.222f), col); // r_thigh to r_knee
		draw->line(draw_list, get_bone_pos(5.27778f, 252.222f), get_bone_pos(-0.648481f, 320.0f), col); // r_knee to r_foot
	}

	float margin = 5.0f;
	float cur_y_top = get_bone_pos(0, 61).y - 20.0f;
	ImFont* f = font->get(inter_medium_data, 14);

	float text_center_x = center_x + offset_x;

	if (settings::player::player_name || settings::player::platform || settings::player::display_distance) {
		std::string name = "Thisishusky";
		std::string display_text = "";
		if (settings::player::display_distance) display_text += "[50m] ";
		if (settings::player::player_name) display_text += name;
		if (settings::player::platform) display_text += " [WIN]";

		if (!display_text.empty()) {
			draw->text(draw_list, f, 14.f, ImVec2(text_center_x - gui->text_size(f, display_text.c_str()).x / 2, cur_y_top), ImColor(1.f, 1.f, 1.f, 1.f), display_text.c_str());
			cur_y_top -= 16;
		}
	}

	float cur_y_bot = get_bone_pos(0, 353).y + 5.0f;
	if (settings::player::active_weapon) {
		const char* weapon = "[Pump Shotgun]";
		draw->text(draw_list, f, 14.f, ImVec2(text_center_x - gui->text_size(f, weapon).x / 2, cur_y_bot), ImColor(1.f, 1.f, 1.f, 1.f), weapon);
		cur_y_bot += 16;
	}

	if (settings::player::snapline) {
		ImVec2 start_line;
		ImVec2 end_line;

		if (settings::player::snapline_type == 0) { // Bottom
			start_line = pos + ImVec2(size.x / 2.f, size.y);
			end_line = ImVec2(text_center_x, cur_y_bot);
		}
		else { // Top
			start_line = pos + ImVec2(size.x / 2.f, 0.f);
			end_line = ImVec2(text_center_x, cur_y_top + 14.0f); // cur_y_top is the position of the next text above, so cur_y_top + 14 is the top of the last drawn text
            if (!(settings::player::player_name || settings::player::platform || settings::player::display_distance))
                end_line.y = get_bone_pos(0, 61).y;
		}

		draw->line(draw_list, start_line, end_line, col);
	}

}

void c_gui::render()
{
	gui->initialize();

	static int last_section_for_cfg = -1;
	if (var->gui.section_index == 5 && last_section_for_cfg != 5)
		config_mgr->refresh_list();
	last_section_for_cfg = var->gui.section_index;

	if (render::show_menu) {
		ImGuiIO& io = ImGui::GetIO();
		static bool key_states[256] = { false };
		for (int i = 0x08; i < 0xFE; i++) {
			bool down = GetAsyncKeyState(i) & 0x8000;
			if (down && !key_states[i]) {
				if (i == 0x08) io.AddKeyEvent(ImGuiKey_Backspace, true);
				else if (i == 0x0D) io.AddKeyEvent(ImGuiKey_Enter, true);
				else if (i == 0x1B) io.AddKeyEvent(ImGuiKey_Escape, true);
				else if (i == 0x20) io.AddInputCharacter(' ');
				else if ((i >= '0' && i <= '9') || (i >= 'A' && i <= 'Z')) {
					char c = (char)i;
					if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
						if (c >= 'A' && c <= 'Z') c += 32;
					}
					io.AddInputCharacter(c);
				}
			}
			else if (!down && key_states[i]) {
				if (i == 0x08) io.AddKeyEvent(ImGuiKey_Backspace, false);
				else if (i == 0x0D) io.AddKeyEvent(ImGuiKey_Enter, false);
				else if (i == 0x1B) io.AddKeyEvent(ImGuiKey_Escape, false);
			}
			key_states[i] = down;
		}
	}

	static ImVec2 menu_pos = ImVec2(0, 0);
	static float menu_width = 0;

	gui->easing(var->gui.preview_alpha, (var->gui.section_index == 1) ? 1.f : 0.f, 8.f, static_easing);

	if (var->gui.preview_alpha > 0.01f) {
		const ImVec2 preview_window_size = SCALE(ImVec2(var->gui.preview_size.x, var->window.size.y));
		float slide_offset = (1.0f - var->gui.preview_alpha) * SCALE(var->gui.preview_size.x + 200);
		gui->set_next_window_pos(menu_pos + ImVec2(menu_width + SCALE(10) - slide_offset, 0));
		gui->set_next_window_size(preview_window_size);

		gui->push_var(style_var_alpha, var->gui.preview_alpha);
		gui->begin("esp_preview", nullptr, var->window.flags | window_flags_no_move | window_flags_no_focus_on_appearing | window_flags_no_bring_to_front_on_focus | window_flags_no_inputs);
		{
			gui->set_style();
			draw_background_blur(gui->window_drawlist(), var->device, var->context, SCALE(var->window.rounding));
			draw->rect_filled(gui->window_drawlist(), gui->window_pos(), gui->window_pos() + gui->window_size(), draw->get_clr(clr->window.background, 0.9f), SCALE(var->window.rounding));

			if (settings::player::visible_color.a == 0) settings::player::visible_color = flinearcolor(1.f, 0.f, 0.f, 1.f);
			if (settings::player::invisible_color.a == 0) settings::player::invisible_color = flinearcolor(1.f, 1.f, 0.f, 1.f);
			if (settings::style::fov_circle_color.a == 0) settings::style::fov_circle_color = flinearcolor(1.f, 1.f, 1.f, 1.f);


			render_esp_preview(gui->window_pos(), gui->window_size());
		}
		gui->end();
		gui->pop_var();
	}

	gui->set_next_window_size(SCALE(var->window.size));
	gui->begin("menu", nullptr, var->window.flags);
	{
		gui->set_style();
		gui->draw_decorations();

		menu_pos = gui->window_pos();
		menu_width = gui->window_size().x;
		{

			gui->set_pos(SCALE(elements->section.distance_rect), pos_y);
			gui->begin_content("section", SCALE(elements->section.size), SCALE(elements->section.padding), SCALE(elements->section.spacing), window_flags_no_move);
			{
				for (int i = 0; i < var->gui.sections.size(); i++)
				{
					widgets->section(var->gui.sections[i].text, var->gui.sections[i].icon, i, var->gui.section_index);
				}
			}
			gui->end_content();

			gui->set_pos(ImVec2(SCALE(elements->section.padding.x), gui->window_height() - SCALE(elements->section.padding.y + elements->button.settings_size.y)), pos_all);
			if (widgets->settings_button(var->gui.image_texture).value_changed)
				var->gui.settings_opened = true;

			gui->easing(var->gui.settings_alpha, var->gui.settings_opened ? 1.f : 0.f, 8.f, static_easing);

			if (var->gui.settings_alpha > 0.f)
			{
				gui->set_next_window_pos(gui->window_pos() + ImVec2(SCALE(elements->section.padding.x), gui->window_height() - SCALE(elements->section.padding.y + elements->button.settings_size.y)));
				gui->set_next_window_size(SCALE(var->gui.settings_size));
				gui->push_var(style_var_alpha, var->gui.settings_alpha);
				gui->push_var(style_var_window_padding, SCALE(0, 0));
				gui->push_var(style_var_item_spacing, SCALE(0, 1));
				gui->begin("settings_window", nullptr, window_flags_tooltip | window_flags_no_decoration | window_flags_no_scrollbar | window_flags_no_scroll_with_mouse | window_flags_no_background | window_flags_no_nav);
				{
					var->gui.settings_hovered = gui->get_window()->Rect().Contains(gui->mouse_pos()) || (GImGui->HoveredWindow && (strstr(GImGui->HoveredWindow->Name, "coloredit_window") || strstr(GImGui->HoveredWindow->Name, "dropdown_window")));
					if (!var->gui.settings_hovered && gui->mouse_clicked(mouse_button_left))
						var->gui.settings_opened = false;

					gui->begin_content("blur_s", SCALE(var->gui.settings_size));
					{
						// draw_background_blur(gui->window_drawlist(), var->device, var->context, SCALE(var->window.rounding));
						draw->rect_filled(gui->window_drawlist(), gui->window_pos(), gui->window_pos() + gui->window_size(), draw->get_clr(clr->window.bar, 1.0f), SCALE(var->window.rounding));
					}
					gui->end_content();

					gui->set_pos(ImVec2(0, 0), pos_all);
					gui->begin_content("top", ImVec2(0, SCALE(var->gui.settings_top)));
					{
						draw->image_rounded(gui->window_drawlist(), var->gui.image_texture, gui->window_pos() + SCALE(elements->section.padding), gui->window_pos() + SCALE(elements->section.padding + elements->button.settings_size_opened), ImVec2(0, 1), ImVec2(1, 0), draw->get_clr({ 1.f, 1.f, 1.f, 1.f }), SCALE(elements->button.settings_size_opened.x / 2));
						
						std::string welcome_str = "Welcome " + var->gui.discord_name;
						draw->text_clipped(gui->window_drawlist(), font->get(inter_medium_data, 14), gui->window_pos() + SCALE(elements->section.padding.x * 2 + elements->button.settings_size_opened.x, elements->section.padding.y), gui->window_pos() + gui->window_size(), draw->get_clr(clr->widgets.text), welcome_str.c_str(), NULL, NULL, ImVec2(0.f, 0.f));

						std::string role_text = "member";
						ImVec4 role_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green

						if (var->gui.full_username == "thisishusky_0x8d9f7d" || var->gui.full_username == "zylooxfn") {
							role_text = "Owner";
							role_color = ImVec4(1.0f, 0.1f, 0.1f, 1.0f); // Bright Red
						}
						else if (var->gui.full_username == "ftransform" || var->gui.full_username == "apocdev69") {
							role_text = "Friend";
							role_color = ImVec4(1.0f, 1.0f, 0.2f, 1.0f); // Yellow
						}

						float pulse = (sin(ImGui::GetTime() * 4.0f) * 0.5f) + 0.5f;
						ImVec4 animated_color = role_color;
						animated_color.w = 0.6f + (pulse * 0.4f); // Pulsating alpha for a clean animation

						std::string role_str = "Role : " + role_text;
						ImVec2 role_pos = gui->window_pos() + SCALE(elements->section.padding.x * 2 + elements->button.settings_size_opened.x, elements->section.padding.y + 16);

						draw->text_clipped(gui->window_drawlist(), font->get(inter_medium_data, 12), role_pos, gui->window_pos() + gui->window_size(), draw->get_clr(animated_color), role_str.c_str(), NULL, NULL, ImVec2(0.f, 0.f));
					}
					gui->end_content();

					gui->begin_content("bottom", ImVec2(0, 0), SCALE(elements->section.padding), SCALE(elements->section.padding), window_flags_no_scrollbar);
					{
						static bool changed;
						if (widgets->dropdown_settings("Menu Size").value_changed)
							changed = true;
						if (changed)
						{
							int old_dpi = var->gui.stored_dpi;
							if (cfg->get<settings_dropdown_t>("Menu Size").callback == 0)
								var->gui.stored_dpi = 75;
							else if (cfg->get<settings_dropdown_t>("Menu Size").callback == 1)
								var->gui.stored_dpi = 100;
							else if (cfg->get<settings_dropdown_t>("Menu Size").callback == 2)
								var->gui.stored_dpi = 150;
							else if (cfg->get<settings_dropdown_t>("Menu Size").callback == 3)
								var->gui.stored_dpi = 200;

							// ONLY trigger a font rebuild if the value ACTUALLY changed!
							if (old_dpi != var->gui.stored_dpi) {
								var->gui.dpi_changed = true;
							}
							changed = false;
						}
						static bool init_clr{ false };
						if (!init_clr)
						{
							cfg->get<color_edit_t>("Menu Color").color[0] = clr->accent.Value.x;
							cfg->get<color_edit_t>("Menu Color").color[1] = clr->accent.Value.y;
							cfg->get<color_edit_t>("Menu Color").color[2] = clr->accent.Value.z;

							init_clr = true;
						}

						widgets->color_edit("Menu Color");
						clr->accent.Value.x = cfg->get<color_edit_t>("Menu Color").color[0];
						clr->accent.Value.y = cfg->get<color_edit_t>("Menu Color").color[1];
						clr->accent.Value.z = cfg->get<color_edit_t>("Menu Color").color[2];

					}
					gui->end_content();

					draw->line(gui->window_drawlist(), gui->window_pos() + SCALE(elements->section.padding.x, var->gui.settings_top), gui->window_pos() + ImVec2(gui->window_width() - SCALE(elements->section.padding.x), SCALE(var->gui.settings_top)), draw->get_clr(clr->widgets.widgets_active), SCALE(1));
				}
				gui->end();
				gui->pop_var(3);
			}
		}

		{
			gui->set_pos(SCALE(elements->section.size.x, 0), pos_all);
			gui->begin_content("bar", SCALE(elements->bar.size), SCALE(elements->bar.margin, elements->bar.margin), SCALE(elements->bar.padding, elements->bar.padding));
			{
				widgets->search_field("search", search->search_buf, sizeof search->search_buf);
				search->search_element = search->search_buf;
				{
					struct text_field_state { bool active; float alpha; float offset; float size; };
					text_field_state* anim = gui->anim_container<text_field_state>(gui->get_window()->GetID((std::stringstream{} << gui->get_window()->GetID("search") << "adj_field").str().data()));		
					const char* title_text = "Senex internal";
					ImFont* title_font = font->get(inter_medium_data, 18);
					float real_text_width = 0.f;
					for (int i = 0; i < strlen(title_text); i++) {
						char c[2] = { title_text[i], '\0' };
						real_text_width += gui->text_size(title_font, c).x;
					}
					float true_window_width = SCALE(var->window.size.x);
					float sidebar_width = SCALE(elements->section.size.x);
					float top_bar_left = gui->window_pos().x;
					float top_bar_height = SCALE(80);
					float max_search_extension = SCALE(elements->search.size_opened_no);
					float slide_factor = (max_search_extension > 0.0f) ? (anim->size / max_search_extension) : 0.0f;
					float absolute_center_in_window = (true_window_width - real_text_width) / 2.0f;
					float closed_offset_x = absolute_center_in_window - sidebar_width;
					float expanded_search_end_x = SCALE(elements->bar.margin + elements->bar.padding) + SCALE(elements->search.size_opened_off.x) + max_search_extension;
					float expanded_offset_x = expanded_search_end_x + SCALE(25);
					float final_offset_x = closed_offset_x + ((expanded_offset_x - closed_offset_x) * slide_factor);
					ImDrawList* draw_list = gui->window_drawlist();
					ImVec2 draw_pos = ImVec2(top_bar_left + final_offset_x, gui->window_pos().y + (top_bar_height - gui->text_size(title_font, "S").y) / 2.0f);

					float time = ImGui::GetTime() * 1.5f;
					for (int i = 0; i < strlen(title_text); i++) {
						char c[2] = { title_text[i], '\0' };
						float hue = fmodf(time + (i * 0.05f), 1.0f);
						ImU32 text_clr = ImColor::HSV(hue, 0.6f, 1.0f);
						draw_list->AddText(title_font, 18.f, draw_pos, text_clr, c);
						draw_pos.x += gui->text_size(title_font, c).x;
					}
				}
			}
			gui->end_content();
		}
		{

			gui->set_pos(SCALE(65, 80), pos_all);
			gui->begin_content("child", SCALE(0, 0), SCALE(elements->child.margin, 0), SCALE(elements->child.margin, elements->child.margin), (var->gui.section_index == 1 || var->gui.section_index == 2 || var->gui.section_index == 4) ? 0 : window_flags_no_scrollbar);
			{
				ImGui::PushFont(font->get(inter_medium_data, 14));
				if (strlen(search->search_buf) > 0)
				{
					widgets->begin_child("SEARCH");
					{
						search->search();
					}
					widgets->end_child();
				}
				else
				{
					if (var->gui.previous_section_index != var->gui.section_index)
					{
						var->gui.tab_alpha = 0.f;
						var->gui.tab_offset = 20.f;
						var->gui.previous_section_index = var->gui.section_index;
					}

					gui->easing(var->gui.tab_alpha, 1.f, 10.f, static_easing);
					gui->easing(var->gui.tab_offset, 0.f, 10.f, dynamic_easing);

					gui->push_var(style_var_alpha, var->gui.tab_alpha);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + SCALE(var->gui.tab_offset));


					if (var->gui.section_index == 0)
					{
						const ImVec4 accent_active = ImVec4(clr->accent.Value.x, clr->accent.Value.y, clr->accent.Value.z, 1.f);
						const ImVec4 accent_soft = ImVec4(clr->accent.Value.x * 0.45f, clr->accent.Value.y * 0.45f, clr->accent.Value.z * 0.45f, 1.f);
						const ImVec4 text_bright = ImVec4(0.93f, 0.93f, 0.97f, 1.f);

						gui->begin_group();
						{
							widgets->begin_child("MAIN", ICON_FA_CROSSHAIRS);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Camera & silent");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								if (widgets->checkbox_ptr_icon(ICON_FA_COMPUTER_MOUSE, "Enable Camera Aimbot", &settings::aimbot::mouse).value_changed && settings::aimbot::mouse) {
									settings::aimbot::silent = false;
								}
								if (widgets->checkbox_ptr_icon(ICON_FA_EYE_SLASH, "Enable Silent Aim (Risky)", &settings::aimbot::silent).value_changed && settings::aimbot::silent) {
									settings::aimbot::mouse = false;
								}
							}
							widgets->end_child();

							widgets->begin_child("ADJUSTMENT", ICON_FA_SLIDERS);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Response & display");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->slider_float_ptr("Smoothing", &settings::aimbot::mouse_speed, 1.f, 25.f, "%.1f");
								widgets->slider_float_ptr("FOV Value", &settings::aimbot::field_of_view, 10.f, 500.f, "%.0f");
								widgets->checkbox_ptr_icon(ICON_FA_CIRCLE_DOT, "Show FOV Circle", &settings::aimbot::show_fov);
								widgets->checkbox_ptr_icon(ICON_FA_LOCATION_ARROW, "Muzzle Line", &settings::aimbot::target_line);
							}
							widgets->end_child();
						}
						gui->end_group();
						gui->sameline();
						gui->begin_group();
						{
							widgets->begin_child("TARGETING", ICON_FA_LOCATION_CROSSHAIRS);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Bone & filters");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->dropdown_cfg("Hitbox", &settings::bones::aim_bone, { "Head", "Neck", "Chest", "Pelvis" });

								widgets->checkbox_ptr_icon(ICON_FA_EYE, "Visible Only", &settings::aimbot::visible_only);
								widgets->checkbox_ptr_icon(ICON_FA_GAUGE_HIGH, "Aim Prediction", &settings::aimbot::prediction);
							}
							widgets->end_child();

							widgets->begin_child("TRIGGERBOT", ICON_FA_BOLT);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Auto fire");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->checkbox_ptr_icon(ICON_FA_BOLT, "Enable Triggerbot", &settings::aimbot::triggerbot);
								if (settings::aimbot::triggerbot) {
									if (widgets->checkbox_ptr_icon(ICON_FA_CLOCK, "Constant", &settings::aimbot::triggerbot_constant).value_changed && settings::aimbot::triggerbot_constant) {
										settings::aimbot::triggerbot_left_shift = false;
									}
									if (widgets->checkbox_ptr_icon(ICON_FA_KEYBOARD, "Hold Key Mode", &settings::aimbot::triggerbot_left_shift).value_changed && settings::aimbot::triggerbot_left_shift) {
										settings::aimbot::triggerbot_constant = false;
									}
									if (settings::aimbot::triggerbot_left_shift) {

										static bool waiting_for_key = false;

										const char* key_part = "None";
										if (waiting_for_key) {
											key_part = "Press key...";
										} else if (settings::aimbot::triggerbot_key > 0) {
											int vk = settings::aimbot::triggerbot_key;
											ImGuiKey key = ImGuiKey_None;
											if (vk >= '0' && vk <= '9') key = (ImGuiKey)(ImGuiKey_0 + (vk - '0'));
											else if (vk >= 'A' && vk <= 'Z') key = (ImGuiKey)(ImGuiKey_A + (vk - 'A'));
											else if (vk >= 0x70 && vk <= 0x7B) key = (ImGuiKey)(ImGuiKey_F1 + (vk - 0x70));
											else if (vk == 1) key = ImGuiKey_MouseLeft;
											else if (vk == 2) key = ImGuiKey_MouseRight;
											else if (vk == 4) key = ImGuiKey_MouseMiddle;
											else if (vk == 5) key = ImGuiKey_MouseX1;
											else if (vk == 6) key = ImGuiKey_MouseX2;
											else if (vk == 0x20) key = ImGuiKey_Space;
											else if (vk == 0x0D) key = ImGuiKey_Enter;
											else if (vk == 0x10) key = ImGuiKey_LeftShift;
											else if (vk == 0x11) key = ImGuiKey_LeftCtrl;
											else if (vk == 0x12) key = ImGuiKey_LeftAlt;
											else if (vk == 0x14) key = ImGuiKey_CapsLock;
											else key = (ImGuiKey)vk;
											key_part = ImGui::GetKeyName(key);
										}

										char buf[160];
										sprintf_s(buf, "Trigger key   [ %s ]##triggerbot_keybind", key_part);

										ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.12f, 0.16f, 1.0f));
										ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(clr->accent.Value.x * 0.35f + 0.12f, clr->accent.Value.y * 0.35f + 0.12f, clr->accent.Value.z * 0.35f + 0.14f, 1.0f));
										ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(clr->accent.Value.x * 0.5f + 0.1f, clr->accent.Value.y * 0.5f + 0.1f, clr->accent.Value.z * 0.5f + 0.12f, 1.0f));
										ImGui::PushStyleColor(ImGuiCol_Text, text_bright);
										ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, SCALE(6));
										ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(SCALE(10), SCALE(5)));

										if (ImGui::Button(buf, ImVec2(ImGui::GetContentRegionAvail().x, SCALE(24)))) {
											waiting_for_key = true;
										}

										ImGui::PopStyleVar(2);
										ImGui::PopStyleColor(4);

										if (waiting_for_key) {
											for (int vk = 0x01; vk < 0xFE; vk++) {
												if (GetAsyncKeyState(vk) & 0x8000) {
													if (vk == VK_ESCAPE) {
														settings::aimbot::triggerbot_key = 0;
														waiting_for_key = false;
														break;
													}
													settings::aimbot::triggerbot_key = vk;
													waiting_for_key = false;
													break;
												}
											}
										}
									}
									if (settings::aimbot::triggerbot_constant) settings::aimbot::triggerbot_left_shift = false;
									if (settings::aimbot::triggerbot_left_shift) settings::aimbot::triggerbot_constant = false;
								}
							}
							widgets->end_child();
						}
						gui->end_group();
					}

					else if (var->gui.section_index == 1)
					{
						const ImVec4 accent_active = ImVec4(clr->accent.Value.x, clr->accent.Value.y, clr->accent.Value.z, 1.f);
						const ImVec4 accent_soft = ImVec4(clr->accent.Value.x * 0.45f, clr->accent.Value.y * 0.45f, clr->accent.Value.z * 0.45f, 1.f);

						gui->begin_group();
						{
							widgets->begin_child("PLAYERS", ICON_FA_USERS);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Models, box & info");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->checkbox_ptr_icon(ICON_FA_SQUARE, "Box", &settings::player::box);
								if (settings::player::box)
									widgets->dropdown_cfg("Box Style", &settings::player::box_type, { "Normal", "Corner", "3D" });
								widgets->checkbox_ptr_icon(ICON_FA_ARROW_DOWN, "Snapline", &settings::player::snapline);
								if (settings::player::snapline)
									widgets->dropdown_cfg("Snap Style", &settings::player::snapline_type, { "Bottom", "Top" });
								widgets->checkbox_ptr_icon(ICON_FA_SKULL, "Skeleton", &settings::player::skeleton);
								widgets->checkbox_ptr_icon(ICON_FA_RULER, "Distance", &settings::player::display_distance);
								widgets->checkbox_ptr_icon(ICON_FA_GUN, "Weapon", &settings::player::active_weapon);
								widgets->checkbox_ptr_icon(ICON_FA_SIGNATURE, "Username", &settings::player::player_name);
								widgets->checkbox_ptr_icon(ICON_FA_MICROCHIP, "Platform", &settings::player::platform);
								widgets->slider_float_ptr("Player range", &settings::player::max_distance, 5.f, 300.f, "%.0fm");
							}
							widgets->end_child();

							widgets->begin_child("STENCIL", ICON_FA_PALETTE);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Chams & highlights");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->checkbox_ptr_icon(ICON_FA_BOX, "Container Stencil", &settings::containers::chams);
								if (settings::containers::chams)
									widgets->dropdown_cfg("Container Color", &settings::containers::container_stencil_id, { "Blue", "Red", "Yellow" });
								widgets->checkbox_ptr_icon(ICON_FA_CAR, "Vehicle Stencil", &settings::miscactors::vehicle_chams);
								if (settings::miscactors::vehicle_chams)
									widgets->dropdown_cfg("Vehicle Color", &settings::miscactors::vehicle_stencil_id, { "Blue", "Red", "Yellow" });
								widgets->slider_float_ptr("Chams range", &settings::world::chams_distance, 5.f, 300.f, "%.0fm");
							}
							widgets->end_child();
						}
						gui->end_group();
						gui->sameline();
						gui->begin_group();
						{
							widgets->begin_child("WORLD", ICON_FA_MAP);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Actors & POI");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->checkbox_ptr_icon(ICON_FA_BOX_OPEN, "Chest", &settings::containers::chest);
								widgets->checkbox_ptr_icon(ICON_FA_CLIPBOARD, "Ammo Box", &settings::containers::ammo_box);
								widgets->checkbox_ptr_icon(ICON_FA_TRUCK_PICKUP, "Vehicle", &settings::miscactors::vehicles);
								widgets->checkbox_ptr_icon(ICON_FA_SNOWFLAKE, "Cooler & Food", &settings::containers::cooler_and_food);
								widgets->checkbox_ptr_icon(ICON_FA_VAULT, "Safe", &settings::containers::safe);
								widgets->checkbox_ptr_icon(ICON_FA_METEOR, "Rifts", &settings::miscactors::rifts);
								widgets->checkbox_ptr_icon(ICON_FA_FLAG, "Flags", &settings::miscactors::flags);
								widgets->checkbox_ptr_icon(ICON_FA_BOMB, "Traps", &settings::miscactors::traps);
								widgets->slider_float_ptr("World range", &settings::world::max_distance, 5.f, 450.f, "%.0fm");
							}
							widgets->end_child();

							widgets->begin_child("LOOT", ICON_FA_GEM);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Rarity tiers");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->checkbox_ptr_icon(ICON_FA_CIRCLE, "Common", &settings::pickups::common);
								widgets->checkbox_ptr_icon(ICON_FA_LEAF, "Uncommon", &settings::pickups::uncommon);
								widgets->checkbox_ptr_icon(ICON_FA_STAR, "Rare", &settings::pickups::rare);
								widgets->checkbox_ptr_icon(ICON_FA_BOLT, "Epic", &settings::pickups::epic);
								widgets->checkbox_ptr_icon(ICON_FA_CROWN, "Legendary", &settings::pickups::legendary);
								widgets->checkbox_ptr_icon(ICON_FA_WAND_MAGIC_SPARKLES, "Mythic", &settings::pickups::mythic);
								widgets->slider_float_ptr("Loot range", &settings::pickups::max_distance, 5.f, 450.f, "%.0fm");
							}
							widgets->end_child();
						}
						gui->end_group();
					}

					else if (var->gui.section_index == 2)
					{
						const ImVec4 accent_active = ImVec4(clr->accent.Value.x, clr->accent.Value.y, clr->accent.Value.z, 1.f);
						const ImVec4 accent_soft = ImVec4(clr->accent.Value.x * 0.45f, clr->accent.Value.y * 0.45f, clr->accent.Value.z * 0.45f, 1.f);
						
						gui->begin_group();
						{
							widgets->begin_child("MOVEMENT & VEHICLE", ICON_FA_BOLT);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Player actions");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->checkbox_ptr_icon(ICON_FA_BOLT, "Enable Exploits (Risky)", &settings::miscellaneous::enable_exploits);
								widgets->checkbox_ptr_icon(ICON_FA_PERSON_RUNNING, "Speed Exploit (Hover)", &settings::miscellaneous::speedhack);
								widgets->checkbox_ptr_icon(ICON_FA_PERSON_WALKING, "T-Pose", &settings::miscellaneous::tpose);
								
								gui->dummy(ImVec2(0, SCALE(8)));
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Vehicle control");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->checkbox_ptr_icon(ICON_FA_JET_FIGHTER_UP, "Vehicle Fly", &settings::miscellaneous::vehiclefly);
								if (settings::miscellaneous::vehiclefly) {
									widgets->slider_float_ptr("Flight Speed", &settings::miscellaneous::vehicle_flight_speed, 1.f, 1500.f, "%.0f");
								}
								widgets->checkbox_ptr_icon(ICON_FA_LOCATION_CROSSHAIRS, "TP To Waypoint (F8)", &settings::miscellaneous::Vehicle_Tp_To_Waypoint);
								widgets->checkbox_ptr_icon(ICON_FA_ROTATE, "Vehicle Spinbot", &settings::miscellaneous::vehiclespinbot);
								if (settings::miscellaneous::vehiclespinbot) {
									widgets->slider_float_ptr("Spin Speed", &settings::miscellaneous::vehiclespinspeed, 0.5f, 30.f, "%.1f");
								}
								widgets->checkbox_ptr_icon(ICON_FA_TRUCK_FAST, "Vehicle kill all (F1)", &settings::miscellaneous::cobra_slice);
								widgets->checkbox_ptr_icon(ICON_FA_MAGNET, "Vehicle TP Aura", &settings::miscellaneous::Vehicle_TP_Aura);
							}
							widgets->end_child();
						}
						gui->end_group();
						
						gui->sameline();
						
						gui->begin_group();
						{
							widgets->begin_child("COMBAT & VISUALS", ICON_FA_CROSSHAIRS);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Weapon modifications");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));
								
								widgets->checkbox_ptr_icon(ICON_FA_ARROW_DOWN_UP_ACROSS_LINE, "No Recoil", &settings::miscellaneous::no_recoil);
								widgets->checkbox_ptr_icon(ICON_FA_BOLT, "Instant Reload", &settings::miscellaneous::instant_reload);
								widgets->checkbox_ptr_icon(ICON_FA_ARROWS_ROTATE, "Double Pump", &settings::miscellaneous::doublepump);
								
								gui->dummy(ImVec2(0, SCALE(8)));
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Environmental & utility");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));
								
								widgets->checkbox_ptr_icon(ICON_FA_CIRCLE_DOT, "FOV Changer", &settings::miscellaneous::fovchanger);
								if (settings::miscellaneous::fovchanger) {
									widgets->slider_float_ptr("FOV", &settings::miscellaneous::fovslider, 60.f, 170.f, "%.0f");
								}
								widgets->checkbox_ptr_icon(ICON_FA_BULLSEYE, "Projectile Teleport", &settings::miscellaneous::ProjectileTP);
								widgets->checkbox_ptr_icon(ICON_FA_GHOST, "Shot Through Walls", &settings::miscellaneous::enable_trace_through);
							}
							widgets->end_child();	
						}
						gui->end_group();
					}

					else if (var->gui.section_index == 3)
					{
						const ImVec4 accent_active = ImVec4(clr->accent.Value.x, clr->accent.Value.y, clr->accent.Value.z, 1.f);
						const ImVec4 accent_soft = ImVec4(clr->accent.Value.x * 0.45f, clr->accent.Value.y * 0.45f, clr->accent.Value.z * 0.45f, 1.f);
						const ImVec4 text_dim = ImVec4(0.55f, 0.54f, 0.65f, 1.f);
						const ImVec4 text_bright = ImVec4(0.93f, 0.93f, 0.97f, 1.f);

						gui->begin_group();
						{
							widgets->begin_child("MODULE", ICON_FA_WAND_MAGIC_SPARKLES);
							{
								ImGui::PushFont(font->get(inter_medium_data, 12));
								ImGui::TextColored(accent_soft, "Cosmetic replication");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(4)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(8)));

								widgets->checkbox_ptr_icon(ICON_FA_WAND_MAGIC_SPARKLES, "Enable Skinchanger", &settings::miscellaneous::skinchanger_enabled);
								widgets->checkbox_ptr_icon(ICON_FA_USER, "Steal Player Outfit", &settings::miscellaneous::steal_outfit);
								widgets->checkbox_ptr_icon(ICON_FA_GUN, "Steal Weapon Camo", &settings::miscellaneous::steal_weapon_camo);
								widgets->checkbox_ptr_icon(ICON_FA_HAMMER, "Steal Pickaxe", &settings::miscellaneous::steal_pickaxe);

								gui->dummy(ImVec2(0, SCALE(18)));
								ImGui::PushFont(font->get(inter_medium_data, 12));
								ImGui::TextColored(accent_soft, "Status");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(6)));

								ImGui::BeginGroup();
								ImGui::PushFont(font->get(icon_data, 13));
								ImGui::TextColored(accent_active, ICON_FA_BULLSEYE);
								ImGui::PopFont();
								ImGui::SameLine(0.f, SCALE(8));
								ImGui::PushFont(font->get(inter_medium_data, 13));
								ImGui::TextColored(text_bright, "Target: %llu", settings::miscellaneous::skinchanger_target);
								ImGui::PopFont();
								ImGui::EndGroup();

								gui->dummy(ImVec2(0, SCALE(4)));

								ImGui::BeginGroup();
								ImGui::PushFont(font->get(icon_data, 13));
								ImGui::TextColored(skinchanger->m_cached_skin.valid ? accent_active : text_dim, ICON_FA_DATABASE);
								ImGui::PopFont();
								ImGui::SameLine(0.f, SCALE(8));
								ImGui::PushFont(font->get(inter_medium_data, 13));
								ImGui::TextColored(skinchanger->m_cached_skin.valid ? ImVec4(0.45f, 0.95f, 0.65f, 1.f) : text_dim,
									"Cache: %s", skinchanger->m_cached_skin.valid ? "VALID" : "EMPTY");
								ImGui::PopFont();
								ImGui::EndGroup();

								gui->dummy(ImVec2(0, SCALE(10)));
							}
							widgets->end_child();
						}
						gui->end_group();
						gui->sameline();
						gui->begin_group();
						{
							widgets->begin_child("TARGETS", ICON_FA_USERS);
							{
								ImGui::PushFont(font->get(inter_medium_data, 12));
								ImGui::TextColored(accent_soft, "Pick a player");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushFont(font->get(inter_medium_data, 12));
								ImGui::PushStyleColor(ImGuiCol_Text, text_dim);
								ImGui::TextUnformatted("Select row - enables skinchanger when you click.");
								ImGui::PopStyleColor();
								ImGui::PopFont();

								gui->dummy(ImVec2(0, SCALE(6)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(6)));

								const float list_w = ImMin(SCALE(310), ImGui::GetContentRegionAvail().x);

								ImGui::PushStyleColor(ImGuiCol_ChildBg, draw->get_clr(ImVec4(0.05f, 0.05f, 0.08f, 0.95f), 1.f));
								ImGui::PushStyleColor(ImGuiCol_Border, draw->get_clr(accent_active, 0.35f));
								ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, SCALE(12));
								ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, SCALE(1));
								ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(SCALE(8), SCALE(8)));

								ImGui::BeginChild("##skinscroll", ImVec2(list_w, SCALE(308)), true, 0);

								{
									std::lock_guard<std::mutex> lock(skinchanger->list_mutex);
									if (skinchanger->player_list.empty()) {
										ImGui::BeginGroup();
										ImGui::PushFont(font->get(icon_data, 14));
										ImGui::TextColored(accent_soft, ICON_FA_USERS);
										ImGui::PopFont();
										gui->dummy(ImVec2(0, SCALE(6)));
										ImGui::PushFont(font->get(inter_medium_data, 13));
										ImGui::PushStyleColor(ImGuiCol_Text, text_dim);
										ImGui::TextWrapped("Scanning lobby for players...");
										ImGui::PopStyleColor();
										ImGui::PopFont();
										ImGui::EndGroup();
									} else {
										ImFont* f_icon = font->get(icon_data, 13);
										ImFont* f_text = font->get(inter_medium_data, 14);
										ImDrawList* row_dl = ImGui::GetWindowDrawList();

										ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, SCALE(5)));

										for (int i = 0; i < (int)skinchanger->player_list.size(); i++) {
											auto& sp = skinchanger->player_list[i];
											const bool is_selected = (settings::miscellaneous::skinchanger_target == (uint64_t)sp.pawn);

											std::string disp = sp.name;
											if (disp.empty()) disp = "Player";
											if (sp.holding_pickaxe) disp += "  *";

											ImGui::PushID((int)(uintptr_t)sp.pawn);

											const float row_h = SCALE(36);
											const float pad_x = SCALE(10);
											const float avail_w = ImGui::GetContentRegionAvail().x;
											const ImVec2 row_screen0 = ImGui::GetCursorScreenPos();
											const ImVec2 row_max = ImVec2(row_screen0.x + avail_w, row_screen0.y + row_h);

											ImGui::InvisibleButton("row", ImVec2(avail_w, row_h));
											const bool hovered = ImGui::IsItemHovered();

											if (ImGui::IsItemClicked()) {
												settings::miscellaneous::skinchanger_target = (uint64_t)sp.pawn;
												settings::miscellaneous::skinchanger_enabled = true;
											}

											if (is_selected) {
												const ImU32 fill = draw->get_clr(ImVec4(
													clr->accent.Value.x * 0.18f + 0.04f,
													clr->accent.Value.y * 0.18f + 0.04f,
													clr->accent.Value.z * 0.22f + 0.08f,
													0.92f), 1.f);
												const ImU32 rim = draw->get_clr(ImVec4(
													clr->accent.Value.x * 0.5f + 0.2f,
													clr->accent.Value.y * 0.5f + 0.2f,
													clr->accent.Value.z * 0.55f + 0.25f,
													1.f), 1.f);
												const float rad = SCALE(10);
												row_dl->AddRectFilled(row_screen0, row_max, fill, rad);
												row_dl->AddRect(row_screen0, row_max, rim, rad, 0, SCALE(1.25f));
											} else if (hovered) {
												row_dl->AddRectFilled(row_screen0, row_max, draw->get_clr(ImVec4(1.f, 1.f, 1.f, 0.04f), 1.f), SCALE(8));
											}

											const char* row_icon = sp.holding_pickaxe ? ICON_FA_HAMMER : ICON_FA_USER;
											row_dl->AddText(f_icon, 13.f, row_screen0 + ImVec2(pad_x, SCALE(9)),
												draw->get_clr(is_selected ? accent_active : ImVec4(0.55f, 0.54f, 0.62f, 1.f), 1.f), row_icon);
											row_dl->AddText(f_text, 14.f, ImVec2(row_screen0.x + SCALE(36), row_screen0.y + SCALE(9)),
												draw->get_clr(is_selected ? ImVec4(0.98f, 0.98f, 1.f, 1.f) : text_bright, 1.f),
												disp.c_str(), disp.c_str() + disp.size());

											ImGui::SetCursorScreenPos(ImVec2(row_screen0.x, row_screen0.y + row_h));

											ImGui::PopID();
										}

										ImGui::PopStyleVar();
									}
								}

								ImGui::Dummy(ImVec2(0, SCALE(4)));
								ImGui::EndChild();
								ImGui::PopStyleVar(3);
								ImGui::PopStyleColor(2);
							}
							widgets->end_child();
						}
						gui->end_group();
					}
					else if (var->gui.section_index == 5)
					{
						const ImVec4 accent_active = ImVec4(clr->accent.Value.x, clr->accent.Value.y, clr->accent.Value.z, 1.f);
						const ImVec4 accent_soft = ImVec4(clr->accent.Value.x * 0.45f, clr->accent.Value.y * 0.45f, clr->accent.Value.z * 0.45f, 1.f);

						gui->begin_group();
						{
							widgets->begin_child("TEXT", ICON_FA_FONT);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "On-screen text");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->checkbox_ptr_icon(ICON_FA_ALIGN_LEFT, "Text outline (ESP labels)", &settings::style::text_outlined);
								widgets->checkbox_ptr_icon(ICON_FA_GAUGE_SIMPLE, "Performance skeleton (lighter)", &settings::style::performance);
							}
							widgets->end_child();

							widgets->begin_child("COLORS", ICON_FA_DROPLET);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Player ESP palette");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->color_edit_ptr("Visible (LOS)", &settings::player::visible_color);
								widgets->color_edit_ptr("Occluded", &settings::player::invisible_color);
								widgets->color_edit_ptr("Aimbot FOV ring", &settings::style::fov_circle_color);
							}
							widgets->end_child();

							widgets->begin_child("CHAMS", ICON_FA_CUBE);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Player / weapon materials");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->checkbox_ptr_icon(ICON_FA_USER, "Player chams", &settings::miscellaneous::chams_enabled);
								widgets->checkbox_ptr_icon(ICON_FA_GUN, "Weapon chams", &settings::miscellaneous::weapon_chams_enabled);
								widgets->dropdown_cfg("Material preset", &settings::miscellaneous::chams_material, { "Ribbon", "Shield", "RezIn", "Glass", "FishBowl" });
								widgets->slider_float_ptr("Emissive strength", &settings::miscellaneous::chams_emissive, 50.f, 2000.f, "%.0f");
								widgets->color_edit_ptr("Body tint", &settings::miscellaneous::chams_color);
								widgets->color_edit_ptr("Weapon tint", &settings::miscellaneous::weapon_chams_color);
								widgets->checkbox_ptr_icon(ICON_FA_VECTOR_SQUARE, "Wireframe", &settings::miscellaneous::chams_wireframe);
								widgets->checkbox_ptr_icon(ICON_FA_LAYER_GROUP, "Ignore depth", &settings::miscellaneous::chams_ignore_depth);
								widgets->checkbox_ptr_icon(ICON_FA_CLONE, "Two-sided", &settings::miscellaneous::chams_two_sided);
								widgets->checkbox_ptr_icon(ICON_FA_EYE, "Visible actors only", &settings::miscellaneous::chams_visible_only);
								widgets->checkbox_ptr_icon(ICON_FA_USER_SECRET, "Include local player", &settings::miscellaneous::chams_local_player);
								widgets->checkbox_ptr_icon(ICON_FA_SUN, "Dynamic area light", &settings::miscellaneous::chams_dynamic_area_light);
							}
							widgets->end_child();
						}
						gui->end_group();
						gui->sameline();
						gui->begin_group();
						{
							widgets->begin_child("LINES", ICON_FA_PENCIL);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Box, skeleton, snaplines");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->checkbox_ptr_icon(ICON_FA_SQUARE_FULL, "Black halo on lines", &settings::style::line_black_outline);
								widgets->slider_float_ptr("Halo thickness", &settings::style::line_outline_pad, 0.f, 4.f, "%.1f");
								widgets->slider_float_ptr("ESP line thickness", &settings::style::esp_line_thickness, 0.5f, 5.f, "%.1f");
								widgets->slider_float_ptr("FOV / radar line width", &settings::style::geometry_line_thickness, 0.5f, 8.f, "%.1f");
							}
							widgets->end_child();

							widgets->begin_child("FOV RING", ICON_FA_CIRCLE_NOTCH);
							{
								ImGui::PushFont(font->get(inter_medium_data, 11));
								ImGui::TextColored(accent_soft, "Aimbot circle (Show FOV)");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(4)));

								widgets->slider_int_ptr("Smoothness (segments)", &settings::style::fov_circle_segments, 8, 256, "%d");
							}
							widgets->end_child();
						}
						gui->end_group();
					}

					else if (var->gui.section_index == 4) // CONFIGS
					{
						static std::string active_cfg = "";
						static char name_buf[64] = "";

						const ImVec4 accent_active = ImVec4(clr->accent.Value.x, clr->accent.Value.y, clr->accent.Value.z, 1.f);
						const ImVec4 accent_soft = ImVec4(clr->accent.Value.x * 0.45f, clr->accent.Value.y * 0.45f, clr->accent.Value.z * 0.45f, 1.f);
						const ImVec4 text_dim = ImVec4(0.55f, 0.54f, 0.65f, 1.f);
						const ImVec4 text_bright = ImVec4(0.93f, 0.93f, 0.97f, 1.f);
						const float name_field_max_w = SCALE(300);

						gui->begin_group();
						{
							widgets->begin_child("LIBRARY", ICON_FA_BOOK);
							{
								ImGui::PushFont(font->get(inter_medium_data, 12));
								ImGui::TextColored(accent_soft, "Save & manage");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(4)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(8)));

								widgets->simple_text_field("Profile name", name_buf, sizeof(name_buf), name_field_max_w);

								gui->dummy(ImVec2(0, SCALE(6)));

								if (widgets->button_clean_icon(ICON_FA_FLOPPY_DISK, "Save").value_changed) {
									if (strlen(name_buf) > 0) {
										config_mgr->save(name_buf);
										active_cfg = name_buf;
										config_mgr->refresh_list();
										notify->add_notify("Saved " + active_cfg, success);
									} else {
										notify->add_notify("Enter a profile name first", warning);
									}
								}

								if (widgets->button_clean_icon(ICON_FA_FILE_IMPORT, "Load").value_changed) {
									if (!active_cfg.empty()) {
										config_mgr->load(active_cfg);
										notify->add_notify("Loaded " + active_cfg, success);
									} else {
										notify->add_notify("Select a profile in the list", warning);
									}
								}

								if (widgets->button_clean_icon(ICON_FA_TRASH, "Delete").value_changed) {
									if (!active_cfg.empty()) {
										config_mgr->remove(active_cfg);
										notify->add_notify("Deleted " + active_cfg, warning);
										active_cfg = "";
										memset(name_buf, 0, sizeof(name_buf));
										config_mgr->refresh_list();
									} else {
										notify->add_notify("Nothing selected to delete", warning);
									}
								}

								if (widgets->button_clean_icon(ICON_FA_ROTATE, "Refresh").value_changed) {
									config_mgr->refresh_list();
									notify->add_notify("List refreshed", success);
								}

								if (widgets->button_clean_icon(ICON_FA_FOLDER_OPEN, "Open folder").value_changed) {
									ShellExecuteA(NULL, "open", "C:\\FortMeme\\Configs", NULL, NULL, SW_SHOWDEFAULT);
								}
							}
							widgets->end_child();
						}
						gui->end_group();

						gui->sameline();

						gui->begin_group();
						{
							widgets->begin_child("PROFILES", ICON_FA_FOLDER_OPEN);
							{
								ImGui::PushFont(font->get(inter_medium_data, 12));
								ImGui::TextColored(accent_soft, "Your profiles");
								ImGui::PopFont();
								gui->dummy(ImVec2(0, SCALE(2)));
								ImGui::PushFont(font->get(inter_medium_data, 12));
								ImGui::PushStyleColor(ImGuiCol_Text, text_dim);
								ImGui::TextUnformatted("Single-click to pick - double-click loads.");
								ImGui::PopStyleColor();
								ImGui::PopFont();

								gui->dummy(ImVec2(0, SCALE(6)));
								ImGui::PushStyleColor(ImGuiCol_Separator, draw->get_clr(accent_active, 0.55f));
								ImGui::Separator();
								ImGui::PopStyleColor();
								gui->dummy(ImVec2(0, SCALE(6)));

								const float list_w = ImMin(SCALE(290), ImGui::GetContentRegionAvail().x);

								ImGui::PushStyleColor(ImGuiCol_ChildBg, draw->get_clr(ImVec4(0.05f, 0.05f, 0.08f, 0.95f), 1.f));
								ImGui::PushStyleColor(ImGuiCol_Border, draw->get_clr(accent_active, 0.35f));
								ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, SCALE(12));
								ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, SCALE(1));
								ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(SCALE(8), SCALE(8)));

								ImGui::BeginChild("##cfg_profile_scroll", ImVec2(list_w, SCALE(308)), true, 0);

								ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, SCALE(5)));

								if (config_mgr->config_list.empty()) {
									ImGui::PushFont(font->get(inter_medium_data, 13));
									ImGui::PushStyleColor(ImGuiCol_Text, text_dim);
									ImGui::TextWrapped("No profiles yet. Type a name and press Save.");
									ImGui::PopStyleColor();
									ImGui::PopFont();
								} else {
									ImFont* f_icon = font->get(icon_data, 13);
									ImFont* f_text = font->get(inter_medium_data, 14);
									ImDrawList* row_dl = ImGui::GetWindowDrawList();

									for (const auto& prof_name : config_mgr->config_list) {
										const bool is_active = (active_cfg == prof_name);

										ImGui::PushID(prof_name.c_str());

										const float row_h = SCALE(36);
										const float pad_x = SCALE(10);
										const float avail_w = ImGui::GetContentRegionAvail().x;
										const ImVec2 row_screen0 = ImGui::GetCursorScreenPos();
										const ImVec2 row_max = ImVec2(row_screen0.x + avail_w, row_screen0.y + row_h);

										ImGui::InvisibleButton("hit", ImVec2(avail_w, row_h));
										const bool hovered = ImGui::IsItemHovered();

										if (ImGui::IsItemClicked()) {
											active_cfg = prof_name;
											strcpy_s(name_buf, prof_name.c_str());
										}
										if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
											config_mgr->load(prof_name);
											active_cfg = prof_name;
											notify->add_notify("Loaded " + prof_name, success);
										}

										if (is_active) {
											const ImU32 fill = draw->get_clr(ImVec4(
												clr->accent.Value.x * 0.18f + 0.04f,
												clr->accent.Value.y * 0.18f + 0.04f,
												clr->accent.Value.z * 0.22f + 0.08f,
												0.92f), 1.f);
											const ImU32 rim = draw->get_clr(ImVec4(
												clr->accent.Value.x * 0.5f + 0.2f,
												clr->accent.Value.y * 0.5f + 0.2f,
												clr->accent.Value.z * 0.55f + 0.25f,
												1.f), 1.f);
											const float rad = SCALE(10);
											row_dl->AddRectFilled(row_screen0, row_max, fill, rad);
											row_dl->AddRect(row_screen0, row_max, rim, rad, 0, SCALE(1.25f));
										} else if (hovered) {
											row_dl->AddRectFilled(row_screen0, row_max, draw->get_clr(ImVec4(1.f, 1.f, 1.f, 0.04f), 1.f), SCALE(8));
										}

										ImVec2 ip = row_screen0 + ImVec2(pad_x, SCALE(9));
										row_dl->AddText(f_icon, 13.f, ip, draw->get_clr(is_active ? accent_active : ImVec4(0.55f, 0.54f, 0.62f, 1.f), 1.f), ICON_FA_FILE_LINES);
										const float text_x = row_screen0.x + SCALE(36);
										row_dl->AddText(f_text, 14.f, ImVec2(text_x, row_screen0.y + SCALE(9)),
											draw->get_clr(is_active ? ImVec4(0.98f, 0.98f, 1.f, 1.f) : text_bright, 1.f),
											prof_name.c_str(), prof_name.c_str() + prof_name.size());

										ImGui::SetCursorScreenPos(ImVec2(row_screen0.x, row_screen0.y + row_h));

										ImGui::PopID();
									}
								}

								ImGui::PopStyleVar();
								ImGui::Dummy(ImVec2(0, SCALE(4)));
								ImGui::EndChild();

								ImGui::PopStyleVar(3);
								ImGui::PopStyleColor(2);
							}
							widgets->end_child();
						}
						gui->end_group();
					}

					gui->pop_var();
				}

				gui->dummy(ImVec2(0, 0));
				ImGui::PopFont();
			}
		}
		gui->end_content();
	}

	ImVec2 main_menu_pos = gui->window_pos();
	float main_menu_width = gui->window_size().x;

	gui->end();
}
