#pragma once
#include "LightGroup.h"
#include "ModelObj.h"
#include <memory>

//スポットライトのオブジェクト
class SpotLightObject
{
public:
	void Init();
	void Update();
	void Draw();

	void SetIndex(int32_t index_);
	void SetPtr(LightGroup* lightPtr_);

	int32_t GetIndex();

	ModelObj obj;
	Vector3 atten;
	Vector3 dir;
	Vector2 factorAngle;
	bool isActive = false;
private:
	LightGroup* lightPtr = nullptr;
	int32_t index = 0;	//ライトグループと合わせるライトのインデックス
};

class SpotLightManager 
{
public:
	std::vector<SpotLightObject> spotLightObjs;

	void Init(LightGroup* lightPtr_);
	void Update();
	void Draw();

	void Imgui();

	void Create(Vector3 spawnPos, LightGroup* lightPtr_);
	
	void Load();
	void Save();

private:
	void SetLightPtr(LightGroup* lightPtr_);

private:
	int32_t createIndex = 0;

	Vector3 spawnPos;

	LightGroup* lightPtr = nullptr;
	
	//ホントはここが持つべきじゃないけどDirictionLightをいじる情報も持たせる
	bool dirActive = true;
	Color dirColor = { 1,1,1,1 };

	bool showLightObj = true;
};
