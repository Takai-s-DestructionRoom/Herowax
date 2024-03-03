#include "TimeManager.h"
#include <list>
#include <string>

namespace TimeManager
{
	float targetFPS = 60;
	float fps = 0;
	float deltaTime = 0;
	float deltaMiliTime = 0;
	float deltaFrame = 0;
	LARGE_INTEGER timeFreq;
	LARGE_INTEGER timeStart;
	LARGE_INTEGER timeEnd;
	std::list<float> fpsSamples;
}

void TimeManager::Init()
{
	QueryPerformanceFrequency(&timeFreq);
	QueryPerformanceCounter(&timeStart);
}

void TimeManager::Update()
{
	if (timeFreq.QuadPart == 0) {
		return;
	}
	QueryPerformanceCounter(&timeEnd);

	if (targetFPS > 0) {
		float targetTime = 1.0f / targetFPS;
		float frameTime = (float)(timeEnd.QuadPart - timeStart.QuadPart) / (float)timeFreq.QuadPart;

		if (frameTime < targetTime) {
			unsigned long sleepTime = (unsigned long)((targetTime - frameTime) * 1000);

			timeBeginPeriod(1);
			Sleep(sleepTime);
			timeEndPeriod(1);
		}

		QueryPerformanceCounter(&timeEnd);
	}

	deltaTime = (float)(timeEnd.QuadPart - timeStart.QuadPart) / (float)timeFreq.QuadPart;
	deltaMiliTime = deltaTime * 1000.0f;
	deltaFrame = deltaTime / ((1.0f / targetFPS));

	if (deltaTime > 0) {
		fpsSamples.push_back(1.0f / deltaTime);
		if (fpsSamples.size() > 5) {
			fpsSamples.pop_front();
		}

		float _fps = 0;
		for (float f : fpsSamples) {
			_fps += f;
		}
		_fps /= fpsSamples.size();
		fps = _fps;
	}

	timeStart = timeEnd;
}
