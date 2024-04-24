#include "EventCaller.h"
#include "SceneTrance.h"

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
