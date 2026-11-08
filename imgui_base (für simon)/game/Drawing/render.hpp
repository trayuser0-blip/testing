#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../../imgui simon/menu/headers/fonts.h"
#include "../../imgui simon/menu/data/fonts.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"

namespace render {
	inline bool show_menu = false;
	inline bool draw_cursor = true;
	inline bool show_menu_old = false;
	inline bool outline = false;
	inline float outlinethickness = 1.f;
	inline float linethickness = 2.f;
	inline float esp_stroke_thickness = 1.f;
	inline int hovered_headers = 0;
	inline uworld* world;
	inline aplayercontroller* controller;
	inline ucanvas* canvas;
	inline uobject* font;
	inline fvector2d screen_center, screen_size;

	inline ImU32 to_imu32(const flinearcolor& c) {
		return ImGui::GetColorU32(ImVec4((float)c.r, (float)c.g, (float)c.b, (float)c.a));
	}

	inline void wchar_to_buf(const wchar_t* wstr, int len, char* out, int out_size) {
		if (!wstr || len <= 0) { out[0] = '\0'; return; }
		int res = WideCharToMultiByte(CP_UTF8, 0, wstr, len, out, out_size - 1, nullptr, nullptr);
		if (res > 0) out[res] = '\0';
		else out[0] = '\0';
	}

	inline bool is_insert_clicked() {
		static bool insert_down_already = false;
		bool insert_down = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0 || (GetAsyncKeyState(VK_F2) & 0x8000) != 0;
		if (insert_down) {
			if (insert_down_already) return false;
			insert_down_already = true;
			return true;
		}
		insert_down_already = false;
		return false;
	}

	inline bool in_circle(int cx, int cy, int r, int x, int y) {
		int dist = (x - cx) * (x - cx) + (y - cy) * (y - cy);
		return dist <= r * r;
	}

	inline bool is_on_screen(fvector2d position) {
		return position.x > 0 && position.x < screen_size.x && position.y > 0 && position.y < screen_size.y;
	}

	inline fvector2d text_size(fstring content) {
		if (!canvas || !defines::font) return fvector2d(0, 0);
		return canvas->k2_text_size(defines::font, content, fvector2d(1.0, 1.0));
	}

	inline void text(fstring content, fvector2d position, flinearcolor render_color, bool center_x, bool center_y, bool outlined) {
		if (!canvas || !defines::font) return;
		fvector2d pos = position;
		if (center_x || center_y) {
			fvector2d size = text_size(content);
			if (center_x) pos.x -= size.x / 2.0;
			if (center_y) pos.y -= size.y / 2.0;
		}
		canvas->k2_draw_text(defines::font, content, pos, fvector2d(1.0, 1.0), render_color, 1.0f, flinearcolor(0, 0, 0, 0), fvector2d(), false, false, outlined, flinearcolor(0, 0, 0, 1));
	}


	inline void line(fvector2d a, fvector2d b, flinearcolor render_color, float thickness) {
		if (!canvas) return;
		canvas->k2_draw_line(a, b, thickness, render_color);
	}

	inline void line_with_outline(fvector2d a, fvector2d b, flinearcolor render_color, float thickness_scale) {
		if (!canvas) return;
		const float t = esp_stroke_thickness * thickness_scale;
		if (outline) {
			canvas->k2_draw_line(a, b, t + outlinethickness, flinearcolor(0, 0, 0, 1));
		}
		canvas->k2_draw_line(a, b, t, render_color);
	}

	inline void circle(fvector2d position, int radius, int segments, flinearcolor color) {
		if (!canvas) return;
		// Circle using lines since k2_draw_circle might not be available
		float step = 3.1415926f * 2.0f / segments;
		for (float i = 0; i < 3.1415926f * 2.0f; i += step) {
			fvector2d a = fvector2d(position.x + cosf(i) * radius, position.y + sinf(i) * radius);
			fvector2d b = fvector2d(position.x + cosf(i + step) * radius, position.y + sinf(i + step) * radius);
			canvas->k2_draw_line(a, b, linethickness, color);
		}
	}

	inline void filled_box(fvector2d position, fvector2d size, flinearcolor render_color) {
		ImDrawList* dl = ImGui::GetForegroundDrawList();
		if (!dl) return;
		dl->AddRectFilled(
			ImVec2((float)position.x, (float)position.y),
			ImVec2((float)(position.x + size.x), (float)(position.y + size.y)),
			to_imu32(render_color)
		);
	}

	inline void mouse_cursor() {}
}
