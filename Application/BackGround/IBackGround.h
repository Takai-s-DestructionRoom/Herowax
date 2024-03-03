/*
* @file IBackGround.h
* @brief ゲーム中の背景の基底クラス
*/

#pragma once
class IBackGround
{
public:
	virtual ~IBackGround() {}

	virtual void Update() {}
	virtual void Draw() {}
};

