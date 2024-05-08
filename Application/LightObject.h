#pragma once
#include "LightGroup.h"
#include "ModelObj.h"
#include "Easing.h"
#include <memory>

//スポットライトのオブジェクト
class BaseSpotLight
{
public:
	void Init();
	virtual void Update() = 0;
	void Draw();

	~BaseSpotLight() {};

	void SetIndex(int32_t index_);
	void SetPtr(LightGroup* lightPtr_);

	int32_t GetIndex();

	ModelObj obj;
	Vector3 atten;
	Vector3 dir;
	Vector2 factorAngle;
	bool isActive = false;
protected:
	LightGroup* lightPtr = nullptr;
	int32_t index = 0;	//ライトグループと合わせるライトのインデックス
};

class SpotLightObject : public BaseSpotLight
{
public:
	void Update()override;
};

class SpotLightManager
{
public:
	static SpotLightManager* GetInstance();

	std::vector<std::unique_ptr<SpotLightObject>> spotLightObjs;

	void Init(LightGroup* lightPtr_);
	void Update();
	void Draw();

	void Imgui();

	void Create(Vector3 spawnPos, LightGroup* lightPtr_);

	void Load();
	void Save();

	SpotLightObject* GetLight(int32_t index);

private:
	void SetLightPtr(LightGroup* lightPtr_);

	SpotLightManager() {};
	~SpotLightManager() {};

private:
	int32_t createIndex = 0;

	Vector3 spawnPos;

	LightGroup* lightPtr = nullptr;
	
	//ホントはここが持つべきじゃないけどDirictionLightをいじる情報も持たせる
	bool dirActive = true;
	Color dirColor = { 1,1,1,1 };

	bool showLightObj = true;
};
