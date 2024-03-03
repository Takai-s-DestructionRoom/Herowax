/*
* @file ISceneTransition.h
* @brief 各シーントランジションの基底クラス
*/

#pragma once
class ISceneTransition
{
protected:
	bool mIsClosed = false;

public:
	virtual ~ISceneTransition() {}

	virtual void Update() {};
	virtual void Draw() {};

	//トランジションを開ける
	virtual void Open() { mIsClosed = false; };
	//トランジションが開ききっているか
	virtual bool IsOpened() { return !mIsClosed; };
	//トランジションを閉める
	virtual void Close() { mIsClosed = true; };
	//トランジションが閉まりきっているか
	virtual bool IsClosed() { return mIsClosed; };
	//トランジションが動作中か
	virtual bool InProgress() { return false; };
};
