/*
* @file IScene.h
* @brief 各シーンの基底クラス
*/

#pragma once
class IScene
{
public:
	virtual ~IScene() {}

	virtual void Init() {}
	virtual void Update() {}
	virtual void Draw() {}
	virtual void Finalize() {}
};

