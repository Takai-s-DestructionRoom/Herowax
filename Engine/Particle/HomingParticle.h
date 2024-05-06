#pragma once
#include "Particle3D.h"
class HomingParticle :
    public IEmitter3D
{
private:
	Vector3 targetPos_;		//向かう場所

	bool isUseSlimeWax = false;

public:
	HomingParticle(Vector3 targetPos, bool useSlimeWax = false);

	//初期化
	void Init() override;
	//更新処理
	void Update() override;
	//パーティクルの追加
	void Add(uint32_t addNum, float life, Color color, TextureHandle tex, float minScale, float maxScale,
		Vector3 minVelo, Vector3 maxVelo, float accelPower = 0.f, Vector3 minRot = {}, Vector3 maxRot = {},
		float growingTimer = 0.f, float endScale = 0.f, bool isGravity = false, bool isBillboard = false);
};