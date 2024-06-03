#pragma once
#include "Vector2.h"
#include <string>
#include <array>

class UIOnce
{
public:
	void Draw();

	std::string load_file = "";
	Vector2 position{};
	Vector2 size{};
	float alpha = 1.f;
};

class ControlUI
{
public:
	static void LoadResource();

	ControlUI();

	void Init();

	void Update();

	void Draw();

private:
	std::array<UIOnce,12> uiOnces;
	Vector2 position;
	Vector2 size;
	float alpha;
};

