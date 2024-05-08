#pragma once
#include "LightGroup.h"
#include "ModelObj.h"
#include "Easing.h"
#include <memory>

//スポットライトのオブジェクト
class BaseSpotLight
{
public:
	virtual void Init() = 0;
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

	std::string lightType = "";

protected:
	LightGroup* lightPtr = nullptr;
	int32_t index = 0;	//ライトグループと合わせるライトのインデックス
};

class SpotLightObject : public BaseSpotLight
{
public:
	void Init()override;
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

	void Create(Vector3 spawnPos, std::string lightType_, LightGroup* lightPtr_);

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

	std::string lightType = "";

	LightGroup* lightPtr = nullptr;
	
	std::vector<const char*> typeCombo;
	int32_t comboSelect = 0;

	//ホントはここが持つべきじゃないけどDirictionLightをいじる情報も持たせる
	bool dirActive = true;
	Color dirColor = { 1,1,1,1 };

	bool showLightObj = true;
};
