#pragma once
#include "string"
#include "IEventScene.h"
#include "Camera.h"

//イベント呼び出し機
class EventCaller
{
public:
	//イベント呼び出しハンドルを設定
	static void EventCall(const std::string& str,bool transFlag = true);

	//イベント呼び出しハンドルを取得
	static std::string GetEventCallStr();
	//イベント呼び出しハンドルを初期化
	static void EventCallStrReset();

	//呼び出し中イベント管理文字列を取得
	static std::string GetNowEventStr();
	//呼び出し中イベント管理文字列を削除
	static void NowEventStrReset();

	static void Init();
	static void Update();
	static void Draw();

	static EventCaller* GetInstance();
	std::unique_ptr<IEventScene> eventScene;

	static Camera* saveCamera;

private:
	EventCaller() = default;
	EventCaller(EventCaller& a) = delete;
	EventCaller& operator=(EventCaller& a) = delete;

	//イベント呼び出しを管理する文字列
	static std::string eventCallStr;

	//呼び出し中のイベントを取得するための文字列
	static std::string nowEventStr;
};

