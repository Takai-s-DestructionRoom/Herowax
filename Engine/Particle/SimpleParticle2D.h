#pragma once
#include "Particle2D.h"

//範囲内に発生して消えゆく普通のパーティクル
class SimpleParticle2D :
	public IEmitter2D
{
public:
	//初期化
	void Init() override;
	//更新処理
	void Update() override;
	//パーティクルの追加
	void Add(
		uint32_t addNum, float life, Color color, TextureHandle tex, float minScale, float maxScale,
		Vector2 minVelo, Vector2 maxVelo, float accelPower = 0.f, float minRot = 0.f, float maxRot = 0.f,
		float growingTimer = 0.f, float endScale = 0.f, bool isGravity = false);
};