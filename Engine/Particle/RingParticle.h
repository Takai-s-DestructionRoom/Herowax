#pragma once
#include "Particle3D.h"

//エミッター座標を中心に指定した半径の大きさでリング状に出るパーティクル
class RingParticle :
	public IEmitter3D
{
public:
	//初期化
	void Init() override;
	//更新処理
	void Update() override;
	//パーティクルの追加
	void AddRing(uint32_t addNum, float life, Color color, float radius, float minScale, float maxScale,
		Vector3 minVelo, Vector3 maxVelo, float accelPower = 0.f, Vector3 minRot = {}, Vector3 maxRot = {}, float growingTimer = 0.f) override;
};