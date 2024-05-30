/*
* @file RInput.h
* @brief DirectInput, XInputを使った入力処理クラス
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>
#pragma warning(pop)

#include "Vector3.h"
#include "Vector2.h"
#include <Xinput.h>

class RInput {
public:
	//入力情報管理機能初期化
	static void Init();
	static RInput* GetInstance();

	//入力情報の更新
	static void Update();

	//キーが押されているか
	static bool GetKey(unsigned char key);
	//キーが離れた瞬間か
	static bool GetKeyUp(unsigned char key);
	//キーが押された瞬間か
	static bool GetKeyDown(unsigned char key);

	//マウスの状態を得る
	static inline DIMOUSESTATE2 GetMouseState() {
		return GetInstance()->mMouseState;
	}
	//前回のマウスの状態を得る
	static inline DIMOUSESTATE2 GetOldMouseState() {
		return GetInstance()->mOldMouseState;
	}

	//マウスクリックされているか
	static bool GetMouseClick(int32_t buttonNum);
	//マウスクリックが離れた瞬間か
	static bool GetMouseClickUp(int32_t buttonNum);
	//マウスクリックがされた瞬間か
	static bool GetMouseClickDown(int32_t buttonNum);

	//マウスの位置を取得する
	static Vector2 GetMousePos();

	//前回のマウスの位置を取得する
	static Vector2 GetOldMousePos();

	//マウスの移動量を取得する
	static Vector3 GetMouseMove();

	//パッドに接続されてるか
	static bool GetPadConnect();

	//パッドのボタンが押されているか
	static bool GetPadButton(uint32_t button);

	//パッドのボタンが離れた瞬間か
	static bool GetPadButtonUp(uint32_t button);

	//パッドのボタンが押された瞬間か
	static bool GetPadButtonDown(uint32_t button);

	//パッドの左スティック
	Vector2 GetPadLStick();

	//パッドの右スティック
	Vector2 GetPadRStick();

	//左トリガーを押し込んだ瞬間か
	bool GetLTriggerDown();

	//右トリガーを押し込んだ瞬間か
	bool GetRTriggerDown();

	//左トリガーを離した瞬間か
	bool GetLTriggerUp();

	//右トリガーを離した瞬間か
	bool GetRTriggerUp();

	//右トリガーを押し込んでいるか
	bool GetRTrigger();
	
	//左トリガーを押し込んでいるか
	bool GetLTrigger();

	//左スティックを上に倒した瞬間か
	bool GetLStickUp();

	//左スティックを下に倒した瞬間か
	bool GetLStickDown();

	/// <summary>
	/// 左スティック入力を得る
	/// </summary>
	/// <param name="useWASD">WASDキーもスティックに見立てて処理する</param>
	/// <param name="useArrow">矢印キーもスティックに見立てて処理する</param>
	/// <returns>入力量</returns>
	static Vector2 GetLStick(bool useWASD, bool useArrow);

	/// <summary>
	/// 右スティック入力を得る
	/// </summary>
	/// <param name="useWASD">WASDキーもスティックに見立てて処理する</param>
	/// <param name="useArrow">矢印キーもスティックに見立てて処理する</param>
	/// <returns>入力量</returns>
	static Vector2 GetRStick(bool useWASD, bool useArrow);

private:
	RInput() {};
	~RInput() = default;
	RInput(const RInput&) {};
	RInput& operator=(const RInput&) { return *this; }

	bool mIsInitialized = false;
	Microsoft::WRL::ComPtr<IDirectInput8> mDirectInput = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mKeyboard = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mMouse = nullptr;
	BYTE mKeyState[256] = {};
	BYTE mOldKeyState[256] = {};
	DIMOUSESTATE2 mMouseState = {};
	DIMOUSESTATE2 mOldMouseState = {};
	Vector2 mMousePos;
	Vector2 mOldMousePos;
	XINPUT_STATE mXInputState{};
	XINPUT_STATE mOldXInputState{};
	bool mIsPadConnected = false;

	void InitInternal();
};
