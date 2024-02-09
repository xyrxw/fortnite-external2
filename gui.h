#pragma once
#include <d3d9.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "imgui/byte_array.h"
#include "imgui/elements.h"

namespace gui
{
	inline bool is_open = true;
	inline bool is_setup = false;

	void setup(LPDIRECT3DDEVICE9 device, HWND window);
	void render();

	enum heads {
		tab1, tab2, tab3
	};

	enum sub_heads {
		subtab1, subtab2
	};
}