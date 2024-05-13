#pragma once
#include "Particle2D.h"
class HomingParticle2D :
	public IEmitter2D
{
private:
	Vector2 targetPos_;		//向かう場所

	bool isUseSlimeWax = false;

public:
	HomingParticle2D(Vector2 targetPos);

	//初期化
	void Init() override;
	//更新処理
	void Update() override;
	//パーティクルの追加
	void Add(uint32_t addNum, float life, Color color, TextureHandle tex, float minScale, float maxScale,
		Vector2 minVelo, Vector2 maxVelo, float accelPower = 0.f, float minRot = 0.f, float maxRot = 0.f,
		float growingTimer = 0.f, float endScale = 0.f, bool isGravity = false);
};