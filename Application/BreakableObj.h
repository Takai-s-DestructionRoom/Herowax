#pragma once
#include "ModelObj.h"
#include <vector>
#include "Easing.h"
#include "Quaternion.h"

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

	Vector3 moveVec = { 0,0,0 };
	Vector3 shotVec = { 0,0,0 };		//跳ぶ位置
	float shotPower = 3.0f;				//跳ぶ強さ
	float gravity = 0.0f;				//重力
	
	float minY = 0.5f;
	float maxY = 1.0f;

	float setRotSpeed = 0.1f;
private:
	Vector3 rotRand = { 0,0,0 };		//回転方向
	float rotSpeed = 0.1f;
	float rotAngle = 0;
	Quaternion rotQuater;

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

private:
	float shotPower = 3.0f;				//跳ぶ強さ
	float gravity = -1.0f;				//重力

	float minY = 0.5f;
	float maxY = 1.0f;

	float rotSpeed = 0.1f;
};

