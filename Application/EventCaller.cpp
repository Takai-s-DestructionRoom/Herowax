#include "EventCaller.h"
#include "SceneTrance.h"
#include "BossAppearanceScene.h"

std::string EventCaller::eventCallStr = "";
std::string EventCaller::nowEventStr = "";

std::string EventCaller::GetEventCallStr()
{
    return eventCallStr;
}

void EventCaller::EventCall(const std::string& str)
{
    SceneTrance::GetInstance()->Start();
    eventCallStr = str;

    nowEventStr = str;
}

void EventCaller::EventCallStrReset()
{
    eventCallStr = "";
}

std::string EventCaller::GetNowEventStr()
{
    return nowEventStr;
}

void EventCaller::NowEventStrReset()
{
    nowEventStr = "";
}

void EventCaller::Init()
{
    GetInstance()->eventScene = std::make_unique<BossAppearanceScene>();
}

void EventCaller::Update()
{
	//イベントシーン中なら
	if (GetInstance()->eventScene->isActive)
	{
        GetInstance()->eventScene->Update();
	}
}

void EventCaller::Draw()
{
    if (GetInstance()->eventScene->isActive)
    {
        GetInstance()->eventScene->Draw();
    }
}

EventCaller* EventCaller::GetInstance()
{
    static EventCaller instance;
    return &instance;
}
