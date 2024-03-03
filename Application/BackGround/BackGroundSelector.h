/*
* @file BackGroundSelector.h
* @brief ゲーム中の背景を文字列から選択するクラス
*/

#pragma once
#include <string>
#include <memory>
#include "IBackGround.h"

class BackGroundSelector
{
public:
	/// <summary>
	/// 指定した名前から背景を選択して生成する
	/// </summary>
	/// <param name="name">背景名</param>
	void Select(std::string name);

	void Update();
	void Draw();

private:
	std::unique_ptr<IBackGround> mBG;
};

