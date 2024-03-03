/*
* @file ParticleObject.h
* @brief パーティクルクラス群の基底クラス
*/

#pragma once
#include <list>
#include <memory>
#include <Vector3.h>
#include <RootSignature.h>
#include <GraphicsPipeline.h>

class ParticleObject
{
public:
	Vector3 pos;
	bool isDeleted = false;
	bool isForce = false;

	virtual ~ParticleObject() {}

	virtual void Update() = 0;
	virtual void Draw() = 0;

	static void ManageParticleObject(bool all);
	static void Clear();

protected:
	static std::list<std::unique_ptr<ParticleObject>> manageParticleObjList;
	static RootSignature& GetRootSignature();
	static GraphicsPipeline& GetGraphicsPipeline();
	static GraphicsPipeline& GetGraphicsPipelineAlpha();
};
