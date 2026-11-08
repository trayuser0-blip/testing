#pragma once
#include "sdk/sdk.hpp"
#include "game/Exploits/exploits.h"
#include "game/Exploits/chams.hxx"
#include "game/Exploits/skinchanger.hxx"
#include <process.h>
#include "utils/vixen_hook.hpp"

namespace essential_hooks {

	fvector location_o = fvector();
	frotator rotation_o = frotator();

	ulocalplayer* local_player_old = 0;
	aplayercontroller* player_controller_old = 0;

	static Hook::VTableHook LocalPlayerHook;
	static Hook::VTableHook PlayerControllerHook;

	using get_view_point_sim = void(*)(ulocalplayer* local_player, fminimalviewinfo* out_viewinfo, BYTE stereo_pass);
	get_view_point_sim get_view_point_o = 0;

	void get_view_point(ulocalplayer* local_player, fminimalviewinfo* out_viewinfo, BYTE stereo_pass) {
		get_view_point_o(local_player, out_viewinfo, stereo_pass);

		if (fortnite::temp::silent_active) {
			out_viewinfo->location = location_o;
			out_viewinfo->rotation = rotation_o;
		}

		if (fortnite::temp::sticky_silent_active) {
			out_viewinfo->location = location_o;
			out_viewinfo->location.z += 40;
			out_viewinfo->rotation = fortnite::temp::sticky_silent_rotation;
		}
	}

	using get_player_view_point_sim = void(*)(aplayercontroller* controller, fvector* location, frotator* rotation);
	get_player_view_point_sim get_player_view_point_o = 0;

	void get_player_view_point(aplayercontroller* controller, fvector* location, frotator* rotation) {
		get_player_view_point_o(controller, location, rotation);

		location_o = *location;
		rotation_o = *rotation;

		if (fortnite::temp::silent_active) {
			*rotation = fortnite::temp::silent_rotation;
		}

		if (fortnite::temp::sticky_silent_active) {
			*rotation = fortnite::temp::sticky_silent_rotation;
		}
	}

	void reset() {
		local_player_old = nullptr;
		player_controller_old = nullptr;
		get_view_point_o = nullptr;
		get_player_view_point_o = nullptr;
	}

	void run(aplayercontroller* controller, ulocalplayer* local_player) {
		if (settings::aimbot::silent || settings::aimbot::sticky_silent) {
			if (!valid_pointer(uintptr_t(local_player)) || !valid_pointer(uintptr_t(controller))) {
				reset();
				return;
			}

			if (local_player != essential_hooks::local_player_old) {
				if (valid_pointer(uintptr_t(essential_hooks::local_player_old)) && get_view_point_o) {
					LocalPlayerHook.Unhook(get_view_point_o, essential_hooks::local_player_old);
				}
				LocalPlayerHook.Hook(local_player, (void*)get_view_point, 80, &get_view_point_o);
				essential_hooks::local_player_old = local_player;
			}

			if (controller != essential_hooks::player_controller_old) {
				if (valid_pointer(uintptr_t(essential_hooks::player_controller_old)) && get_player_view_point_o) {
					PlayerControllerHook.Unhook(get_player_view_point_o, essential_hooks::player_controller_old);
				}
				PlayerControllerHook.Hook(controller, (void*)get_player_view_point, 256, &get_player_view_point_o);
				essential_hooks::player_controller_old = controller;
			}
		} else {
			if (valid_pointer(uintptr_t(essential_hooks::local_player_old)) && get_view_point_o) {
				LocalPlayerHook.Unhook(get_view_point_o, essential_hooks::local_player_old);
			}
			essential_hooks::local_player_old = nullptr;

			if (valid_pointer(uintptr_t(essential_hooks::player_controller_old)) && get_player_view_point_o) {
				PlayerControllerHook.Unhook(get_player_view_point_o, essential_hooks::player_controller_old);
			}
			essential_hooks::player_controller_old = nullptr;
		}
	}
}

namespace gameloop {

	inline std::wstring s2wc(const char* c) {
		const size_t cSize = (strlen(c) + 1);
		std::wstring wc(cSize, L'#');
		mbstowcs(&wc[0], c, cSize);
		return wc;
	}

	void draw_target_line(aplayercontroller* controller, player_pawn* local_pawn,
								fvector nearest_3d) {
		if (!settings::aimbot::target_line || !controller || !controller->is_valid() || !local_pawn) return;

		auto current_weapon = local_pawn->get_current_weapon();
		if (!current_weapon || !current_weapon->is_valid() || skinchanger->is_pickaxe(current_weapon)) return;

		auto weapon_data = current_weapon->get_weapon_data();
		if (!weapon_data || !weapon_data->is_valid()) return;

		auto weapon_name = defines::system->get_object_name(weapon_data);
		if (defines::string->contains(weapon_name.c_str(), L"Consumable", false, false) ||
			defines::string->contains(weapon_name.c_str(), L"Medkit", false, false) ||
			defines::string->contains(weapon_name.c_str(), L"Shield", false, false) ||
			defines::string->contains(weapon_name.c_str(), L"Bottle", false, false) ||
			defines::string->contains(weapon_name.c_str(), L"Bandage", false, false) ||
			defines::string->contains(weapon_name.c_str(), L"Slurp", false, false)) {
			return;
		}

		fvector target_3d = (nearest_3d.x != 0 || nearest_3d.y != 0 || nearest_3d.z != 0)
			? nearest_3d : fortnite::aimbot_target_3d;
		if (target_3d.x == 0 && target_3d.y == 0 && target_3d.z == 0) return;

		fvector2d target_2d = fvector2d();
		if (!controller->w2s(target_3d, &target_2d)) return;

		fvector2d start_pos = render::screen_center;
		fvector muzzle_3d = current_weapon->get_muzzle_location();
		if (muzzle_3d.x != 0 || muzzle_3d.y != 0 || muzzle_3d.z != 0) {
			fvector2d muzzle_screen;
			if (controller->w2s(muzzle_3d, &muzzle_screen))
				start_pos = muzzle_screen;
		}

		render::line_with_outline(start_pos, target_2d, fortnite::target_visible ? settings::player::visible_color : settings::player::invisible_color, 1.f);
	}

	void run_internal(ugameviewportclient* viewport, ucanvas* canvas) {
			static player_pawn* target_player = nullptr;
			target_player = nullptr;
			fortnite::aimbot_target_3d = fvector();
			auto screen_size = fvector2d((double)canvas->size_x(), (double)canvas->size_y());
			auto center = fvector2d((double)screen_size.x / 2, (double)screen_size.y / 2);
			bool player_silent = false;

			auto world = viewport->get_world();
			if (!world || !world->is_valid()) {
				skinchanger->force_cleanup();
				chams->force_cleanup();
				return;
			}

			static uintptr_t last_world = 0;
			static uint64_t  world_start_time = 0;

			if (uintptr_t(world) != last_world) {
				skinchanger->force_cleanup();
				chams->force_cleanup();
				essential_hooks::reset();
				settings::miscellaneous::skinchanger_target = 0;
				last_world = uintptr_t(world);
				world_start_time = GetTickCount64();
			}

			// Safety Delay: Avoid any actor operations while the world is still loading objects
			if (GetTickCount64() - world_start_time < 5000) {
				return;
			}

			auto game_instance = defines::game_statics->get_game_instance(world);
			if (!game_instance) return;
			auto local_players = game_instance->get_local_players();
			if (local_players.size() <= 0) return;
			auto local_player = local_players[0];
			if (!local_player || !local_player->is_valid()) return;

			auto controller = defines::game_statics->get_player_controller(world, std::int32_t(0));
			if (!controller || !controller->is_valid()) {
				skinchanger->force_cleanup();
				chams->force_cleanup();
				return;
			}
			auto camera_manager = defines::game_statics->get_player_camera_manager(world, std::int32_t(0));

			bool Update__GetPlayerViewPoint__bSilentAimActive = false;
			frotator Update__GetPlayerViewPoint__SilentLocationTarget = frotator();

			render::world = world;
			render::canvas = canvas;
			render::controller = controller;
			render::screen_center = center;
			render::screen_size = screen_size;

			render::esp_stroke_thickness = settings::style::esp_line_thickness;
			render::outline = settings::style::line_black_outline;
			render::outlinethickness = settings::style::line_outline_pad;
			render::linethickness = settings::style::geometry_line_thickness;

			auto camera_location = camera_manager->get_camera_location();
			auto camera_rotation = camera_manager->get_camera_rotation();
			auto camera_fov = camera_manager->get_fov_angle();
			float fov_radius = (settings::aimbot::field_of_view / 500.0f) * (float)(screen_size.x / 2.0);

			if (settings::aimbot::show_fov) {
				int seg = settings::style::fov_circle_segments;
				if (seg < 8) seg = 8;
				if (seg > 256) seg = 256;
				flinearcolor fov_col = settings::style::fov_circle_color;
				if (fov_col.a <= 0.f) fov_col = flinearcolor(1.f, 1.f, 1.f, 1.f);
				render::circle(center, (int)fov_radius, seg, fov_col);
			}

			auto local_pawn_holding_pickaxe = false;
			auto local_pawn = controller->get_pawn();
			fortnite::local_pawn = local_pawn;
			fortnite::local_controller = controller;





				{
					std::lock_guard<std::mutex> lock(skinchanger->list_mutex);
					skinchanger->temp_player_list.clear();
				}

			if (settings::player::enable) {
				double closest_distance = DBL_MAX;

				tarray<uobject*> cached_player_array = defines::game_statics->get_all_actors_of_class(world, defines::actor_player_class);

				for (int i = 0; i < cached_player_array.size(); i++) {
					if (!cached_player_array.valid(i)) continue;

					auto player = (player_pawn*)cached_player_array[i];
					if (player == local_pawn) continue;
					if (!player || !player->is_valid()) continue;
					if (IsBadReadPtr(player, 8)) continue;
					if (defines::lib->OnSameTeam(local_pawn, player)) continue;

					if (player->is_dead()) continue;



					auto object_name = defines::system->get_object_name(player);

					if (!defines::string->contains(object_name.c_str(), L"PlayerPawn", false, false)) continue;

					fstring name_str = defines::lib->get_player_name_safe(player, defines::actor_player_nameprivate);
					std::string ascii_name = "Player";
					if (name_str.valid() && name_str.c_str()) {
						std::wstring ws(name_str.c_str());
						ascii_name = "";
						for (auto c : ws) {
							if (c > 31 && c < 127) ascii_name += (char)c;
						}
						if (ascii_name.empty()) ascii_name = "Player";
					}

					exploits::skin_player_t sp;
					sp.pawn = player;
					sp.name = ascii_name;
					// Optimization: only check pickaxe if needed
					if (settings::miscellaneous::steal_pickaxe || settings::miscellaneous::skinchanger_enabled) {
						sp.holding_pickaxe = skinchanger->is_pickaxe(player->get_current_weapon());
					}
					{
						std::lock_guard<std::mutex> lock(skinchanger->list_mutex);
						skinchanger->temp_player_list.push_back(sp);
					}

					auto skeleton_color = flinearcolor(0.f, 1.f, 1.f, 1.f);
					flinearcolor esp_color;
					auto black_color = flinearcolor(0.f, 0.f, 0.f, 1.f);

					auto world_location = player->get_actor_location();
					fvector2d screen_pos;
					if (!controller->w2s(world_location, &screen_pos) || !render::is_on_screen(screen_pos)) continue;

					auto max_distance = settings::player::max_distance;
					double distance = defines::math->vector_distance(world_location, camera_location) * 0.01;
					if (distance > max_distance) continue;

					bool visible = defines::lib->check_line_of_sight_to_actor_with_channel(camera_location, player, ECollisionChannel::ECC_Visibility, local_pawn);
					if (visible) {
						fortnite::visible = true;
						esp_color = settings::player::visible_color;
					}
					else {
						fortnite::visible = false;
						esp_color = settings::player::invisible_color;
					}



					auto mesh = player->get_pawn_mesh();
					if (!mesh) continue;

					if (!settings::style::performance) {
						if (!mesh->get_full_skeleton(controller, fortnite::player_skeleton)) continue;
					}
					else {
						if (!mesh->get_performance_skeleton(controller, fortnite::player_skeleton)) continue;
					}

					if (settings::player::skeleton) {
						render::line_with_outline(fortnite::player_skeleton.s_head, fortnite::player_skeleton.s_neck, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_neck, fortnite::player_skeleton.s_chest, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_chest, fortnite::player_skeleton.s_pelvis, esp_color, 1.f);

						render::line_with_outline(fortnite::player_skeleton.s_chest, fortnite::player_skeleton.s_rshoulder, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_rshoulder, fortnite::player_skeleton.s_relbow, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_relbow, fortnite::player_skeleton.s_rhand, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_pelvis, fortnite::player_skeleton.s_rthigh, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_rthigh, fortnite::player_skeleton.s_rknee, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_rknee, fortnite::player_skeleton.s_rfoot, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_rfoot, fortnite::player_skeleton.s_rfootout, esp_color, 1.f);

						render::line_with_outline(fortnite::player_skeleton.s_chest, fortnite::player_skeleton.s_lshoulder, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_lshoulder, fortnite::player_skeleton.s_lelbow, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_lelbow, fortnite::player_skeleton.s_lhand, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_pelvis, fortnite::player_skeleton.s_lthigh, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_lthigh, fortnite::player_skeleton.s_lknee, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_lknee, fortnite::player_skeleton.s_lfoot, esp_color, 1.f);
						render::line_with_outline(fortnite::player_skeleton.s_lfoot, fortnite::player_skeleton.s_lfootout, esp_color, 1.f);
					}


					fvector origin, extent; float radius;
					defines::system->get_component_bounds(mesh, &origin, &extent, &radius);

					double min_x = DBL_MAX;
					double max_x = DBL_MIN;
					double min_y = DBL_MAX;
					double max_y = DBL_MIN;

					for (int i = 0; i < 8; i++) {
						fvector corner;
						corner.x = (i & 1) ? extent.x : -extent.x;
						corner.y = (i & 2) ? extent.y : -extent.y;
						corner.z = (i & 4) ? extent.z : -extent.z;

						fvector2d corner_screen;
						if (controller->w2s(origin + corner, &corner_screen)) {
							if (corner_screen.x < min_x) min_x = corner_screen.x;
							if (corner_screen.x > max_x) max_x = corner_screen.x;
							if (corner_screen.y < min_y) min_y = corner_screen.y;
							if (corner_screen.y > max_y) max_y = corner_screen.y;
						}
					}

					float box_x = (float)min_x;
					float box_y = (float)min_y;
					float box_w = (float)(max_x - min_x);
					float box_h = (float)(max_y - min_y);

					if (settings::player::box) {
						if (settings::player::box_type == 0) {
							render::line_with_outline(fvector2d(box_x, box_y), fvector2d(box_x + box_w, box_y), esp_color, 1.f);
							render::line_with_outline(fvector2d(box_x, box_y + box_h), fvector2d(box_x + box_w, box_y + box_h), esp_color, 1.f);
							render::line_with_outline(fvector2d(box_x, box_y), fvector2d(box_x, box_y + box_h), esp_color, 1.f);
							render::line_with_outline(fvector2d(box_x + box_w, box_y), fvector2d(box_x + box_w, box_y + box_h), esp_color, 1.f);
						}
						else if (settings::player::box_type == 1) {
							float corner_w = box_w / 4.0f;
							float corner_h = box_h / 6.0f;


							render::line_with_outline(fvector2d(box_x, box_y), fvector2d(box_x + corner_w, box_y), esp_color, 1.f);
							render::line_with_outline(fvector2d(box_x, box_y), fvector2d(box_x, box_y + corner_h), esp_color, 1.f);


							render::line_with_outline(fvector2d(box_x + box_w, box_y), fvector2d(box_x + box_w - corner_w, box_y), esp_color, 1.f);
							render::line_with_outline(fvector2d(box_x + box_w, box_y), fvector2d(box_x + box_w, box_y + corner_h), esp_color, 1.f);

							render::line_with_outline(fvector2d(box_x, box_y + box_h), fvector2d(box_x + corner_w, box_y + box_h), esp_color, 1.f);
							render::line_with_outline(fvector2d(box_x, box_y + box_h), fvector2d(box_x, box_y + box_h - corner_h), esp_color, 1.f);


							render::line_with_outline(fvector2d(box_x + box_w, box_y + box_h), fvector2d(box_x + box_w - corner_w, box_y + box_h), esp_color, 1.f);
							render::line_with_outline(fvector2d(box_x + box_w, box_y + box_h), fvector2d(box_x + box_w, box_y + box_h - corner_h), esp_color, 1.f);
						}
						else if (settings::player::box_type == 2) {
							fvector corners[8];
							for (int i = 0; i < 8; i++) {
								fvector corner;
								corner.x = (i & 1) ? extent.x : -extent.x;
								corner.y = (i & 2) ? extent.y : -extent.y;
								corner.z = (i & 4) ? extent.z : -extent.z;
								corners[i] = origin + corner;
							}

							fvector2d s_corners[8];
							bool all_valid = true;
							for (int i = 0; i < 8; i++) {
								if (!controller->w2s(corners[i], &s_corners[i])) {
									all_valid = false;
									break;
								}
							}

							if (all_valid) {
								render::line_with_outline(s_corners[0], s_corners[1], esp_color, 1.f);
								render::line_with_outline(s_corners[1], s_corners[3], esp_color, 1.f);
								render::line_with_outline(s_corners[3], s_corners[2], esp_color, 1.f);
								render::line_with_outline(s_corners[2], s_corners[0], esp_color, 1.f);
								render::line_with_outline(s_corners[4], s_corners[5], esp_color, 1.f);
								render::line_with_outline(s_corners[5], s_corners[7], esp_color, 1.f);
								render::line_with_outline(s_corners[7], s_corners[6], esp_color, 1.f);
								render::line_with_outline(s_corners[6], s_corners[4], esp_color, 1.f);
								render::line_with_outline(s_corners[0], s_corners[4], esp_color, 1.f);
								render::line_with_outline(s_corners[1], s_corners[5], esp_color, 1.f);
								render::line_with_outline(s_corners[2], s_corners[6], esp_color, 1.f);
								render::line_with_outline(s_corners[3], s_corners[7], esp_color, 1.f);
							}
						}
					}

					float top_offset = 5.0f;
					if (settings::player::player_name || settings::player::platform || settings::player::display_distance) {
						std::wstring combined_info_w = L"";

						if (settings::player::display_distance) {
							int rounded_distance = (int)(distance + 0.5f);
							wchar_t dist_buf[32];
							swprintf_s(dist_buf, L"[%dm] ", rounded_distance);
							combined_info_w += dist_buf;
						}

						if (settings::player::player_name) {
							fstring name_fs = defines::lib->get_player_name_safe(player, defines::actor_player_nameprivate);
							if (name_fs.valid()) {
								combined_info_w += name_fs.c_str();
							}
						}

						if (settings::player::platform) {
							player_state* state = player->get_player_state();
							if (std::uintptr_t(state) > 0x10000) {
								fstring platform_fs = state->get_platform();
								if (platform_fs.valid()) {
									combined_info_w += L" [" + std::wstring(platform_fs.c_str()) + L"]";
								}
							}
						}

						if (!combined_info_w.empty()) {
							fstring final_info(combined_info_w.c_str());
							fvector2d info_size = render::text_size(final_info);
							fvector2d info_pos = { box_x + (box_w / 2) - (info_size.x / 2), box_y - info_size.y - top_offset };
							render::text(final_info, info_pos, flinearcolor(1.f, 1.f, 1.f, 1.f), false, false, settings::style::text_outlined);
							top_offset += info_size.y + 2.0f;
						}
					}

					float bottom_offset = 2.0f;
					if (settings::player::active_weapon) {
							bool weapon_found = false;
							auto current_weapon = player->get_current_weapon();
							if (current_weapon) {
								auto weapon_data = current_weapon->get_weapon_data();
								if (weapon_data) {
									auto weapon_display_name = weapon_data->display_name();
									wchar_t* weapon_name_str = weapon_display_name.c_str();
									if (weapon_name_str) {
										wchar_t weapon_text[128];
										swprintf_s(weapon_text, L"[%s]", weapon_name_str);
										EFortRarity rarity = weapon_data->GetRarity();
										flinearcolor rarity_color = flinearcolor::GetColorByTier(rarity);

										fvector2d weapon_size = render::text_size(fstring(weapon_text));
										fvector2d weapon_pos = { box_x + (box_w / 2) - (weapon_size.x / 2), box_y + box_h + bottom_offset };
										render::text(fstring(weapon_text), weapon_pos, rarity_color, false, false, settings::style::text_outlined);
										bottom_offset += weapon_size.y + 2.0f;
										weapon_found = true;
									}
								}
							}
							if (!weapon_found) {
								fstring no_weapon_text(L"[No Weapon]");
								fvector2d no_weapon_size = render::text_size(no_weapon_text);
								fvector2d no_weapon_pos = { box_x + (box_w / 2) - (no_weapon_size.x / 2), box_y + box_h + bottom_offset };
								render::text(no_weapon_text, no_weapon_pos, flinearcolor(1.f, 1.f, 1.f, 1.f), false, false, settings::style::text_outlined);
								bottom_offset += no_weapon_size.y + 2.0f;
							}
					}


					if (settings::player::snapline) {
						fvector2d start_pos;
						fvector2d end_pos;

						if (settings::player::snapline_type == 0) {
							start_pos = { (float)canvas->size_x() / 2, (float)canvas->size_y() };
							end_pos = { (float)box_x + (box_w / 2), (float)box_y + box_h + bottom_offset };
						}
						else {
							start_pos = { (float)canvas->size_x() / 2, 0 };
							end_pos = { (float)box_x + (box_w / 2), (float)box_y - top_offset + 5.0f };
						}
						render::line_with_outline(start_pos, end_pos, esp_color, 1.f);
					}

					if (settings::aimbot::enable) {
						fvector target_3d = fvector();
						if (settings::bones::aim_bone == 0) target_3d = fortnite::player_skeleton.head;
						else if (settings::bones::aim_bone == 1) target_3d = fortnite::player_skeleton.neck;
						else if (settings::bones::aim_bone == 2) target_3d = fortnite::player_skeleton.chest;
						else if (settings::bones::aim_bone == 3) target_3d = fortnite::player_skeleton.pelvis;

						fvector2d target_2d = fvector2d();
						if (controller->w2s(target_3d, &target_2d)) {
							auto dx = center.x - target_2d.x;
							auto dy = center.y - target_2d.y;
							auto fov_distance = sqrt(dx * dx + dy * dy);

							if (fov_distance <= fov_radius && fov_distance < closest_distance) {
								closest_distance = fov_distance;
								target_player = player;
								fortnite::aimbot_target_3d = target_3d;
								fortnite::target_visible = visible;
							}
						}
					}
				}


				if (settings::aimbot::target_line && target_player && local_pawn) {
					draw_target_line(controller, local_pawn, fortnite::aimbot_target_3d);
				}

			if (target_player && (settings::aimbot::silent || (GetAsyncKeyState(settings::aimbot::aim_key) & 0x8000))) {
					if (settings::aimbot::visible_only && !fortnite::target_visible) {
						fortnite::ResetCamera(camera_manager);
					}
					else {
						if (settings::aimbot::prediction && local_pawn) {
							auto current_weapon = local_pawn->get_current_weapon();

							if (current_weapon) {
								auto weapon_data = current_weapon->get_weapon_data();

								float proj_speed = *reinterpret_cast<float*>(current_weapon + 0x2224); //speed
								float proj_pitch = *reinterpret_cast<float*>(current_weapon + 0x2220); //gravity

								if (proj_speed > 0.f || proj_pitch > 0.f) {
									render::text(fstring(L"Prediction supported weapon in hand"), fvector2d(screen_size.x / 2.f, 50.f), flinearcolor(0.f, 1.f, 0.f, 1.f), true, false, true);
								}
								else {
									render::text(fstring(L"Not supported prediction weapon in hand"), fvector2d(screen_size.x / 2.f, 50.f), flinearcolor(1.f, 0.f, 0.f, 1.f), true, false, true);
								}

								if (weapon_data) {
									double projectile_speed = (double)proj_speed;
									double projectile_gravity = (double)proj_pitch;

									fvector target_velocity = target_player ? target_player->get_velocity() : fvector();
									fvector local_velocity = local_pawn->get_velocity();
									fvector relative_velocity = fvector(
										target_velocity.x - local_velocity.x,
										target_velocity.y - local_velocity.y,
										target_velocity.z - local_velocity.z
									);

									double player_distance = defines::math->vector_distance(camera_location, fortnite::aimbot_target_3d);
									if (projectile_speed > 0.0) {
										double time_to_target = player_distance / projectile_speed;
										fortnite::aimbot_target_3d.x += relative_velocity.x * time_to_target;
										fortnite::aimbot_target_3d.y += relative_velocity.y * time_to_target;
										double difference = camera_location.z - fortnite::aimbot_target_3d.z;
										double horizontal_distance = sqrt(
											pow(camera_location.x - fortnite::aimbot_target_3d.x, 2.0) +
											pow(camera_location.y - fortnite::aimbot_target_3d.y, 2.0)
										);
										double vertical_angle = atan2(fabs(difference), horizontal_distance);
										double angle = vertical_angle * (180.0 / 3.14159265358979323846);
										double factor = fabs(angle - 90.0) / 90.0;
										projectile_gravity *= factor;
										fortnite::aimbot_target_3d.z += relative_velocity.z * time_to_target +
											(fabs(projectile_gravity * -980.0) * (time_to_target * time_to_target)) * 0.5;
									}
								}
							}
						}

						if (settings::aimbot::silent)
						{
							Update__GetPlayerViewPoint__bSilentAimActive = true;
							Update__GetPlayerViewPoint__SilentLocationTarget = defines::math->find_look_at_rotation(camera_location, fortnite::aimbot_target_3d);
						}
						else if (GetAsyncKeyState(settings::aimbot::aim_key) & 0x8000)
							fortnite::Camera_Aimbot(camera_manager, fortnite::aimbot_target_3d);
					}
				}
				else {
					fortnite::ResetCamera(camera_manager);
					fortnite::target_visible = false;
				}

				if (settings::aimbot::triggerbot) {
					static bool triggerbot_active = false;
					auto target = controller->targeted_fort_pawn();
					bool hold_key_down = false;

					if (settings::aimbot::triggerbot_key > 0) {
						hold_key_down = (GetAsyncKeyState(settings::aimbot::triggerbot_key) & 0x8000) != 0;
					} else {
						hold_key_down = (GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0;
					}

					bool should_trigger = (target != nullptr) && (settings::aimbot::triggerbot_constant || (hold_key_down && settings::aimbot::triggerbot_left_shift));

					if (should_trigger) {
						if (!triggerbot_active) {
							local_pawn->pawn_start_fire();
							triggerbot_active = true;
						}
					} else {
						if (triggerbot_active) {
							local_pawn->pawn_stop_fire();
							triggerbot_active = false;
						}
					}
				}
			}
			else {
			}

			fortnite::temp::silent_active = Update__GetPlayerViewPoint__bSilentAimActive;
			fortnite::temp::silent_rotation = Update__GetPlayerViewPoint__SilentLocationTarget;


			if (settings::miscellaneous::skinchanger_enabled) {
				player_pawn* donor = nullptr;
				if (settings::miscellaneous::skinchanger_target != 0) {
					std::lock_guard<std::mutex> lock(skinchanger->list_mutex);
					for (const auto& sp : skinchanger->player_list) {
						if ((uint64_t)sp.pawn == settings::miscellaneous::skinchanger_target) {
							if (sp.pawn && sp.pawn->is_valid() && sp.pawn != local_pawn) {
								donor = sp.pawn;
							}
							break;
						}
					}
				}

				if (local_pawn && local_pawn->is_valid()) {
					skinchanger->apply(local_pawn, donor);
				}
			}
			else {
				// Immediate reset when checkbox is disabled
				skinchanger->manual_reset();
			}

			exploits::run(local_pawn, controller, camera_manager, world, target_player);
			essential_hooks::run(controller, local_player);

			fortnite::world_esp::render_containers(world, controller, camera_location);
			fortnite::world_esp::render_pickups(world, controller, camera_location);
			fortnite::world_esp::render_vehicles(world, controller, camera_location);
			fortnite::world_esp::render_misc(world, controller, camera_location);


			skinchanger->swap_list();
	}

	void run(ugameviewportclient* viewport, ucanvas* canvas) {
		__try {
			run_internal(viewport, canvas);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {}
	}
}

