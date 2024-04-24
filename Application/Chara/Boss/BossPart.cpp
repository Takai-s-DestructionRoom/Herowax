#include "BossPart.h"
#include "TimeManager.h"
#include "WaxManager.h"
#include "Parameter.h"
#include "ParticleManager.h"
#include "Wax.h"

Parts::Parts() : GameObject()
{
	colliderSize = 20.f;

	std::map<std::string, std::string> extract = Parameter::Extract("Player");
	solidTime = Parameter::GetParam(extract, "固まるまでの時間", 5.f);
	atkPower = (int32_t)Parameter::GetParam(extract, "敵に与えるダメージ", 10.0f);
	atkRange = Parameter::GetParam(extract, "攻撃範囲", 3.f);
	atkSize = 0.f;
	atkSpeed = Parameter::GetParam(extract, "射出速度", 1.f);
	atkTime = Parameter::GetParam(extract, "攻撃時間", 0.5f);
}

Parts::~Parts()
{
	ParticleManager::GetInstance()->AddHoming(
		obj.mTransform.position, obj.mTransform.scale,
		10, 0.8f, Wax::waxOriginColor, "", 0.8f, 1.5f,
		-Vector3::ONE * 0.3f, Vector3::ONE * 0.3f,
		0.03f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.3f, 0.5f);
}

void Parts::Init()
{
	isAlive = true;

	isCollected = false;
	mutekiTimer.Reset();

	waxShakeOffTimer.Reset();

	shake = Vector3::ZERO;
	shakeTimer.Reset();
	waxScatterTimer.Reset();

	waxSolidCount = 0;

	obj.mPaintDissolveMapTex = TextureManager::Load("./Resources/DissolveMap.png", "DissolveMapTex");

	state = std::make_unique<BossPartNormal>();
	changingState = false;

	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Parts::Update()
{
	mutekiTimer.Update();
	whiteTimer.Update();
	waxShakeOffTimer.Update();
	shakeTimer.Update();
	waxScatterTimer.Update();

	//シェイクを初期化
	obj.mTransform.position -= shake;
	shake = { 0,0,0 };

	//ステート更新(吸収)
	state->Update(this);
	if (changingState) {
		std::swap(state, nextState);
		changingState = false;
		nextState = nullptr;
	}

	//かかっているロウを振り払う
	//10段階かかったら固まる
	if (waxSolidCount > 0 && waxSolidCount < requireWaxSolidCount) {
		//振り払うタイマーが終わったら
		if (waxShakeOffTimer.GetEnd())
		{
			waxShakeOffTimer.Start();
			//振り払う
			//減るのは一段階
			waxSolidCount--;
			//シェイク入れる
			shakeTimer.Start();
		}
	}

	//白を加算
	if (whiteTimer.GetStarted()) {
		brightColor.r = Easing::OutQuad(1.f, 0.f, whiteTimer.GetTimeRate());
		brightColor.g = Easing::OutQuad(1.f, 0.f, whiteTimer.GetTimeRate());
		brightColor.b = Easing::OutQuad(1.f, 0.f, whiteTimer.GetTimeRate());
	}

	//シェイク中なら適当な値を入れる
	if (shakeTimer.GetRun()) {
		shake.x = Util::GetRand(-1.0f,1.0f);
		shake.y = Util::GetRand(-1.0f,1.0f);
		shake.z = Util::GetRand(-1.0f,1.0f);

		Vector3 atkVec = Util::GetRandVector3({ 0,0,0 }, -0.1f, 0.1f, {1,0,1});

		//ついでにその場にロウをばらまく(勝手実装)
		//生成
		if (!waxScatterTimer.GetRun()) {
			waxScatterTimer.Start();
			Transform spawnTrans = obj.mTransform;
			spawnTrans.position.y -= spawnTrans.scale.y * 2;
			//仕様変更に伴いいったん削除
			//どうせもうちょい広がる感じに作り直す予定なのでヨシ
			/*WaxManager::GetInstance()->Create(
				spawnTrans, atkVec, atkSpeed,
				atkRange, atkSize, atkTime, solidTime);*/
			//シェイク中に自分が出したロウに当たらないように無敵にする
			mutekiTimer.Start();
		}
	}

	UpdateCollider();

	//座標加算
	obj.mTransform.position += shake;

	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);
	obj.mPaintDataBuff->dissolveVal = waxSolidCount >= requireWaxSolidCount ? 1.0f : 0.3f / (requireWaxSolidCount - 1) * waxSolidCount;
	obj.mPaintDataBuff->color = Color(0.8f, 0.6f, 0.35f, 1.0f);
	obj.mPaintDataBuff->slide += TimeManager::deltaTime;
}

void Parts::Draw()
{
	if (isAlive) {	
		BrightDraw();
	}
}

bool Parts::GetIsSolid()
{
	return waxSolidCount >= requireWaxSolidCount;
}

void Parts::DealDamage(int32_t damage)
{
	//無敵時間さん中ならスキップ
	if (mutekiTimer.GetRun())return;

	//無敵時間開始
	mutekiTimer.Start();

	//無敵時間の前半の時間は白く光る演出
	whiteTimer.maxTime_ = mutekiTimer.maxTime_ / 2;
	whiteTimer.Start();

	//かかりカウント加算
	waxSolidCount += damage;
	//振り払いタイマー開始
	//(初期化も行うため、攻撃を受ける度にタイマーが継続する形に)
	waxShakeOffTimer.Start();

	waxScatterTimer.Reset();
}