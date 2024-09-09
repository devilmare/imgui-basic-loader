#include "Menu.hpp"

WNDCLASSEX               wc;
HWND                     main_hwnd;
LPDIRECT3DDEVICE9        g_pd3dDevice;
D3DPRESENT_PARAMETERS    g_d3dpp;
LPDIRECT3D9              g_pD3D;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    {
        g_d3dpp.Windowed = TRUE;
        g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
        g_d3dpp.EnableAutoDepthStencil = TRUE;
        g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    }

    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);

    ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::destroy()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
}

void Menu::render()
{
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    ImVec2 window_pos = ImVec2((screen_width - window::size.x) / 2.0f, (screen_height - window::size.y) / 2.0f);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(window::size, ImGuiCond_Once);

    ImGui::Begin(window::title, &window::is_open, window::flags);
    {
        ImGui::NewLine();
        ImGui::Text("Username:");
        
        ImGui::PushItemWidth(-1);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, (30 - ImGui::GetTextLineHeight()) * 0.5f));

        ImGui::InputText("##USERNAME", window::szUsername, sizeof window::szUsername, ImGuiInputTextFlags_CharsNoBlank);

        ImGui::Text("Password:");

        ImGui::InputText("##PASSWRD", window::szPassword, sizeof window::szPassword, ImGuiInputTextFlags_Password);

        ImGui::PopStyleVar();
        ImGui::PopItemWidth();

        ImGui::NewLine();

        if (ImGui::Button("Login", ImVec2(-1, 30)) && strlen(window::szUsername) > 0 && strlen(window::szPassword) > 0)
        {
            MessageBoxA(GetForegroundWindow(), "Work!", "Info", MB_ICONINFORMATION);
        }
    }
    ImGui::End();

    if (!window::is_open) // destroy menu, if you press (x) in title bar
    {
        destroy(); 
        PostQuitMessage(0);
    }
}

int Menu::setup()
{
	wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, window::title, NULL };
    RegisterClassEx(&wc);

	main_hwnd = CreateWindow(wc.lpszClassName, window::title, WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);
    if (!CreateDeviceD3D(main_hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(main_hwnd, SW_HIDE);
    UpdateWindow(main_hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); // setup main imgui config
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // to enable imgui in desktop
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = NULL;
    io.LogFilename = NULL;

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) // setup imgui style
    {
        style.Alpha = 1.0f;
        style.WindowRounding = 0.0f;
        style.WindowPadding = ImVec2(15, 15);
        style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
        style.FrameRounding = 5.0f;
        style.FramePadding = ImVec2(5, 5);
        style.ItemSpacing = ImVec2(12, 8);
        style.ItemInnerSpacing = ImVec2(8, 6);
        style.IndentSpacing = 25.0f;
        style.ScrollbarSize = 15.0f;
        style.ScrollbarRounding = 8.0f;
        style.GrabMinSize = 5.0f;
        style.GrabRounding = 3.0f;

        style.Colors[ImGuiCol_WindowBg] = ImColor(18, 18, 18, 255);

        style.Colors[ImGuiCol_FrameBg] = ImColor(31, 31, 31, 255);
        style.Colors[ImGuiCol_FrameBgActive] = ImColor(41, 41, 41, 255);
        style.Colors[ImGuiCol_FrameBgHovered] = ImColor(41, 41, 41, 255);

        style.Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
        style.Colors[ImGuiCol_Separator] = ImColor(36, 36, 36, 255);
    }

    ImGui_ImplWin32_Init(main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

	auto is_done = false;
	while (!is_done)
	{
		MSG message;
		while (PeekMessage(&message, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);

			if (message.message == WM_QUIT)
				is_done = true;
		}

		if (is_done)
			break;

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			render();
		}
		ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        {
            ResetDevice();
        }
	}

    destroy();

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}