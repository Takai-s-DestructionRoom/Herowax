/*
* @file RWindow.h
* @brief WindowsAPI制御の基盤となるクラス
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <Windows.h>
#include <string>
#include <memory>
#pragma warning(pop)

#include "Vector2.h"

LRESULT _DefWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class RWindow
{
private:
	RWindow() {};
	~RWindow() = default;
	RWindow(const RWindow&) {};
	RWindow& operator=(const RWindow&) { return *this; }

	LRESULT(*WindowProc)(HWND, UINT, WPARAM, LPARAM) = _DefWindowProc; //ウィンドウプロシージャへのポインタ

	int32_t mWindowWidth = 1280; //ウィンドウの幅
	int32_t mWindowHeight = 720; //ウィンドウの高さ
	std::wstring mWindowName = L"DirectXGame";

	uint32_t mIconID = 0;

	WNDCLASSEX mWndClassEx{};
	HWND mWndHandle{};
	MSG mMsg{};

	bool mMouseCursorLock = false;

	void InitInternal();

public:
	//WindowsAPIの初期化処理
	static void Init();
	static RWindow* GetInstance();

	static void SetWindowSize(const int32_t width, const int32_t height) {
		GetInstance()->mWindowWidth = width;
		GetInstance()->mWindowHeight = height;
	}

	static void SetWindowName(const std::wstring name) {
		GetInstance()->mWindowName = name;
	}

	static void SetIcon(const int32_t id) {
		GetInstance()->mIconID = id;
	}

	static int32_t GetWidth() {
		return GetInstance()->mWindowWidth;
	}

	static int32_t GetHeight() {
		return GetInstance()->mWindowHeight;
	}

	static WNDCLASSEX GetWindowClassEx() {
		return GetInstance()->mWndClassEx;
	}

	static HWND GetWindowHandle() {
		return GetInstance()->mWndHandle;
	}

	static MSG GetMessageStructure() {
		return GetInstance()->mMsg;
	}

	//メッセージ処理
	static void ProcessMessage();

	static Vector2 GetMousePos();

	/// <summary>
	/// マウスカーソル移動
	/// ウィンドウの左上を0, 0として指定する
	/// </summary>
	/// <param name="posX">移動先X座標</param>
	/// <param name="posY">移動先Y座標</param>
	static void SetMousePos(int32_t posX, int32_t posY);

	/// <summary>
	/// マウスカーソル移動
	/// モニターの左上を0, 0として指定する
	/// </summary>
	/// <param name="posX">移動先X座標</param>
	/// <param name="posY">移動先Y座標</param>
	static void SetAbsMousePos(int32_t posX, int32_t posY);

	static void SetMouseHideFlag(bool hide);

	/// <summary>
	/// マウスカーソルのロック
	/// 非表示にしたうえで画面中央に固定します
	/// </summary>
	/// <param name="lock">ロックするかどうか</param>
	static void SetMouseLock(bool lock) {
		GetInstance()->mMouseCursorLock = lock;
	}
};
