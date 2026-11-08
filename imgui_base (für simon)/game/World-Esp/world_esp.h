#pragma once
#include "../../sdk/sdk.hpp"
#include "../../game/Exploits/chams.hxx"

namespace fortnite {
	namespace world_esp {
		inline void render_containers(uworld* world, aplayercontroller* controller, fvector camera_location) {
			if (!settings::containers::enable) return;

			tarray<uobject*> container_array = defines::game_statics->get_all_actors_of_class(world, defines::actor_container_class);
			for (int i = 0; i < container_array.size(); i++) {
				if (!container_array.valid(i)) continue;

				auto container = (container_actor*)container_array[i];
				if (!container || IsBadReadPtr(container, sizeof(uintptr_t))) continue;

				if (container->already_searched()) continue;

				auto world_location = container->get_actor_location();
				fvector2d screen_pos;
				if (!controller->w2s(world_location, &screen_pos) || !render::is_on_screen(screen_pos)) continue;

				double distance = defines::math->vector_distance(world_location, camera_location) * 0.01;

				auto object_name = defines::system->get_object_name(container);
				if (!object_name.valid()) continue;

				auto cmesh = container->get_static_mesh();
				if (cmesh) {
					if (settings::containers::chams && distance <= settings::world::chams_distance) {
						int stencil_ids[] = { 8, 11, 12 };
						int stencil_value = stencil_ids[settings::containers::container_stencil_id % 3];

						cmesh->set_custom_depth_stencil_value(stencil_value);
						cmesh->set_render_custom_depth(true);
					} else {
						cmesh->set_render_custom_depth(false);
					}
				}

				if (distance > settings::world::max_distance) continue;

				flinearcolor container_color = flinearcolor(1.f, 1.f, 1.f, 1.f);
				fstring container_text = L"";
				bool should_render = false;

				bool is_chest = defines::string->contains(object_name.c_str(), L"Tiered_Chest", false, false);
				bool is_ammo = defines::string->contains(object_name.c_str(), L"Tiered_Ammo", false, false);



				if (settings::containers::chest && is_chest) {
					container_color = flinearcolor(1.f, 0.84f, 0.f, 1.f);
					container_text = L"Chest";
					should_render = true;
				}
				else if (settings::containers::ammo_box && is_ammo) {
					container_color = flinearcolor(0.5f, 0.5f, 0.5f, 1.f);
					container_text = L"Ammo";
					should_render = true;
				}
				else if (settings::containers::cooler_and_food && (defines::string->contains(object_name.c_str(), L"Cooler", false, false) || 
																 defines::string->contains(object_name.c_str(), L"Food", false, false))) {
					container_color = flinearcolor(0.f, 0.5f, 1.f, 1.f);
					container_text = L"Cooler";
					should_render = true;
				}
				else if (settings::containers::safe && defines::string->contains(object_name.c_str(), L"Safe", false, false)) {
					container_color = flinearcolor(0.7f, 0.7f, 0.7f, 1.f);
					container_text = L"Safe";
					should_render = true;
				}

				if (should_render) {
					if (settings::containers::display_distance) {
						container_text = defines::string->build_string_int(container_text, L" (", (std::int32_t)distance, L"m)");
					}
					render::text(container_text, screen_pos, container_color, true, false, settings::style::text_outlined);


				}
			}
		}

		inline void render_pickups(uworld* world, aplayercontroller* controller, fvector camera_location) {
			if (!settings::pickups::enable) return;

			tarray<uobject*> pickup_array = defines::game_statics->get_all_actors_of_class(world, defines::actor_pickup_class);
			for (int i = 0; i < pickup_array.size(); i++) {
				if (!pickup_array.valid(i)) continue;

				auto pickup = (pickup_actor*)pickup_array[i];
				if (!pickup || IsBadReadPtr(pickup, sizeof(uintptr_t))) continue;

				auto world_location = pickup->get_actor_location();
				fvector2d screen_pos;
				if (!controller->w2s(world_location, &screen_pos) || !render::is_on_screen(screen_pos)) continue;

				double distance = defines::math->vector_distance(world_location, camera_location) * 0.01;

				if (distance > settings::pickups::max_distance) continue;

				auto item_def = pickup->get_item_definition();
				if (!item_def || IsBadReadPtr(item_def, sizeof(uintptr_t))) continue;

				auto tier = item_def->GetRarity();
				flinearcolor pickup_color = flinearcolor::GetColorByTier(tier);
				bool should_render = false;

				if (tier == EFortRarity::Common && settings::pickups::common) { should_render = true; }
				else if (tier == EFortRarity::Uncommon && settings::pickups::uncommon) { should_render = true; }
				else if (tier == EFortRarity::Rare && settings::pickups::rare) { should_render = true; }
				else if (tier == EFortRarity::Epic && settings::pickups::epic) { should_render = true; }
				else if (tier == EFortRarity::Legendary && settings::pickups::legendary) { should_render = true; }
				else if (tier == EFortRarity::Mythic && settings::pickups::mythic) { should_render = true; }

				if (should_render) {
					fstring pickup_text = L"Item";
					ftext display_name = item_def->display_name();
					if (display_name.c_str() && !IsBadReadPtr(display_name.c_str(), sizeof(wchar_t))) {
						pickup_text = fstring(display_name.c_str());
					}

					if (settings::pickups::display_distance) {
						pickup_text = defines::string->build_string_int(pickup_text, L" (", (std::int32_t)distance, L"m)");
					}

					render::text(pickup_text, screen_pos, pickup_color, true, false, settings::style::text_outlined);



				}
			}
		}

		inline void render_vehicles(uworld* world, aplayercontroller* controller, fvector camera_location) {
			bool do_chams = settings::miscactors::vehicle_chams;
			bool do_esp = settings::miscactors::enable && settings::miscactors::vehicles;

			if (!do_chams && !do_esp) return;

			tarray<uobject*> vehicle_array = defines::game_statics->get_all_actors_of_class(world, defines::actor_vehicle_class);
			for (int i = 0; i < vehicle_array.size(); i++) {
				if (!vehicle_array.valid(i)) continue;

				auto vehicle = (vehicle_actor*)vehicle_array[i];
				if (!vehicle || IsBadReadPtr(vehicle, sizeof(uintptr_t))) continue;

				auto world_location = vehicle->get_actor_location();
				double distance = defines::math->vector_distance(world_location, camera_location) * 0.01;

				// Vehicle Chams - unabhaengig von Vehicle ESP
				if (do_chams && distance <= settings::world::chams_distance) {
					int stencil_ids[] = { 8, 11, 12 };
					int stencil_value = stencil_ids[settings::miscactors::vehicle_stencil_id % 3];
					vehicle->set_vehicle_stencil_highlighted(vehicle, true, stencil_value);
				} else {
					vehicle->set_vehicle_stencil_highlighted(vehicle, false, 0);
				}

				// Vehicle ESP
				if (!do_esp) continue;

				fvector2d screen_pos;
				if (!controller->w2s(world_location, &screen_pos) || !render::is_on_screen(screen_pos)) continue;

				if (distance > settings::world::max_distance) continue;

				flinearcolor vehicle_color = flinearcolor(0.f, 1.f, 1.f, 1.f);
				fstring vehicle_text = vehicle->get_display_name();
				if (!vehicle_text.valid()) vehicle_text = L"Vehicle";

				if (settings::miscactors::display_distance) {
					vehicle_text = defines::string->build_string_int(vehicle_text, L" (", (std::int32_t)distance, L"m)");
				}

				render::text(vehicle_text, screen_pos, vehicle_color, true, false, settings::style::text_outlined);


			}
		}

		inline void render_misc(uworld* world, aplayercontroller* controller, fvector camera_location) {
			if (!settings::miscactors::enable) return;
			if (settings::miscactors::rifts) {
				tarray<uobject*> rift_array = defines::game_statics->get_all_actors_of_class(world, defines::actor_rift_class);
				for (int i = 0; i < rift_array.size(); i++) {
					if (!rift_array.valid(i)) continue;
					auto rift = (rift_actor*)rift_array[i];
					if (!rift || IsBadReadPtr(rift, sizeof(uintptr_t))) continue;

					auto world_location = rift->get_actor_location();
					fvector2d screen_pos;
					if (!controller->w2s(world_location, &screen_pos) || !render::is_on_screen(screen_pos)) continue;

					double distance = defines::math->vector_distance(world_location, camera_location) * 0.01;
					if (distance > settings::world::max_distance) continue;

					flinearcolor rift_color = flinearcolor(0.5f, 0.f, 1.f, 1.f);
					fstring rift_text = L"Rift";
					if (settings::miscactors::display_distance) {
						rift_text = defines::string->build_string_int(rift_text, L" (", (std::int32_t)distance, L"m)");
					}
					render::text(rift_text, screen_pos, rift_color, true, false, settings::style::text_outlined);
				}
			}

			if (settings::miscactors::flags) {
				tarray<uobject*> flag_array = defines::game_statics->get_all_actors_of_class(world, defines::actor_flag_class);
				for (int i = 0; i < flag_array.size(); i++) {
					if (!flag_array.valid(i)) continue;
					auto flag = (flag_actor*)flag_array[i];
					if (!flag || IsBadReadPtr(flag, sizeof(uintptr_t))) continue;

					auto world_location = flag->get_actor_location();
					double distance = defines::math->vector_distance(world_location, camera_location) * 0.01;
					if (distance > settings::world::max_distance) continue;

					fvector2d screen_pos;
					if (!controller->w2s(world_location, &screen_pos) || !render::is_on_screen(screen_pos)) continue;

					flinearcolor flag_color = flinearcolor(1.f, 0.f, 0.f, 1.f);
					fstring flag_text = L"Flag";
					if (settings::miscactors::display_distance) {
						flag_text = defines::string->build_string_int(flag_text, L" (", (std::int32_t)distance, L"m)");
					}
					render::text(flag_text, screen_pos, flag_color, true, false, settings::style::text_outlined);
				}
			}

			if (settings::miscactors::traps) {
				tarray<uobject*> trap_array = defines::game_statics->get_all_actors_of_class(world, defines::actor_trap_class);
				for (int i = 0; i < trap_array.size(); i++) {
					if (!trap_array.valid(i)) continue;
					auto trap = (trap_actor*)trap_array[i];
					if (!trap || IsBadReadPtr(trap, sizeof(uintptr_t))) continue;

					auto world_location = trap->get_actor_location();
					double distance = defines::math->vector_distance(world_location, camera_location) * 0.01;
					if (distance > settings::world::max_distance) continue;

					fvector2d screen_pos;
					if (!controller->w2s(world_location, &screen_pos) || !render::is_on_screen(screen_pos)) continue;

					flinearcolor trap_color = flinearcolor(1.f, 0.5f, 0.f, 1.f);
					fstring trap_text = L"Trap";
					if (settings::miscactors::display_distance) {
						trap_text = defines::string->build_string_int(trap_text, L" (", (std::int32_t)distance, L"m)");
					}
					render::text(trap_text, screen_pos, trap_color, true, false, settings::style::text_outlined);
				}
			}
		}
	}
}
