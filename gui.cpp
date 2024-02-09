#include <d3d9.h>
#include <tchar.h>
#include <stdexcept>

#include "gui.h"
#include "settings.hpp"
#include "string.hpp"

ImFont* medium;
ImFont* bold;
ImFont* tab_title;
ImFont* combo_arrow;

void gui::setup(LPDIRECT3DDEVICE9 device, HWND window)
{
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

	ImFontConfig font_config;
	font_config.PixelSnapH = false;
	font_config.OversampleH = 5;
	font_config.OversampleV = 5;
	font_config.RasterizerMultiply = 1.2f;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
		0x2DE0, 0x2DFF, // Cyrillic Extended-A
		0xA640, 0xA69F, // Cyrillic Extended-B
		0xE000, 0xE226, // icons
		0,
	};

	font_config.GlyphRanges = ranges;

	medium = io.Fonts->AddFontFromMemoryTTF(PTRootUIMedium, sizeof(PTRootUIMedium), 15.0f, &font_config, ranges);
	bold = io.Fonts->AddFontFromMemoryTTF(PTRootUIBold, sizeof(PTRootUIBold), 15.0f, &font_config, ranges);
	tab_title = io.Fonts->AddFontFromMemoryTTF(PTRootUIBold, sizeof(PTRootUIBold), 19.0f, &font_config, ranges);
	combo_arrow = io.Fonts->AddFontFromMemoryTTF(combo, sizeof(combo), 9.0f, &font_config, ranges);

	is_setup = true;
}

void gui::render()
{
	if (!is_setup || !is_open) return; // don't really need this but i like sanity checks

	// default menu pages
	static heads tab{ tab1 };
	static const char* combo_items[3] = { "Head", "Neck", "Boobs" };

	ImGui::SetNextWindowSize({ 551, 295 });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

	ImGui::Begin(_("AliWare Private (not by rea but ali)"), &is_open, ImGuiWindowFlags_NoDecoration); {
		auto draw = ImGui::GetWindowDrawList();
		auto pos = ImGui::GetWindowPos();
		auto size = ImGui::GetWindowSize();

		ImGuiStyle style = ImGui::GetStyle();
		draw->AddRectFilled(pos, ImVec2(pos.x + 60, pos.y + size.y), ImColor(1, 0, 0), style.WindowRounding, ImDrawFlags_RoundCornersLeft);
		draw->AddLine(ImVec2(pos.x + 59, pos.y + 2), ImVec2(pos.x + 59, pos.y + size.y - 2), ImColor(0.84f, 0.84f, 0.84f));
		draw->AddLine(ImVec2(pos.x + 2, pos.y + 59), ImVec2(pos.x + 59, pos.y + 59), ImColor(1.0f, 1.0f, 1.0f, 0.03f));

		ImGui::SetCursorPos({ 8, 56 });
		ImGui::BeginGroup();
		{
			if (elements::tab(_("Aimbot"), tab == tab1)) { tab = tab1; }
			if (elements::tab(_("Visuals"), tab == tab2)) { tab = tab2; }
			if (elements::tab(_("Misc"), tab == tab3)) { tab = tab3; }
		} 
		ImGui::EndGroup();

		switch (tab) { // switch case better than else if because its eww
		case tab1:
			ImGui::SetCursorPos({ 66, 10 });
			elements::begin_child(_("AliWare Private"), ImVec2(240, 300));
			{
				ImGui::Checkbox(_("Enable Aimbot"), &settings::aimbot::enable);
				ImGui::Checkbox(_("Show FOV"), &settings::aimbot::show_fov);
				ImGui::Checkbox(_("Draw Filled FOV"), &settings::aimbot::filled_fov);
				ImGui::Combo(_("Hitbox"), &settings::aimbot::hitbox, combo_items, IM_ARRAYSIZE(combo_items));
			}
			elements::end_child();

			ImGui::SetCursorPos({ 311, 10 });
			elements::begin_child(_("Click + Drag"), ImVec2(240, 240));
			{
				ImGui::SliderFloat(_("##fov"), &settings::aimbot::fov, 44.0f, 369.0f, _("fov: %.3f"));
				ImGui::SliderFloat(_("##smooth"), &settings::aimbot::smoothness, 1.0f, 14.0f, _("smoothness: %.3f"));
			}
			elements::end_child();

			//ImGui::SetCursorPos({ 66, 320 });
			//elements::begin_child(_("##section3"), ImVec2(240, 110));
			//{
			//}
			//elements::end_child();

			//ImGui::SetCursorPos({ 311, 260 });
			//elements::begin_child(_("##section4"), ImVec2(240, 170));
			//{
			//}
			//elements::end_child();
			break;
		case tab2:
			ImGui::SetCursorPos({ 66, 10 });
			elements::begin_child(_("AliWare Private"), ImVec2(240, 300));
			{
				ImGui::Checkbox(_("Enable ESP"), &settings::visuals::enable);
				ImGui::Checkbox(_("Draw Cornered Box"), &settings::visuals::cornered_box);
				ImGui::Checkbox(_("Draw Filled Box"), &settings::visuals::filled_box);
				ImGui::Checkbox(_("Box"), &settings::visuals::box);
				ImGui::Checkbox(_("Distance"), &settings::visuals::distance);
				ImGui::Checkbox(_("SnapLine"), &settings::visuals::line);
				ImGui::Checkbox(_("Platform"), &settings::visuals::platform);
				ImGui::Checkbox(_("Name"), &settings::visuals::name);

				ImGui::SliderInt(_("##thickness"), &settings::visuals::thickness, 0.633f, 11.0f, _("thickness: %d"));
			}
			elements::end_child();

			ImGui::SetCursorPos({ 311, 10 });
			elements::begin_child(_("colors"), ImVec2(240, 240));
			{
				elements::color_picker(_("box"), settings::visuals::box_color, settings::visuals::box_color[3]); // this is gay
				elements::color_picker(_("filled box"), settings::visuals::filled_box_color, settings::visuals::filled_box_color[3]); // this is gay
				elements::color_picker(_("line"), settings::visuals::line_color, settings::visuals::line_color[3]); // this is gay
				elements::color_picker(_("invisble box"), settings::visuals::inv_box_color, settings::visuals::inv_box_color[3]); // this is gay
				elements::color_picker(_("invisble filled box"), settings::visuals::inv_filled_box_color, settings::visuals::inv_filled_box_color[3]); // this is gay
				elements::color_picker(_("invisble line"), settings::visuals::inv_line_color, settings::visuals::inv_line_color[3]); // this is gay
			}
			elements::end_child();

			//ImGui::SetCursorPos({ 66, 320 });
			//elements::begin_child(_("##section3"), ImVec2(240, 110));
			//{
			//}
			//elements::end_child();

			//ImGui::SetCursorPos({ 311, 260 });
			//elements::begin_child(_("##section4"), ImVec2(240, 170));
			//{
			//}
			//elements::end_child();
			break;
		case tab3:
			ImGui::SetCursorPos({ 66, 10 });
			elements::begin_child(_("AliWare Private"), ImVec2(240, 300));
			{
				ImGui::Checkbox(_("enable misc"), &settings::misc::enable);
				ImGui::Checkbox(_("no recoil"), &settings::misc::no_recoil);
			}
			elements::end_child();
			break;
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}
