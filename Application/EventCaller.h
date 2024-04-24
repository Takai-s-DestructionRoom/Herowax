#pragma once
#include "string"

//イベント呼び出し機
class EventCaller
{
public:
	//イベント呼び出しハンドルを設定
	static void EventCall(const std::string& str);

	//イベント呼び出しハンドルを取得
	static std::string GetEventCallStr();
	//イベント呼び出しハンドルを初期化
	static void EventCallStrReset();

	//呼び出し中イベント管理文字列を取得
	static std::string GetNowEventStr();
	//呼び出し中イベント管理文字列を削除
	static void NowEventStrReset();

private:
	//イベント呼び出しを管理する文字列
	static std::string eventCallStr;

	//呼び出し中のイベントを取得するための文字列
	static std::string nowEventStr;
};

