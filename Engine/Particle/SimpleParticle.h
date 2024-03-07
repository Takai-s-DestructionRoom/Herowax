#pragma once
#include "Particle3D.h"

class SimpleParticle :
	public IEmitter3D
{
public:
	//初期化
	void Init() override;
	//更新処理
	void Update() override;
	//パーティクルの追加
	void Add(uint32_t addNum, float life, Color color, float minScale, float maxScale,
		Vector3 minVelo, Vector3 maxVelo, float accelPower = 0.f, Vector3 minRot = {}, Vector3 maxRot = {}, float growingTimer = 0.f) override;
};