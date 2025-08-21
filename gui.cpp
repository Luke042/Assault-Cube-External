#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <chrono>
#include "gui.h"
#include "settings.h"
#include "font.hpp"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

//my code is shit i know but it works so idgaf


static int selected_font = 0;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

long __stdcall WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED)
		{
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			gui::ResetDevice();
		}

	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU)
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter); // set click points
	}return 0;

	case WM_MOUSEMOVE: {
		if (values::show_menu) {
			if (wideParameter == MK_LBUTTON) {
				const auto points = MAKEPOINTS(longParameter);
				auto rect = ::RECT{ };

				GetWindowRect(gui::window, &rect);

				rect.left += points.x - gui::position.x;
				rect.top += points.y - gui::position.y;

				if (gui::position.x >= 0 &&
					gui::position.x <= gui::WIDTH &&
					gui::position.y >= 0 && gui::position.y <= 19)
					SetWindowPos(
						gui::window,
						HWND_TOPMOST,
						rect.left,
						rect.top,
						0, 0,
						SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
					);
			}
		}
	} return 0;
	
	}

	return DefWindowProc(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.hbrBackground = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = "class001";
	windowClass.hIconSm = 0;

	RegisterClassEx(&windowClass);

	window = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_APPWINDOW,
		windowClass.lpszClassName,
		windowName,
		WS_POPUP,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr
	);

	SetLayeredWindowAttributes(window, RGB(0, 0, 0), 0, LWA_COLORKEY);
	ShowWindow(window, SW_SHOWNOACTIVATE);
	UpdateWindow(window);
}

void SetClickThrough(bool enable)
{
	LONG exStyle = GetWindowLong(gui::window, GWL_EXSTYLE);
	if (enable)
		SetWindowLong(gui::window, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST);
	else
		SetWindowLong(gui::window, GWL_EXSTYLE, (exStyle & ~WS_EX_TRANSPARENT) | WS_EX_LAYERED | WS_EX_TOPMOST);
}

void gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d) return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));
	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;

	presentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;

	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;;


	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}

void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT)
		{
			isRunning = !isRunning;
			return;
		}
	}
	// Start the Dear ImGui frame
	SetClickThrough(!values::show_menu);
	ImGuiIO& io = ImGui::GetIO();

	if (selected_font == 0) {
		io.Fonts->AddFontFromMemoryTTF(BurBank, sizeof(BurBank), 13.5f);
	}
	else if (selected_font == 1) {
		io.Fonts->AddFontFromMemoryTTF(Roboto, sizeof(Roboto), 13.5f);
	}
	
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();

}
/*
void overlay::RenderText() noexcept {
	values::fps = ImGui::GetIO().Framerate;

	//set position
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowBgAlpha(0.3f);

	ImGui::Begin("FPS Overlay", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoInputs
	);

	ImGui::Text("FPS: %.1f", values::fps);
	ImGui::End();
}

void overlay::RenderFPS_NoWindow() noexcept {
	values::fps = ImGui::GetIO().Framerate;

	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
	draw_list->AddText(ImVec2(10, 10), IM_COL32(0, 255, 0, 255),
		("FPS: " + std::to_string(static_cast<int>(values::fps))).c_str());
}
*/
void gui::Render() noexcept
{

	ImGuiIO& io = ImGui::GetIO();
	if (selected_font == 0) {
		io.Fonts->AddFontFromMemoryTTF(BurBank, sizeof(BurBank), 13.5f);
	}

	else if (selected_font == 1) {
		io.Fonts->AddFontFromMemoryTTF(Roboto, sizeof(Roboto), 13.5f);
	}
	//menu styling
	ImGuiStyle& style = ImGui::GetStyle();
	style.TabRounding = 6.0f;
	style.FramePadding = ImVec2(6.0f, 5.0f);
	style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
	style.ChildRounding = 4.0f;
	style.ScrollbarRounding = 6.0f;
	style.WindowRounding = 6;
	style.ScrollbarRounding = 0;

	style.Colors[ImGuiCol_ResizeGrip] = ImColor(0, 0, 0, 0);
	style.Colors[ImGuiCol_ResizeGripActive] = ImColor(0, 0, 0, 0);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImColor(0, 0, 0, 0);

	style.Colors[ImGuiCol_Button] = ImColor(24, 24, 24);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(26, 26, 26);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(26, 26, 26);

	style.Colors[ImGuiCol_CheckMark] = ImColor(0, 189, 0, 255);

	style.Colors[ImGuiCol_FrameBg] = ImColor(24, 24, 24);
	style.Colors[ImGuiCol_FrameBgActive] = ImColor(26, 26, 26);
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor(26, 26, 26);

	style.Colors[ImGuiCol_SliderGrab] = ImColor(0, 189, 0, 255);
	style.Colors[ImGuiCol_SliderGrabActive] = ImColor(0, 189, 0, 255);

	style.Colors[ImGuiCol_Tab] = ImColor(24, 24, 24);
	style.Colors[ImGuiCol_TabActive] = ImColor(0, 189, 0, 255);
	style.Colors[ImGuiCol_TabHovered] = ImColor(0, 189, 0, 255);

	style.Colors[ImGuiCol_TitleBgActive] = ImColor(24, 24, 24);
	

	if (values::show_menu) {
		auto frame_start = std::chrono::high_resolution_clock::now();
		ImVec2 menu_pos;
		ImGui::SetNextWindowPos(menu_pos);
		if (ImGui::Begin("Assault Cube External")) {
			ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
			ImGui::Begin(
				"Assault Cube External",
				&isRunning,
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoMove
			);
			
			if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				menu_pos.x += ImGui::GetIO().MouseDelta.x;
				menu_pos.y += ImGui::GetIO().MouseDelta.y;
			}
			
			if (ImGui::BeginTabBar("tabs"))
			{
				if (ImGui::BeginTabItem("Aim"))
				{
					ImGui::Checkbox("Aimbot", &aim::aimbot_toggle);
					if (aim::aimbot_toggle) {
						aim::silent_aim = false;
						aim::perfect_aim = false;
						ImGui::Text("Aimbot Key:");
						ImGui::InputInt(" ", &aim::aim_key);
						ImGui::Text("Really Shit Buggy Smoothening");
						ImGui::SliderFloat("         ", &aim::aimbot_smoothening, 0.1f, 1);
						ImGui::Checkbox("Team Check", &aim::aimbot_team_check);
						ImGui::Checkbox("Visible Check", &aim::aimbot_visable_check);
						ImGui::NewLine();
					}
					ImGui::Checkbox("Perfect Aim", &aim::perfect_aim);
					if (aim::perfect_aim) {
						aim::aimbot_toggle = false;
						aim::silent_aim = false;
						ImGui::Checkbox("Team Check   ", &aim::aimbot_team_check);
						ImGui::Checkbox("Visible Check ", &aim::aimbot_visable_check);
						ImGui::NewLine();
					}
					/*
					else if (!aim::aimbot_toggle) {
						aim::perfect_aim = false;
						aim::bezier_aim = false;
					}
					*/
					ImGui::Checkbox("Bezier Curve Aimbot", &aim::bezier_aim);
					if (aim::bezier_aim) {
						ImGui::Text("Aimbot Key:        ");
						ImGui::InputInt("  ", &aim::aim_key);
						ImGui::Text("Smoothening    ");
						ImGui::SliderFloat("          ", &values::bezier_speed, 0.1f, 1);
						ImGui::Checkbox("Team Check         ", &aim::bezier_team_check);
						ImGui::Checkbox("Visible Check   ", &aim::bezier_visable_check);
					}				

					ImGui::Checkbox("Silent Aim", &aim::silent_aim);
					if (aim::silent_aim) {
						aim::aimbot_toggle = false;
						aim::bezier_aim = false;
						aim::perfect_aim = false;
						ImGui::Checkbox("Team Check ", &aim::silent_team_check);
						ImGui::Text("Silent Key:");
						ImGui::InputInt("      ", &aim::silent_key);
					}
					else if (!aim::silent_aim) {
						aim::silent_team_check = false;
						aim::silent_visable_check = false;
					}

					ImGui::Checkbox("Trigger bot", &aim::triggerbot);
					if (aim::triggerbot) {
						ImGui::Checkbox("Team Check       ", &aim::trigger_team_check);
						ImGui::Checkbox("Visible Check         ", &aim::trigger_visable_check);
						ImGui::Text("Trigger bot key:");
						ImGui::InputInt("    ", &aim::trigger_key);
					}

					ImGui::NewLine();
					ImGui::EndTabItem();
				}

				/*
				if (ImGui::BeginTabItem("Visuals"))
				{
					ImGui::Checkbox("ESP(Broken/Fix Later)", &visuals::esp_toggle);
					ImGui::EndTabItem();
				}*/

				if (ImGui::BeginTabItem("Exploits"))
				{
					ImGui::Checkbox("Fast Fire", &exploits::fast_shoot);
					if (exploits::fast_shoot) {
						ImGui::Text("Turn down your volume :)");
					}
					ImGui::Checkbox("Infinite Jump", &exploits::inf_jump);
				//	ImGui::Checkbox("Kill Aura", &exploits::kill_aura);
					ImGui::Checkbox("Static Ammo", &exploits::static_ammo);
					ImGui::Checkbox("Static Health", &exploits::static_health);
					if (exploits::static_health) {
						ImGui::Text("(Static health does not work in multiplayer\nand applys to everyone)");
					}
					ImGui::Checkbox("Anti-Recoil", &exploits::anti_recoil);
					ImGui::Checkbox("FOV Slider", &exploits::fov);
					if (exploits::fov) {
						ImGui::SliderFloat("Enter FOV value", &values::fov_value, 1.0f, 170.0f);
					}
					ImGui::Checkbox("Fast Grenades", &exploits::fast_grenade);
					ImGui::Checkbox("Overwrite ammo", &exploits::overwrite_ammo);
					ImGui::Checkbox("Overwrite health", &exploits::overwrite_health);
					ImGui::Checkbox("Fly", &exploits::fly);
					if (exploits::fly) {
						ImGui::SliderFloat("fly height", &values::fly_height, 0.0f, 100.0f);
						ImGui::Text("Controls:\nSpace to fly\nShift to unstuck");
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Way points"))
				{
					if (ImGui::Button("Save position")) {
						values::save_position = true;

					}
					if (ImGui::Button("Load position")) {
						values::load_position = true;
					}
					ImGui::Text("First Way point:");
					ImGui::Text("X: %.2f", values::wp_posX);
					ImGui::Text("Y: %.2f", values::wp_posY);
					ImGui::Text("Z: %.2f", values::wp_posZ);
					ImGui::NewLine();
					if (ImGui::Button("Save position 2")) {
						values::save_position2 = true;

					}
					if (ImGui::Button("Load position 2")) {
						values::load_position2 = true;
					}
					ImGui::Text("Second Way point:");
					ImGui::Text("X: %.2f", values::wp_posX2);
					ImGui::Text("Y: %.2f", values::wp_posY2);
					ImGui::Text("Z: %.2f", values::wp_posZ2);
					ImGui::EndTabItem();

				}

				if (ImGui::BeginTabItem("Misc"))
				{
					values::fps = ImGui::GetIO().Framerate;
					ImGui::Text("FPS: %.0f", values::fps);

					ImGui::Button("Save config");
					ImGui::Button("load config");

					const char* fonts[2] = { "BurBank", "Roboto" };
					ImGui::NewLine();
					ImGui::Checkbox("Low End Mode (Lower CPU Usage)", &values::low_end);
					if (ImGui::Button("Emergency Close Key: F8")) {
						exit(0);
					}
					ImGui::NewLine();
					ImGui::Text("Select Font: ");
					ImGui::Combo(" ", &selected_font, fonts, IM_ARRAYSIZE(fonts));

					if (selected_font == 1) {
						ImGuiIO& io = ImGui::GetIO();
						io.Fonts->AddFontFromMemoryTTF(Roboto, sizeof(Roboto), 13.5f);
						//ImGui_ImplDX9_NewFrame();
					//	ImGui_ImplWin32_NewFrame();
					}

					ImVec2 windowSize = ImGui::GetWindowSize();
					ImVec2 textSize = ImGui::CalcTextSize("Bottom Right");
					ImGui::SetCursorPos(ImVec2(windowSize.x - textSize.x - 20.0f, windowSize.y - textSize.y - 35.0f));
					ImGui::Text("Credits:\nluke.042\non discord");
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
				

			}
			ImGui::End();

		}

	
		ImGui::End();
		}

	}
	

