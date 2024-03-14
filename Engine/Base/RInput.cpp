#include "RInput.h"
#include <memory>
#include <cassert>
#include "RWindow.h"

using namespace std;
using namespace Microsoft::WRL;

RInput* RInput::GetInstance() {
	static RInput instance;
	return &instance;
}

void RInput::Init() {
	GetInstance()->InitInternal();
}

void RInput::InitInternal()
{
	assert(!mIsInitialized);

	HRESULT result;

	result = DirectInput8Create(
		RWindow::GetWindowClassEx().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&mDirectInput, nullptr
	);
	assert(SUCCEEDED(result));

	result = mDirectInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, NULL);
	assert(SUCCEEDED(result));
	result = mDirectInput->CreateDevice(GUID_SysMouse, &mMouse, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = mKeyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	result = mMouse->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = mKeyboard->SetCooperativeLevel(
		RWindow::GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE // | DISCL_NOWINKEY //左から、アクティブウィンドウ,専有しない,Winキー無効
	);
	assert(SUCCEEDED(result));
	result = mMouse->SetCooperativeLevel(
		RWindow::GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE //左から、アクティブウィンドウ,専有しない
	);
	assert(SUCCEEDED(result));

	ZeroMemory(&mXInputState, sizeof(XINPUT_STATE));
	DWORD dresult;
	dresult = XInputGetState(0, &mXInputState);

	if (result == ERROR_SUCCESS)
	{
		mIsPadConnected = true;
	}
	else
	{
		mIsPadConnected = false;
	}
}

void RInput::Update()
{
	RInput* instance = GetInstance();

	//情報の取得開始
	instance->mKeyboard->Acquire();
	instance->mMouse->Acquire();

	//全キーの入力状態を取得する
	for (int32_t i = 0; i < 256; i++) {
		instance->mOldKeyState[i] = instance->mKeyState[i];
	}
	instance->mOldMouseState = instance->mMouseState;
	instance->mKeyboard->GetDeviceState(sizeof(mKeyState), instance->mKeyState);
	instance->mMouse->GetDeviceState(sizeof(mMouseState), &instance->mMouseState);

	instance->mOldMousePos = instance->mMousePos;
	instance->mMousePos = RWindow::GetInstance()->GetMousePos();

	instance->mOldXInputState = instance->mXInputState;
	DWORD dresult = XInputGetState(0, &instance->mXInputState);
	if (dresult == ERROR_SUCCESS) {
		instance->mIsPadConnected = true;
	}
	else {
		instance->mIsPadConnected = false;
	}

	if ((instance->mXInputState.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		instance->mXInputState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(instance->mXInputState.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		instance->mXInputState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		instance->mXInputState.Gamepad.sThumbLX = 0;
		instance->mXInputState.Gamepad.sThumbLY = 0;
	}

	if ((instance->mXInputState.Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		instance->mXInputState.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
		(instance->mXInputState.Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		instance->mXInputState.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
	{
		instance->mXInputState.Gamepad.sThumbRX = 0;
		instance->mXInputState.Gamepad.sThumbRY = 0;
	}
}

bool RInput::GetKey(unsigned char key)
{
	return GetInstance()->mKeyState[key];
}

bool RInput::GetKeyUp(unsigned char key)
{
	return !GetInstance()->mKeyState[key] && GetInstance()->mOldKeyState[key];
}

bool RInput::GetKeyDown(unsigned char key)
{
	return GetInstance()->mKeyState[key] && !GetInstance()->mOldKeyState[key];
}

bool RInput::GetMouseClick(int32_t buttonNum)
{
	return (GetInstance()->mMouseState.rgbButtons[buttonNum] & 0x80) != 0;
}

bool RInput::GetMouseClickUp(int32_t buttonNum)
{
	return (GetInstance()->mMouseState.rgbButtons[buttonNum] & 0x80) == 0 && (GetInstance()->mOldMouseState.rgbButtons[buttonNum] & 0x80) != 0;
}

bool RInput::GetMouseClickDown(int32_t buttonNum)
{
	return (GetInstance()->mMouseState.rgbButtons[buttonNum] & 0x80) != 0 && (GetInstance()->mOldMouseState.rgbButtons[buttonNum] & 0x80) == 0;
}

Vector2 RInput::GetMousePos()
{
	return GetInstance()->mMousePos;
}

Vector2 RInput::GetOldMousePos()
{
	return GetInstance()->mOldMousePos;
}

Vector3 RInput::GetMouseMove() {
	return Vector3((float)GetInstance()->mMouseState.lX, (float)GetInstance()->mMouseState.lY, (float)GetInstance()->mMouseState.lZ);
}

bool RInput::GetPadConnect()
{
	return GetInstance()->mIsPadConnected;
}

bool RInput::GetPadButton(uint32_t button)
{
	return GetInstance()->mXInputState.Gamepad.wButtons == button;
}

bool RInput::GetPadButtonUp(uint32_t button)
{
	return GetInstance()->mXInputState.Gamepad.wButtons != button && GetInstance()->mOldXInputState.Gamepad.wButtons == button;
}

bool RInput::GetPadButtonDown(uint32_t button)
{
	return GetInstance()->mXInputState.Gamepad.wButtons == button && GetInstance()->mOldXInputState.Gamepad.wButtons != button;
}

Vector2 RInput::GetPadLStick()
{
	SHORT x = mXInputState.Gamepad.sThumbLX;
	SHORT y = mXInputState.Gamepad.sThumbLY;

	return Vector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
}

Vector2 RInput::GetPadRStick()
{
	SHORT x = mXInputState.Gamepad.sThumbRX;
	SHORT y = mXInputState.Gamepad.sThumbRY;

	return Vector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
}

bool RInput::GetLTriggerDown()
{
	if (mOldXInputState.Gamepad.bLeftTrigger < 128 && mXInputState.Gamepad.bLeftTrigger >= 128)
	{
		return true;
	}
	return false;
}

bool RInput::GetRTriggerDown()
{
	if (mOldXInputState.Gamepad.bRightTrigger < 128 && mXInputState.Gamepad.bRightTrigger >= 128)
	{
		return true;
	}
	return false;
}

bool RInput::GetLTriggerUp()
{
	if (mOldXInputState.Gamepad.bLeftTrigger >= 128 && mXInputState.Gamepad.bLeftTrigger < 128)
	{
		return true;
	}
	return false;
}

bool RInput::GetRTriggerUp()
{
	if (mOldXInputState.Gamepad.bRightTrigger >= 128 && mXInputState.Gamepad.bRightTrigger < 128)
	{
		return true;
	}
	return false;
}

bool RInput::GetRTrigger()
{
	if (mXInputState.Gamepad.bRightTrigger >= 128)
	{
		return true;
	}
	return false;
}

bool RInput::GetLStickUp()
{
	if (mOldXInputState.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		mXInputState.Gamepad.sThumbLY >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		return true;
	}
	return false;
}

bool RInput::GetLStickDown()
{
	if (mOldXInputState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		mXInputState.Gamepad.sThumbLY <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		return true;
	}
	return false;
}

Vector2 RInput::GetLStick(bool useWASD, bool useArrow)
{
	RInput* instance = GetInstance();

	Vector2 pad;
	pad.x = static_cast<float>(instance->mXInputState.Gamepad.sThumbLX) / 32767.0f;
	pad.y = static_cast<float>(instance->mXInputState.Gamepad.sThumbLY) / 32767.0f;

	Vector2 wasd;
	if (useWASD) {
		wasd.x = static_cast<float>(instance->mKeyState[DIK_D] - instance->mKeyState[DIK_A]);
		wasd.y = static_cast<float>(instance->mKeyState[DIK_W] - instance->mKeyState[DIK_S]);
	}

	Vector2 arrow;
	if (useArrow) {
		arrow.x = static_cast<float>(instance->mKeyState[DIK_RIGHT] - instance->mKeyState[DIK_LEFT]);
		arrow.y = static_cast<float>(instance->mKeyState[DIK_UP] - instance->mKeyState[DIK_DOWN]);
	}

	Vector2 result = pad + wasd + arrow;
	result.Normalize();
	return result;
}

Vector2 RInput::GetRStick(bool useWASD, bool useArrow)
{
	RInput* instance = GetInstance();

	Vector2 pad;
	pad.x = static_cast<float>(instance->mXInputState.Gamepad.sThumbRX) / 32767.0f;
	pad.y = static_cast<float>(instance->mXInputState.Gamepad.sThumbRY) / 32767.0f;

	Vector2 wasd;
	if (useWASD) {
		wasd.x = static_cast<float>(instance->mKeyState[DIK_D] - instance->mKeyState[DIK_A]);
		wasd.y = static_cast<float>(instance->mKeyState[DIK_W] - instance->mKeyState[DIK_S]);
	}

	Vector2 arrow;
	if (useArrow) {
		arrow.x = static_cast<float>(instance->mKeyState[DIK_RIGHT] - instance->mKeyState[DIK_LEFT]);
		arrow.y = static_cast<float>(instance->mKeyState[DIK_UP] - instance->mKeyState[DIK_DOWN]);
	}

	Vector2 result = pad + wasd + arrow;
	result.Normalize();
	return result;
}
