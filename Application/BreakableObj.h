#pragma once
#include "ModelObj.h"
#include <vector>
#include "Easing.h"

class BreakablePart 
{
public:
	void Init();
	void Update();
	void Draw();

	//飛ばす
	void CreateShotVec();

	//地面を設定
	void SetGroundY(float ground) { groundY = ground; };

public:
	ModelObj obj;

	float groundY = 0.0f;

private:
	Vector3 moveVec = { 0,0,0 };
	Vector3 shotVec = { 0,0,0 };		//跳ぶ位置
	float shotPower = 0.0f;				//跳ぶ強さ
	float gravity = 0.0f;				//重力
	
	Vector3 rotRand = { 0,0,0 };

	float setShotPower = 3.f;			//設定用の跳ぶ強さ
	Easing::EaseTimer lowerTimer = 0.5f;		//勢いが無くなるまでの時間
};

class BreakableObj
{
public:
	BreakableObj();
	void Init();
	void Update();
	void Draw();

	void SetGround(const ModelObj& ground);

public:
	std::vector<BreakablePart> parts;	//描画する分割されたほう
	std::list<Model> models;	//分割したモデルの本体を保持しておく

	ModelObj root;	//親(描画はしない)
};

