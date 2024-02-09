#pragma once
namespace settings
{
	inline int width = GetSystemMetrics(SM_CXSCREEN);
	inline int height = GetSystemMetrics(SM_CYSCREEN);
	namespace aimbot
	{
		inline bool enable = true;
		inline bool show_fov = true;
		inline bool filled_fov = true;
		inline float fov = 160.0f;
		inline float smoothness = 5.0f;
		inline int hitbox = 0;
	}
	namespace visuals
	{
		inline bool enable = true;
		inline bool box = true;
		inline bool cornered_box = false;
		inline bool filled_box = false;
		inline bool line = false;
		inline bool distance = false;
		inline bool name = false;
		inline bool platform = true;

		inline float box_color[4] = { 1.0f, 0.5f, 0.0f, 1.0f };
		inline float filled_box_color[4] = { 0.0f, 0.0f, 0.0f, 0.2f };
		inline float line_color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	

		inline float inv_box_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		inline float inv_filled_box_color[4] = { 0.0f, 0.0f, 0.0f, 0.2f };
		inline float inv_line_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

		inline int thickness = 3;
	}
	namespace misc
	{
		inline bool enable = false;
		inline bool no_recoil = false;
	}
}