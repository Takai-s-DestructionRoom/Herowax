#include "Particle3D.h"
#include "Camera.h"
#include "Util.h"

//LightGroup* IEmitter3D::sLightGroup = nullptr;

IEmitter3D::IEmitter3D()
{
	Init();
}

void IEmitter3D::Init()
{
	pos_ = Vector3::ZERO;
	rot_ = Vector3::ZERO;
	scale_ = { 0.1f,0.1f,0.1f };

	addInterval_ = 0;
	maxScale_ = 0;
	minScale_ = 0;
	scaling_ = 0;

	isActive_ = true;	//生成時には有効フラグ立てる

	elapseSpeed_ = 1.f;
}

void IEmitter3D::Update()
{
	//寿命が尽きたパーティクルを全削除
	for (uint32_t i = 0; i < particles_.size(); i++)
	{
		if (particles_[i].aliveTimer.GetEnd())
		{
			particles_.erase(particles_.begin() + i);
			i--;
		}
	}

	//全パーティクル更新
	for (auto& particle : particles_)
	{
		//生存時間とイージング用タイマーの更新
		particle.aliveTimer.Update(elapseSpeed_);
		particle.easeTimer.Update(elapseSpeed_);
		particle.growingTimer.Update(elapseSpeed_);

		//スケールの線形補間
		if (isGrowing_)	//発生時に拡大する場合
		{
			if (particle.growingTimer.GetRun())	//拡大タイマーが動いてたら
			{
				particle.scale = Easing::lerp(0.f, particle.startScale, particle.growingTimer.GetTimeRate());
			}
			else
			{
				particle.scale = Easing::lerp(particle.startScale, particle.endScale, particle.easeTimer.GetTimeRate());
			}
		}
		else
		{
			particle.scale = Easing::lerp(particle.startScale, particle.endScale, particle.easeTimer.GetTimeRate());
		}

		//加速度を速度に加算
		particle.velo += particle.accel;

		//初期のランダム角度をもとに回す
		if (isRotation_)
		{
			particle.obj.mTransform.rotation += particle.plusRot * elapseSpeed_;

			//一回転したら0に戻してあげる
			if (abs(particle.obj.mTransform.rotation.x) >= Util::PI2)
			{
				particle.obj.mTransform.rotation.x = 0.0f;
			}

			if (abs(particle.obj.mTransform.rotation.y) >= Util::PI2)
			{
				particle.obj.mTransform.rotation.y = 0.0f;
			}

			if (abs(particle.obj.mTransform.rotation.z) >= Util::PI2)
			{
				particle.obj.mTransform.rotation.z = 0.0f;
			}
		}

		//重力加算
		if (isGravity_)
		{
			particle.velo.y += particle.gravity * elapseSpeed_;
		}

		//速度による移動
		particle.obj.mTransform.position += particle.velo * elapseSpeed_;

		particle.obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
		particle.obj.mTransform.UpdateMatrix();
	}
}

void IEmitter3D::Draw()
{
	for (auto& p:particles_)
	{
		p.obj.Draw();
	}
}

void IEmitter3D::Add(uint32_t addNum, float life, Color color, float minScale, float maxScale,
	Vector3 minVelo, Vector3 maxVelo, float accelPower, Vector3 minRot, Vector3 maxRot, float growingTimer)
{
	for (uint32_t i = 0; i < addNum; i++)
	{
		//指定した最大数超えてたら生成しない
		if (particles_.size() >= maxParticle_)
		{
			return;
		}

		//リストに要素を追加
		particles_.emplace_back();
		//追加した要素の参照
		Particle3D& p = particles_.back();
		//エミッターの中からランダムで座標を決定
		float pX = Util::GetRand(-scale_.x, scale_.x);
		float pY = Util::GetRand(-scale_.y, scale_.y);
		float pZ = Util::GetRand(-scale_.z, scale_.z);
		Vector3 randomPos(pX, pY, pZ);
		//引数の範囲から大きさランダムで決定
		float sX = Util::GetRand(minScale, maxScale);
		float sY = Util::GetRand(minScale, maxScale);
		float sZ = Util::GetRand(minScale, maxScale);
		Vector3 randomScale(sX, sY, sZ);
		//引数の範囲から飛ばす方向ランダムで決定
		float vX = Util::GetRand(minVelo.x, maxVelo.x);
		float vY = Util::GetRand(minVelo.y, maxVelo.y);
		float vZ = Util::GetRand(minVelo.z, maxVelo.z);
		Vector3 randomVelo(vX, vY, vZ);
		//引数の範囲から回転をランダムで決定
		float rX = Util::GetRand(minRot.x, maxRot.x);
		float rY = Util::GetRand(minRot.y, maxRot.y);
		float rZ = Util::GetRand(minRot.z, maxRot.z);
		Vector3 randomRot(rX, rY, rZ);

		//決まった座標にエミッター自体の座標を足して正しい位置に
		p.obj.mTransform.position = randomPos + pos_;
		//飛んでく方向に合わせて回転
		p.obj.mTransform.rotation = randomRot;
		p.plusRot = p.obj.mTransform.rotation;
		p.velo = randomVelo;
		p.accel = randomVelo * accelPower;
		p.aliveTimer = life;
		p.aliveTimer.Start();
		p.scale = sX;
		p.startScale = p.scale;
		p.endScale = 0.0f;
		p.obj.mTuneMaterial.mColor = color;
		//イージング用のタイマーを設定、開始
		p.easeTimer.maxTime_ = life - growingTimer;	//全体の時間がずれないように最初の拡大部分を引く
		p.easeTimer.Start();
		p.growingTimer.maxTime_ = growingTimer;
		p.growingTimer.Start();
	}
}

void IEmitter3D::SetScale(const Vector3& scale)
{
	scale_ = scale;
	originalScale_ = scale_;			//拡縮用に元のサイズを保管
}

void IEmitter3D::SetScalingTimer(float easeTimer)
{
	scalingTimer_.maxTime_ = easeTimer;
}

void IEmitter3D::StartScalingTimer(bool isRun)
{
	if (isRun)
	{
		scalingTimer_.Start();	//設定と同時にタイマーもスタート
	}
	else
	{
		scalingTimer_.ReverseStart();	//設定と同時にタイマーもスタート
	}
}