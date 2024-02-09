#pragma once

#include <dwmapi.h>

#include "gui.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

IDirect3D9Ex* p_object = NULL;
IDirect3DDevice9Ex* p_device = NULL;
D3DPRESENT_PARAMETERS p_params = { NULL };
HWND game_wnd = NULL;
RECT game_rect = { NULL };
HWND my_wnd = NULL;
MSG messager = { NULL };

HRESULT directx_init()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object)))
		exit(3);
	ZeroMemory(&p_params, sizeof(p_params));
	p_params.Windowed = TRUE;
	p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_params.hDeviceWindow = my_wnd;
	p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_params.BackBufferWidth = settings::width;
	p_params.BackBufferHeight = settings::height;
	p_params.EnableAutoDepthStencil = TRUE;
	p_params.AutoDepthStencilFormat = D3DFMT_D16;
	p_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, my_wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_params, 0, &p_device)))
	{
		p_object->Release();
		exit(4);
	}

	gui::setup(p_device, my_wnd);

	p_object->Release();
	return S_OK;
}
void create_overlay()
{
	WNDCLASSEXA wcsex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		nullptr,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		(_("performance ov")),
		LoadIcon(nullptr, IDI_APPLICATION)
	};
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	RegisterClassExA(&wcsex);
	my_wnd = CreateWindowExA(NULL, (_("performance ov")), (_("performance ov")), WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, NULL, NULL, wcsex.hInstance, NULL);
	SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(my_wnd, &margin);
	ShowWindow(my_wnd, SW_SHOW);
	SetWindowPos(my_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	UpdateWindow(my_wnd);
}
void cleanup_d3d()
{
	if (p_device != NULL)
	{
		p_device->EndScene();
		p_device->Release();
	}
	if (p_object != NULL)
	{
		p_object->Release();
	}
}
void draw_cornered_box(int x, int y, int w, int h, const ImColor color, int thickness)
{
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, thickness);
}
void draw_filled_rect(int x, int y, int w, int h, const ImColor color)
{
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}
void draw_rect(int x, int y, int w, int h, const ImColor color, int thickness)
{
	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0, thickness);
}
void draw_line(Vector3 target, ImColor color)
{
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(settings::width / 2, settings::height), ImVec2(target.x, target.y), color, 0.1f);
}
void draw_text(Vector3 target, const char* text, ImColor color)
{
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(target.x, target.y), color, text);
}

void aimbot(uintptr_t target_pawn, uintptr_t closest_distance)
{
	if (!target_pawn) return;
	uintptr_t mesh = read<uintptr_t>(target_pawn + 0x310);
	if (!mesh)
	{
		closest_distance = FLT_MAX;
		target_pawn = NULL;
	}

	int hitboxid = 106;
	switch (settings::aimbot::hitbox)
	{
	case 0:
		hitboxid = 106; // head
		break;
	case 1:
		hitboxid = 67; // neck
		break;
	case 2:
		hitboxid = 7; // chest
		break;
	}

	Vector3 head3d = GetBoneWithRotation(mesh, hitboxid);
	Vector3 head2d = ProjectWorldToScreen(head3d);
	double dx = head2d.x - (settings::width / 2);
	double dy = head2d.y - (settings::height / 2);
	float dist = sqrtf(dx * dx + dy * dy);
	if (dist > settings::aimbot::fov or head2d.x == 0 or head2d.y == 0)
	{
		closest_distance = FLT_MAX;
		target_pawn = NULL;
	}
	Vector2 screen_center = { (double)settings::width / 2, (double)settings::height / 2 };
	Vector2 target{};
	if (IsVisible(mesh))
	{
		if (head2d.x != 0)
		{
			if (head2d.x > screen_center.x)
			{
				target.x = -(screen_center.x - head2d.x);
target.x /= settings::aimbot::smoothness;
if (target.x + screen_center.x > screen_center.x * 2) target.x = 0;
			}
			if (head2d.x < screen_center.x)
			{
				target.x = head2d.x - screen_center.x;
				target.x /= settings::aimbot::smoothness;
				if (target.x + screen_center.x < 0) target.x = 0;
			}
		}
		if (head2d.y != 0)
		{
			if (head2d.y > screen_center.y)
			{
				target.y = -(screen_center.y - head2d.y);
				target.y /= settings::aimbot::smoothness;
				if (target.y + screen_center.y > screen_center.y * 2) target.y = 0;
			}
			if (head2d.y < screen_center.y)
			{
				target.y = head2d.y - screen_center.y;
				target.y /= settings::aimbot::smoothness;
				if (target.y + screen_center.y < 0) target.y = 0;
			}
		}
		Input::MoveMouse(target.x, target.y);
	}
}

void game_loop()
{
	pointer::uworld = read<uintptr_t>(driver::image_base + 0xEEDBD10);
	//pointer::gobjects = read<uintptr_t>(image_base + 0xEA52F68);
	pointer::game_instance = read<uintptr_t>(pointer::uworld + 0x1B8);
	pointer::local_players = read<uintptr_t>(read<uintptr_t>(pointer::game_instance + 0x38));
	pointer::player_controller = read<uintptr_t>(pointer::local_players + 0x30);
	pointer::local_pawn = read<uintptr_t>(pointer::player_controller + 0x330);
	pointer::root_component = read<uintptr_t>(pointer::local_pawn + 0x190);
	pointer::player_state = read<uintptr_t>(pointer::local_pawn + 0x2A8);
	pointer::game_state = read<uintptr_t>(pointer::uworld + 0x158);
	pointer::player_array = read<uintptr_t>(pointer::game_state + 0x2A0);
	pointer::local_relative_location = read<Vector3>(pointer::root_component + 0x11c);
	pointer::closest_distance = FLT_MAX;
	pointer::closest_pawn = NULL;

	int count = read<int>(pointer::game_state + (0x2A0 + sizeof(uintptr_t)));
	for (int i = 0; i < count; i++)
	{
		uintptr_t current_player_state = read<uintptr_t>(pointer::player_array + (i * sizeof(uintptr_t)));
		if (!current_player_state) continue;
		int my_team_id = read<int>(pointer::player_state + 0x10A0);
		int current_actor_team_id = read<int>(current_player_state + 0x10A0);
		if (my_team_id == current_actor_team_id) continue;
		uintptr_t current_local_pawn_private = read<uintptr_t>(current_player_state + 0x300);
		if (!current_local_pawn_private) continue;
		if (current_local_pawn_private == pointer::local_pawn) continue;
		uintptr_t current_mesh = read<uintptr_t>(current_local_pawn_private + 0x310);
		if (!current_mesh) continue;
		Vector3 head3d = GetBoneWithRotation(current_mesh, 106);
		Vector3 head2d = ProjectWorldToScreen(head3d);
		Vector3 bottom3d = GetBoneWithRotation(current_mesh, 0);
		Vector3 bottom2d = ProjectWorldToScreen(bottom3d);
		float corner_height = abs(head2d.y - bottom2d.y);
		float corner_width = corner_height * 0.80;
		double dx = head2d.x - (settings::width / 2);
		double dy = head2d.y - (settings::height / 2);
		float dist = sqrtf(dx * dx + dy * dy);

		float _distance = pointer::local_relative_location.distance(bottom3d) / 100.0f;

		if (dist < settings::aimbot::fov && dist < pointer::closest_distance)
		{
			pointer::closest_distance = dist;
			pointer::closest_pawn = current_local_pawn_private;
		}
		if (settings::visuals::enable)
		{
			int bottom_offset = 7, top_offset = 17;
			ImColor color = ImColor(255, 255, 255, 255);
			static int size[2] = { 200, 200 };
			static int pos[2] = {100, 100};

			if (settings::visuals::name)
			{
				auto name = name_dec(current_player_state);
				ImVec2 DistanceTextSize2 = ImGui::CalcTextSize(name.c_str());
				draw_text(Vector3(head2d.x - DistanceTextSize2.x / 2, (head2d.y - top_offset), 0), name.c_str(), color);
				top_offset += 15;
			}
			if (settings::visuals::platform)
			{
				DWORD_PTR test_platform = read<DWORD_PTR>(current_player_state + 0x430);
				wchar_t platform[64];
				driver::read_physical_memory((PVOID)test_platform, &platform, sizeof(platform));
				std::wstring balls_sus(platform);
				std::string str_platform(balls_sus.begin(), balls_sus.end());
				ImVec2 DistanceTextSize2 = ImGui::CalcTextSize(str_platform.c_str());

				if (str_platform.find(_("XBL")) != std::string::npos || str_platform.find(_("XSX")) != std::string::npos) {
					draw_text(Vector3(head2d.x - DistanceTextSize2.x / 2, (head2d.y - top_offset), 0), _("XBOX"), color);
				}
				else if (str_platform.find(_("PSN")) != std::string::npos || str_platform.find(_("PS5")) != std::string::npos) {
					draw_text(Vector3(head2d.x - DistanceTextSize2.x / 2, (head2d.y - top_offset), 0), _("PSN"), color);
				}
				else if (str_platform.find(_("WIN")) != std::string::npos) {
					draw_text(Vector3(head2d.x - DistanceTextSize2.x / 2, (head2d.y - top_offset), 0), _("PC"), color);
				}
				else if (str_platform.find(_("SWH")) != std::string::npos) {
					draw_text(Vector3(head2d.x - DistanceTextSize2.x / 2, (head2d.y - top_offset), 0), _("SIW"), color);
				}
				top_offset += 15;
			}
			if (settings::visuals::box)
			{
				color = ImColor(settings::visuals::box_color[0], settings::visuals::box_color[1], settings::visuals::box_color[2], settings::visuals::box_color[3]);
				draw_rect(head2d.x - (corner_width / 2), head2d.y, corner_width, corner_height, color, settings::visuals::thickness);
			}
			if (settings::visuals::cornered_box)
			{
				if (IsVisible(current_mesh))
				{
					color = ImColor(settings::visuals::box_color[0], settings::visuals::box_color[1], settings::visuals::box_color[2], settings::visuals::box_color[3]);
					draw_cornered_box(head2d.x - (corner_width / 2), head2d.y, corner_width, corner_height, color, settings::visuals::thickness);
					if (settings::visuals::filled_box)
					{
						color = ImColor(settings::visuals::filled_box_color[0], settings::visuals::filled_box_color[1], settings::visuals::filled_box_color[2], settings::visuals::filled_box_color[3]);
						draw_filled_rect(head2d.x - (corner_width / 2), head2d.y, corner_width, corner_height, color);
					}
				}
				else
				{
					color = ImColor(settings::visuals::inv_box_color[0], settings::visuals::inv_box_color[1], settings::visuals::inv_box_color[2], settings::visuals::inv_box_color[3]);
					draw_cornered_box(head2d.x - (corner_width / 2), head2d.y, corner_width, corner_height, color, settings::visuals::thickness);
					if (settings::visuals::filled_box)
					{
						color = ImColor(settings::visuals::inv_filled_box_color[0], settings::visuals::inv_filled_box_color[1], settings::visuals::inv_filled_box_color[2], settings::visuals::inv_filled_box_color[3]);
						draw_filled_rect(head2d.x - (corner_width / 2), head2d.y, corner_width, corner_height, color);
					}
				}
			}
			if (settings::visuals::line)
			{
				if (IsVisible(current_mesh))
				{
					color = ImColor(settings::visuals::line_color[0], settings::visuals::line_color[1], settings::visuals::line_color[2], settings::visuals::line_color[3]);
					draw_line(bottom2d, color);
				}
				else
				{
					color = ImColor(settings::visuals::inv_line_color[0], settings::visuals::inv_line_color[1], settings::visuals::inv_line_color[2], settings::visuals::inv_line_color[3]);
					draw_line(bottom2d, color);
				}
			}
			if (settings::visuals::distance) {
				std::string distance_text = std::to_string(_distance);
				distance_text.append(_("m"));
				Vector3 text_pos(head2d.x + corner_width, head2d.y - top_offset, 0);
				ImU32 distance_color = ImColor(255, 255, 255);
				draw_text(text_pos, distance_text.c_str(), distance_color);

			}

		}
	}

	/*uintptr_t item_levels = read<uintptr_t>(pointer::uworld + 0x170);
	for (int i = 0; i < read<DWORD>(pointer::uworld + (0x170 + sizeof(PVOID))); ++i) {
		uintptr_t item_level = read<uintptr_t>(item_levels + (i * sizeof(uintptr_t)));
		for (int i = 0; i < read<DWORD>(item_level + (0x98 + sizeof(PVOID))); ++i) {
			uintptr_t ItemsPawns = read<uintptr_t>(item_level + 0x98);
			uintptr_t CurrentItemPawn = read<uintptr_t>(ItemsPawns + (i * sizeof(uintptr_t)));
			uintptr_t ItemRootComponent = read<uintptr_t>(CurrentItemPawn + 0x188);
			Vector3 ItemPosition = read<Vector3>(ItemRootComponent + 0x128);


		}
	}*/

	if (settings::aimbot::enable)
	{
		if (GetAsyncKeyState(VK_RBUTTON))
		{
			aimbot(pointer::closest_pawn, pointer::closest_distance);
		}
	}
	if (settings::misc::enable)
	{
		if (settings::misc::no_recoil)
		{
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				write<float>(pointer::player_controller + 0x64, -1);
			}
		}
	}
}

WPARAM render_loop()
{
	static RECT old_rc;
	ZeroMemory(&messager, sizeof(MSG));
	while (messager.message != WM_QUIT)
	{
		if (PeekMessage(&messager, my_wnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&messager);
			DispatchMessage(&messager);
		}
		HWND hwnd_active = GetForegroundWindow();
		if (GetAsyncKeyState(VK_END) & 1)
			exit(8);
		if (hwnd_active == game_wnd)
		{
			HWND hwnd_test = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(my_wnd, hwnd_test, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		RECT rc;
		POINT xy;
		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(game_wnd, &rc);
		ClientToScreen(game_wnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;
		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = game_wnd;
		io.DeltaTime = 1.0f / 60.0f;
		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;
		if (GetAsyncKeyState(0x1))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
		{
			io.MouseDown[0] = false;
		}
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;
			settings::width = rc.right;
			settings::height = rc.bottom;
			p_params.BackBufferWidth = settings::width;
			p_params.BackBufferHeight = settings::height;
			SetWindowPos(my_wnd, (HWND)0, xy.x, xy.y, settings::width, settings::height, SWP_NOREDRAW);
			p_device->Reset(&p_params);
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		game_loop();

		if (settings::aimbot::show_fov)
		{
			ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::aimbot::fov, ImColor(255, 255, 255, 255), 100, 1.0f);
			if (settings::aimbot::filled_fov) ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::aimbot::fov, ImColor(255, 255, 255, 20), 100);
		}

		if (GetAsyncKeyState(VK_INSERT) & 1) gui::is_open = !gui::is_open;

		gui::render();

		ImGui::EndFrame();

		p_device->SetRenderState(D3DRS_ZENABLE, false);
		p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		p_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		if (p_device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_device->EndScene();
		}
		HRESULT result = p_device->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_device->Reset(&p_params);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	cleanup_d3d();
	DestroyWindow(my_wnd);
	return messager.wParam;
}