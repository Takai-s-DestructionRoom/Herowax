#include "RWindow.h"
#include <imgui_impl_win32.h>

using namespace std;

void RWindow::Init() {
	GetInstance()->InitInternal();
}

RWindow* RWindow::GetInstance() {
	static RWindow instance;
	return &instance;
}

void RWindow::InitInternal()
{
	mWndClassEx.cbSize = sizeof(WNDCLASSEX);
	mWndClassEx.lpfnWndProc = (WNDPROC)WindowProc;
	mWndClassEx.lpszClassName = mWindowName.c_str();
	mWndClassEx.hInstance = GetModuleHandle(nullptr);
	mWndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	
	if (mIconID != 0) {
		mWndClassEx.hIcon = LoadIcon(mWndClassEx.hInstance, MAKEINTRESOURCE(mIconID));
		mWndClassEx.hIconSm = LoadIcon(mWndClassEx.hInstance, MAKEINTRESOURCE(mIconID));
	}

	RegisterClassEx(&mWndClassEx);
	RECT wrc = { 0, 0, mWindowWidth, mWindowHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME, false);

	mWndHandle = CreateWindow(mWndClassEx.lpszClassName,
		mWndClassEx.lpszClassName,
		WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr, //親ウィンドウハンドル
		nullptr, //メニューハンドル
		mWndClassEx.hInstance, //呼び出しアプリケーションハンドル
		nullptr); //オプション

	ShowWindow(mWndHandle, SW_SHOW);
}

void RWindow::ProcessMessage()
{
	RWindow* instance = GetInstance();
	if (PeekMessage(&instance->mMsg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&instance->mMsg); //キー入力メッセージ処理
		DispatchMessage(&instance->mMsg);
	}

	if (GetForegroundWindow() == RWindow::GetWindowHandle() 
		&& instance->mMouseCursorLock) {
		RWindow::SetMouseHideFlag(true);

		int32_t xPos_absolute, yPos_absolute;

		WINDOWINFO windowInfo{};

		//ウィンドウの位置を取得
		windowInfo.cbSize = sizeof(WINDOWINFO);
		GetWindowInfo(RWindow::GetWindowHandle(), &windowInfo);

		//マウスの移動先の絶対座標（モニター左上からの座標）
		xPos_absolute = RWindow::GetWidth() / 2 + windowInfo.rcWindow.left;
		yPos_absolute = RWindow::GetHeight() / 2 + windowInfo.rcWindow.top + 35; //ウィンドウのタイトルバーの分（35px）をプラス

		SetCursorPos(xPos_absolute, yPos_absolute);

		RECT rect{ xPos_absolute, yPos_absolute, xPos_absolute, yPos_absolute };
		ClipCursor(&rect);
	}
	else {
		RWindow::SetMouseHideFlag(false);
		ClipCursor(NULL);
	}
}

Vector2 RWindow::GetMousePos()
{
	POINT point;
	GetCursorPos(&point);

	WINDOWINFO windowInfo{};

	//ウィンドウの位置を取得
	windowInfo.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(RWindow::GetWindowHandle(), &windowInfo);

	ScreenToClient(RWindow::GetWindowHandle(), &point);

	//マウスの移動先の絶対座標（モニター左上からの座標）にして返す
	return Vector2((float)point.x, (float)point.y);
}

void RWindow::SetMousePos(int32_t posX, int32_t posY)
{
	int32_t xPos_absolute, yPos_absolute;

	WINDOWINFO windowInfo{};

	//ウィンドウの位置を取得
	windowInfo.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(RWindow::GetWindowHandle(), &windowInfo);

	//マウスの移動先の絶対座標（モニター左上からの座標）
	xPos_absolute = posX + windowInfo.rcWindow.left;
	yPos_absolute = posY + windowInfo.rcWindow.top + 35; //ウィンドウのタイトルバーの分（35px）をプラス

	SetCursorPos(xPos_absolute, yPos_absolute);
}

void RWindow::SetAbsMousePos(int32_t posX, int32_t posY)
{
	SetCursorPos(posX, posY);
}

void RWindow::SetMouseHideFlag(bool hide)
{
	if (hide) {
		while (ShowCursor(0) >= 0);
	}
	else {
		while (ShowCursor(1) < 0);
	}
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

LRESULT _DefWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return true;

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}
