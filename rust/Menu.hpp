#include "../Renderer/Drawer.h"
#include "../Renderer/ImGui/imgui_impl_dx11.h"
#include "../Vars.h"
#include <d3d11.h>
#include "../xorstr.hpp"

#include <Windows.h>


#pragma comment(lib, "d3d11.lib")
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace menu {
	static ID3D11Device* g_pd3dDevice = NULL;
	static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	static IDXGISwapChain* g_pSwapChain = NULL;
	static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

	void CreateRenderTarget()
	{
		DXGI_SWAP_CHAIN_DESC sd;
		g_pSwapChain->GetDesc(&sd);

		ID3D11Texture2D* pBackBuffer;
		D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
		ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
		render_target_view_desc.Format = sd.BufferDesc.Format;
		render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &g_mainRenderTargetView);
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		pBackBuffer->Release();
	}

	void CleanupRenderTarget()
	{
		if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
	}

	HRESULT CreateDeviceD3D(HWND hWnd)
	{
		DXGI_SWAP_CHAIN_DESC sd;
		{
			ZeroMemory(&sd, sizeof(sd));
			sd.BufferCount = 2;
			sd.BufferDesc.Width = 0;
			sd.BufferDesc.Height = 0;
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.RefreshRate.Numerator = 0;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.OutputWindow = hWnd;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = TRUE;
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		}

		UINT createDeviceFlags = DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[1] = { D3D_FEATURE_LEVEL_11_0, };
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 1, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
			return E_FAIL;

		CreateRenderTarget();

		return S_OK;
	}

	void CleanupDeviceD3D()
	{
		CleanupRenderTarget();
		if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
		if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	}

	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				ImGui_ImplDX11_InvalidateDeviceObjects();
				CleanupRenderTarget();
				g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
				ImGui_ImplDX11_CreateDeviceObjects();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	namespace DisplayInformation
	{
		float FPSLock = 144.f;
		bool bMenu = true;
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		bool WindowStatus = true;
		Vector2 Resolution;

		Vector2 GetDisplayResolution()
		{
			RECT Desktop;
			GetWindowRect(GetDesktopWindow(), &Desktop);
			Resolution.x = Desktop.right;
			Resolution.y = Desktop.bottom;
			return Resolution;
		}
	}

	void ChangeClickAbility(bool canclick, HWND CHWND)
	{
		long style = GetWindowLong(CHWND, GWL_EXSTYLE);
		if (canclick)
		{
			style &= ~WS_EX_LAYERED;
			SetWindowLong(CHWND, GWL_EXSTYLE, style);
			SetForegroundWindow(CHWND);
		}
		else
		{
			style |= WS_EX_LAYERED;
			SetWindowLong(CHWND, GWL_EXSTYLE, style);
		}
	}

	void BuildMenuMain()



	{


		auto& style = ImGui::GetStyle();
		style.WindowPadding = { 10.f, 10.f };
		style.PopupRounding = 0.f;
		style.FramePadding = { 8.f, 4.f };
		style.ItemSpacing = { 10.f, 8.f };
		style.ItemInnerSpacing = { 6.f, 6.f };
		style.TouchExtraPadding = { 0.f, 0.f };
		style.IndentSpacing = 21.f;
		style.ScrollbarSize = 15.f;
		style.GrabMinSize = 8.f;
		style.WindowBorderSize = 1.5f;
		style.ChildBorderSize = 1.5f;
		style.PopupBorderSize = 1.5f;
		style.FrameBorderSize = 0.f;
		style.WindowRounding = 3.f;
		style.ChildRounding = 3.f;
		style.FrameRounding = 3.0f;
		style.ScrollbarRounding = 1.f;
		style.GrabRounding = 1.f;
		style.ButtonTextAlign = { 0.5f, 0.5f };
		style.DisplaySafeAreaPadding = { 3.f, 3.f };

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4({ 255.f,255.f,255.f,1.f });
		colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 0.90f, 0.19f, 1.f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.f, 0.f, 0.f, 0.9f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(1.f, 0.15f, 0.18f, 0.7f);
		colors[ImGuiCol_BorderShadow] = ImVec4(1.f, 1.f, 1.f, 1.f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.21f, 0.21f, 0.21f, 0.78f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.27f, 0.27f, 1.f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_SliderGrab] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_Button] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_ButtonActive] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_Header] = ImVec4(0.19f, 0.19f, 0.19f, 1.f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.19f, 0.19f, 0.19f, 1.f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
		colors[ImGuiCol_Separator] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(1.f, 0.15f, 0.18f, 1.f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.26f, 0.26f, 1.f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.9f);

		float ImGuiWidth = 500.f;
		float ImGuiHight = 360.f;
		static int switchTabs = 4;

		float psilentfov = 0.f;
		bool psilent = false;
		static bool espsettings = true;
		static bool weaponexploits = true;
		static bool playerexploits = true;
		static bool miscfuncs = true;
		static bool playervisuals = true;
		static bool worldentities = true;
		ImGui::Begin("Syndicate | Rust Menu", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse /*ImGuiWindowFlags_NoScrollbar*/);

		ImGui::SetWindowSize(ImVec2((float)ImGuiWidth, (float)ImGuiHight));

		if (ImGui::Button("Aimbot", ImVec2(120.f, 40.0f)))
			switchTabs = 0;
		ImGui::SameLine(0.0, 2.0f);
		if (ImGui::Button("Visuals", ImVec2(120.f, 40.0f)))
			switchTabs = 1;
		ImGui::SameLine(0.0, 2.0f);
		if (ImGui::Button("Exploits", ImVec2(120.f, 40.0f)))
			switchTabs = 2;
		ImGui::SameLine(0.0, 2.0f);
		if (ImGui::Button("Settings", ImVec2(120.f, 40.0f)))
			switchTabs = 3;

		switch (switchTabs) {
		case 0:

			ImGui::Separator();

			ImGui::Checkbox("Psilent", &config.psilent);
			if (psilent) {
				ImGui::SliderFloat("Psilent Range", &config.psilentfov, 0.f, 1.f);

			}

			ImGui::Checkbox(xorget("Activate Aimbot"), &config.aimbot);
			
				ImGui::Hotkey(xorget("Key"), &config.aimbot_key);
				ImGui::SliderInt(xorget("Fov"), &config.fov, 1, 50);
				ImGui::SliderFloat(xorget("Smoothing"), &config.aimbot_smooth, 0.1f, 1.f);
			

			break;

		case 1:

			ImGui::Separator();
			//ImGui::Text("  ");

			ImGui::Checkbox("Player Visuals", &playervisuals);
			if (playervisuals) {



				ImGui::Checkbox(xorget("Sleeper ESP"), &config.sleeper);
				ImGui::Checkbox(xorget("Box ESP"), &config.box_esp);
				if (config.box_esp) {
					ImGui::Checkbox(xorget("Cornered Box"), &config.cornered_box_esp);
					ImGui::Checkbox(xorget("Filled Box"), &config.filled_box_esp);
				}
				ImGui::Checkbox(xorget("Name ESP"), &config.name_esp);
				ImGui::Checkbox(xorget("Health ESP"), &config.health_esp);
				ImGui::Checkbox(xorget("Snapline ESP"), &config.snapline_esp);
				ImGui::Checkbox(xorget("Skeleton ESP"), &config.skeleton_esp);
				ImGui::Checkbox(xorStr("Crosshair"), &Settings_::Aim::Crosshair);
				if (Settings_::Aim::Crosshair) {
					ImGui::SliderInt(xorStr("Crosshair Size"), &Settings_::Aim::Croshair_size, 1, 7);
				}
			}


			ImGui::Checkbox("World Entities", &worldentities);
			if (worldentities) {

				ImGui::Checkbox(xorget("Node ESP"), &config.node_esp);
				if (config.node_esp) {
					if (ImGui::TreeNodeEx(xorget("Nodes"))) {
						ImGui::Checkbox(xorget("Sulfur ESP"), &config.sulfur_node);
						ImGui::Checkbox(xorget("Stone ESP"), &config.stone_node);
						ImGui::Checkbox(xorget("Metal ESP"), &config.metal_node);
						ImGui::TreePop();
					}
				}
				ImGui::Checkbox(xorget("Scientist ESP"), &config.scientist);
				ImGui::Checkbox(xorget("Dropped Item ESP"), &config.dropped_esp);
				ImGui::Checkbox(xorget("Raid ESP"), &config.Raid_esp);
				ImGui::Checkbox(xorget("Corpse ESP"), &config.corpse_esp);
				ImGui::Checkbox(xorget("Stash ESP"), &config.stash_esp);
				ImGui::Checkbox(xorget("Vehicle ESP"), &config.heli_esp);
				ImGui::Checkbox(xorget("Airdrop ESP"), &config.airdrop);
				ImGui::Checkbox(xorget("Hemp ESP"), &config.hemp);
				ImGui::Checkbox(xorget("Food ESP"), &config.food_esp);
				ImGui::Checkbox(xorget("Cargo Ship ESP"), &config.cargo_esp);
				ImGui::Checkbox(xorget("Patrol Helicopter ESP"), &config.patrol_heli);
				ImGui::Checkbox(xorget("Tool cupboard ESP"), &config.tool_cupboard);
				ImGui::Checkbox(xorget("Hackable Crate ESP"), &config.hackable_crate);
				ImGui::Checkbox(xorget("high_tier_crate ESP"), &config.high_tier_crate);
				ImGui::Checkbox(xorget("low_tier_crate ESP"), &config.low_tier_crate);
			}



			//ImGui::Combo("Weapons", &Weapons, items2, IM_ARRAYSIZE(items2));

			break;
		case 2:

			ImGui::Separator();
			//ImGui::Text("  ");

			ImGui::Checkbox("Weapon Functions", &weaponexploits);
			if (weaponexploits) {

				ImGui::Checkbox(xorget("No Recoil"), &config.recoil);

				ImGui::Checkbox(xorget("No Spread"), &config.spread);

				ImGui::Checkbox(xorget("No Sway"), &config.sway);

				ImGui::Checkbox(xorget("Automatic"), &Settings_::Misc::automatic);

				ImGui::Checkbox(xorget("Instant Eoka"), &Settings_::Misc::InstantEoka);

				ImGui::Checkbox(xorget("Rapid Fire"), &Settings_::Misc::RapidFire);

			}

			ImGui::Checkbox("Player Functions", &playerexploits);
			if (playerexploits) {

				ImGui::Checkbox(xorget("Spiderman / No Fall"), &config.spiderman);

				ImGui::Checkbox(xorStr("Gravity Jump (Middle Mouse Button)"), &Settings_::Misc::SetGravitu);
				if (Settings_::Misc::SetGravitu) {
					ImGui::Hotkey(xorget("Key"), &Settings_::FloatAntInt::GravityButton);
					ImGui::SliderFloat(xorStr("Gravity Jump Value"), &Settings_::FloatAntInt::JumpValue, 0.1f, 10.f);
				}

				ImGui::Checkbox(xorget("Double Jump"), &config.doublejump);

				ImGui::Checkbox(xorStr("Walk On Water"), &Settings_::Misc::WalkonWater);
				if (Settings_::Misc::WalkonWater) {
					ImGui::Hotkey(xorStr("Key"), &config.waterwalkButton);
				}

				//ImGui::Checkbox(xorStr("Long Hand"), &Settings_::Misc::LongHand);
				//if (Settings_::Misc::LongHand) {
					//ImGui::SliderFloat(xorStr("LongHand Range Value"), &Settings_::FloatAntInt::LongHandValue, 1, 4);
				//}


				ImGui::Checkbox(xorStr("Long Neck"), &Settings_::Misc::LongNeck);
				if (Settings_::Misc::LongNeck) {
					ImGui::Hotkey(xorStr("Key"), &config.LongNeckButton);
					ImGui::SliderFloat(xorStr("Long Neck Value"), &Settings_::FloatAntInt::LongNeck, 1.f, 500.f);
				}


			}

			ImGui::Checkbox("Misc", &miscfuncs);
			if (miscfuncs) {

				ImGui::Checkbox(xorget("Admin Mode"), &config.setadmin);

				ImGui::Checkbox(xorget("Sky Color"), &config.skyColorChanger);
				if (config.skyColorChanger)
					ImGui::SliderFloat(xorget("Value"), &config.sky_color, 0.f, 100.f);

				ImGui::Checkbox(xorget("Time Changer"), &config.day_changer);
				if (config.day_changer)
					ImGui::SliderFloat(xorget("DayTime"), &config.day_time, 0.f, 24.f);



			}

			break;
		case 3:

			ImGui::Separator();
			//ImGui::Text("  ");

			ImGui::Checkbox("Settings", &espsettings);
			if (espsettings) {


				ImGui::SliderInt(xorget("ESP Distance"), &config.max_dist, 0, 1000);
				ImGui::SliderInt(xorget("ESP max_dist_ore"), &config.max_dist_ore, 0, 1000);
				ImGui::SliderInt(xorget("ESP max_dist_vehicle"), &config.max_dist_vehicle, 0, 1000);
				ImGui::SliderInt(xorget("ESP max_dist_collectable"), &config.max_dist_collectable, 0, 1000);
				ImGui::SliderInt(xorget("ESP max_dist_high_crate"), &config.max_dist_high_crate, 0, 1000);
				ImGui::SliderInt(xorget("ESP max_dist_low_crate"), &config.max_dist_low_crate, 0, 1000);
				ImGui::SliderInt(xorget("ESP max_dist_corpse"), &config.max_dist_corpse, 0, 1000);
				ImGui::SliderInt(xorget("ESP max_dist_airdrop"), &config.max_dist_airdrop, 0, 1000);
				ImGui::SliderInt(xorget("ESP max_dist_tool_cupboard"), &config.max_dist_tool_cupboard, 0, 1000);
				ImGui::SliderInt(xorget("ESP max_dist_dropped_items"), &config.max_dist_dropped_items, 0, 1000);
				ImGui::SliderInt(xorget("ESP max_dist_stash"), &config.max_dist_stash, 0, 1000);
				ImGui::SliderInt(xorget("ESP max_dist_hackable"), &config.max_dist_hackable, 0, 1000);
				ImGui::SliderInt(xorget("ESP max_dist_Raid"), &config.max_dist_Raid, 0, 1000);

			}

			break;

		}





		ImGui::End();

	}

	bool IntializeMainCanvasMenuOverlay()
	{
		Vector2 Resolution = DisplayInformation::GetDisplayResolution();

		if (1) {
			SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
			WNDCLASSEX WindowClass = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, _T("ImDumb"), NULL };
			RegisterClassEx(&WindowClass);
			HWND MenuWindowHandle = CreateWindowEx(WS_EX_TRANSPARENT, _T("ImDumb"), _T("ImDumb"), WS_POPUP | WS_CHILD, 0, 0, DisplayInformation::GetDisplayResolution().x - 1, DisplayInformation::GetDisplayResolution().y - 1, NULL, NULL, WindowClass.hInstance, NULL);

			MARGINS margins = { -1 };
			DwmExtendFrameIntoClientArea(MenuWindowHandle, &margins);

			if (CreateDeviceD3D(MenuWindowHandle) < 0)
			{
				CleanupDeviceD3D();
				UnregisterClass(_T("Paint"), WindowClass.hInstance);
				return 0;
			}

			ImGui::CreateContext();
			ImGui_ImplDX11_Init(MenuWindowHandle, g_pd3dDevice, g_pd3dDeviceContext);
			ChangeClickAbility(true, MenuWindowHandle);

			Renderer::GetInstance()->Initialize();
			ImGuiIO& io = ImGui::GetIO();
			ImFont* imFont = io.Fonts->AddFontDefault();

			MSG msg;
			ZeroMemory(&msg, sizeof(msg));

			ShowWindow(MenuWindowHandle, SW_SHOWNORMAL);

			while (msg.message != WM_QUIT) {
				SetWindowPos(MenuWindowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					continue;
				}

				ImGui_ImplDX11_NewFrame();

				if (GetAsyncKeyState(VK_INSERT))
				{
					ShowWindow(MenuWindowHandle, SW_SHOWDEFAULT);
					UpdateWindow(MenuWindowHandle);
					DisplayInformation::bMenu = DisplayInformation::bMenu ? false : true;
					DisplayInformation::WindowStatus = DisplayInformation::WindowStatus ? false : true;
					Sleep(150);
				}

				if (DisplayInformation::bMenu) {
					BuildMenuMain();
					ChangeClickAbility(true, MenuWindowHandle);
				}
				else {
					ChangeClickAbility(false, MenuWindowHandle);
				}
				if (!DisplayInformation::WindowStatus) {
					ShowWindow(MenuWindowHandle, SWP_SHOWWINDOW);
				}

				Renderer::GetInstance()->BeginScene();
				g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, reinterpret_cast<const float*>(&DisplayInformation::clearColor));
				std::lock_guard<std::mutex> lk(entity_mutex);

				////////// Crosshair Function //////////
				if (Settings_::Aim::Crosshair)
				{
// removed
				}
				Vector3 position = Vector3(100, 55, 03);

				rust::localplayer_thread();
				if(rust::local)
					rust::gettaggedobjects();

				if(rust::cam)
// figure it out

				Renderer::GetInstance()->DrawScene();
				Renderer::GetInstance()->EndScene();
				ImGui::Render();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
				g_pSwapChain->Present(0, 0);
				HWND DESKTOP = GetForegroundWindow();
				HWND MOVERDESK = GetWindow(DESKTOP, GW_HWNDPREV);
				SetWindowPos(MenuWindowHandle, MOVERDESK, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
				UpdateWindow(MenuWindowHandle);
			}

			ImGui_ImplDX11_Shutdown();
			ImGui::DestroyContext();
			CleanupDeviceD3D();
			UnregisterClass(_T("Paint"), WindowClass.hInstance);
			return 0;
		}
	}
}