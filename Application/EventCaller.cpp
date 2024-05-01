#include "EventCaller.h"
#include "SceneTrance.h"
#include "BossAppearanceScene.h"

std::string EventCaller::eventCallStr = "";
std::string EventCaller::nowEventStr = "";
std::unique_ptr<IEventScene> EventCaller::eventScene;

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
    eventScene = std::make_unique<BossAppearanceScene>();
}

void EventCaller::Update()
{
	//イベントシーン中なら
	if (eventScene->isActive)
	{
		eventScene->Update();
	}
}

void EventCaller::Draw()
{
    if (eventScene->isActive)
    {
        eventScene->Draw();
    }
}
