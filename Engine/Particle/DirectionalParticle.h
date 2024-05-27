#pragma once
#include "Particle3D.h"
class DirectionalParticle :
    public IEmitter3D
{	
	//初期化
	void Init() override;
	//更新処理
	void Update() override;
	//パーティクルの追加
	void Add(
		uint32_t addNum, float life, Color color, TextureHandle tex, float minScale, float maxScale,
		Vector3 minVelo, Vector3 maxVelo, float accelPower = 0.f,
		float growingTimer = 0.f, float endScale = 0.f, bool isGravity = false,
		bool isBillboard = false, BlendMode blendMode = BlendMode::Add);

};

