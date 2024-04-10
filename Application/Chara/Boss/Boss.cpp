#include "Boss.h"
#include "BossNormal.h"
#include "BossPunch.h"
#include "Camera.h"
#include "ParticleManager.h"
#include "RInput.h"
#include "ImGui.h"
#include "TimeManager.h"
#include "Parameter.h"

Boss::Boss() : GameObject(),
moveSpeed(0.1f), hp(0), maxHP(10.f)
{
	state = std::make_unique<BossNormal>();
	nextState = nullptr;
	obj = PaintableModelObj(Model::Load("./Resources/Model/bossBody/bossBody.obj", "bossBody", true));
	obj.mPaintDissolveMapTex = TextureManager::Load("./Resources/DissolveMap.png", "DissolveMapTex");
	obj.mTransform.scale = Vector3::ONE * 8.f;

	//モデル設定
	parts[(int32_t)PartsNum::LeftHand].obj = PaintableModelObj(Model::Load("./Resources/Model/leftArm/leftArm.obj", "leftArm", true));
	parts[(int32_t)PartsNum::RightHand].obj = PaintableModelObj(Model::Load("./Resources/Model/rightArm/rightArm.obj", "rightArm", true));

	for (size_t i = 0; i < parts.size(); i++)
	{
		parts[i].obj.mTransform.scale = Vector3::ONE * 2.f;
	}

	parts[(size_t)PartsNum::LeftHand].oriPos = { -50.f,20.f,0.f };
	parts[(size_t)PartsNum::RightHand].oriPos = { 50.f,20.f,0.f };

	punchTimer = 0.7f;
	stayTimer = 0.2f;

	BossUI::LoadResource();

	std::map<std::string, std::string> extract = Parameter::Extract("Boss");
	obj.mTransform.scale.x = Parameter::GetParam(extract,"ボス本体のスケールX", obj.mTransform.scale.x);
	obj.mTransform.scale.y = Parameter::GetParam(extract,"ボス本体のスケールY", obj.mTransform.scale.y);
	obj.mTransform.scale.z = Parameter::GetParam(extract,"ボス本体のスケールZ", obj.mTransform.scale.z);
	colliderSize = Parameter::GetParam(extract,"ボス本体の当たり判定", colliderSize);
	mutekiTimer.maxTime_ = Parameter::GetParam(extract,"無敵時間", mutekiTimer.maxTime_);
	
	 parts[(int32_t)PartsNum::LeftHand].obj.mTransform.scale.x = Parameter::GetParam(extract,"左手スケールX", parts[(int32_t)PartsNum::LeftHand].obj.mTransform.scale.x);
	 parts[(int32_t)PartsNum::LeftHand].obj.mTransform.scale.y = Parameter::GetParam(extract,"左手スケールY", parts[(int32_t)PartsNum::LeftHand].obj.mTransform.scale.y);
	 parts[(int32_t)PartsNum::LeftHand].obj.mTransform.scale.z = Parameter::GetParam(extract,"左手スケールZ", parts[(int32_t)PartsNum::LeftHand].obj.mTransform.scale.z);
	 parts[(int32_t)PartsNum::RightHand].obj.mTransform.scale.x = Parameter::GetParam(extract,"右手スケールX", parts[(int32_t)PartsNum::RightHand].obj.mTransform.scale.x);
	 parts[(int32_t)PartsNum::RightHand].obj.mTransform.scale.y = Parameter::GetParam(extract,"右手スケールY", parts[(int32_t)PartsNum::RightHand].obj.mTransform.scale.y);
	 parts[(int32_t)PartsNum::RightHand].obj.mTransform.scale.z = Parameter::GetParam(extract,"右手スケールZ", parts[(int32_t)PartsNum::RightHand].obj.mTransform.scale.z);
}

Boss::~Boss()
{
	////死んだときパーティクル出す
	//ParticleManager::GetInstance()->AddSimple(
	//	obj.mTransform.position, obj.mTransform.scale * 0.5f, 20, 0.3f,
	//	Color::kWhite, "", 0.5f, 0.8f,
	//	{ -0.3f,-0.3f,-0.3f }, { 0.3f,0.3f,0.3f },
	//	0.05f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f);
}

void Boss::Init()
{
	hp = maxHP;

	ai.Init();
}

void Boss::AllStateUpdate()
{
	//全ステート共通処理

	//タイマー更新
	mutekiTimer.Update();
	whiteTimer.Update();
	waxShakeOffTimer.Update();

	//かかっているロウを振り払う
	//10段階かかったら固まる
	if (waxSolidCount > 0 && waxSolidCount < requireWaxSolidCount) {
		//振り払うタイマーが終わったら
		if (waxShakeOffTimer.GetEnd())
		{
			//振り払う
			waxShakeOffTimer.Reset();
			//減るのは一段階
			waxSolidCount--;
		}
	}

	//白を加算
	if (whiteTimer.GetStarted()) {
		brightColor.r = Easing::OutQuad(1.f,0.f, whiteTimer.GetTimeRate());
		brightColor.g = Easing::OutQuad(1.f,0.f, whiteTimer.GetTimeRate());
		brightColor.b = Easing::OutQuad(1.f,0.f, whiteTimer.GetTimeRate());
	}

	//コライダー更新
	UpdateCollider();

	//ミニマップ表示
	ui.Update(this);

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);
	obj.mPaintDataBuff->dissolveVal = waxSolidCount >= requireWaxSolidCount ? 1.0f : 0.3f / (requireWaxSolidCount - 1) * waxSolidCount;
	obj.mPaintDataBuff->color = Color(0.8f, 0.6f, 0.35f, 1.0f);
	obj.mPaintDataBuff->slide += TimeManager::deltaTime;

	for (size_t i = 0; i < parts.size(); i++)
	{
		parts[i].SetDrawCollider(isDrawCollider);
		parts[i].Update();
	}
}

void Boss::Update()
{
	ai.Update(this);

	//各ステート時の固有処理
	state->Update(this);

	//全ステートの共通処理
	AllStateUpdate();

	// モーションの変更(デバッグ用)
	if (RInput::GetInstance()->GetKeyDown(DIK_1))
	{
		state = std::make_unique<BossNormal>();
	}
	else if (RInput::GetInstance()->GetKeyDown(DIK_2))
	{
		state = std::make_unique<BossPunch>(true);
	}
	else if (RInput::GetInstance()->GetKeyDown(DIK_3))
	{
		state = std::make_unique<BossPunch>(false);
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 300, 250 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Boss", NULL, window_flags);

	ImGui::Text("ボス本体");
	ImGui::Text("1:待機\n2:左パンチ\n3:右パンチ");
	ImGui::Checkbox("オブジェクト描画", &isDrawObj);
	ImGui::Checkbox("当たり判定描画", &isDrawCollider);

	if (ImGui::TreeNode("調整項目_ボス")) {
		ImGui::DragFloat3("スケール", &obj.mTransform.scale.x, 0.1f);
		ImGui::DragFloat("当たり判定", &colliderSize, 0.1f);
		ImGui::InputFloat("無敵時間", &mutekiTimer.maxTime_, 0.1f);
		ImGui::TreePop();
	}

	ImGui::Text("------------------------");
	ImGui::Text("手");
	ImGui::Text("手のコライダー座標\nx:%f\ny:%f\nz:%f",
		parts[0].collider.pos.x, parts[0].collider.pos.y, parts[0].collider.pos.z);
	ImGui::Text("手のコライダーサイズ:%f", parts[0].collider.r);
	ImGui::Text("手のコライダー描画フラグ:%d", parts[0].GetIsDrawCollider());
	if (ImGui::TreeNode("調整項目_手")) {
		ImGui::DragFloat3("右手スケール", &parts[(int32_t)PartsNum::LeftHand].obj.mTransform.scale.x, 0.1f);
		ImGui::DragFloat3("左手スケール", &parts[(int32_t)PartsNum::RightHand].obj.mTransform.scale.x, 0.1f);
		ImGui::TreePop();
	}

	if (ImGui::Button("セーブ")) {
		Parameter::Begin("Boss");
		Parameter::Save("ボス本体のスケールX", obj.mTransform.scale.x);
		Parameter::Save("ボス本体のスケールY", obj.mTransform.scale.y);
		Parameter::Save("ボス本体のスケールZ", obj.mTransform.scale.z);
		Parameter::Save("ボス本体の当たり判定", colliderSize);
		Parameter::Save("無敵時間", mutekiTimer.maxTime_);
		Parameter::Save("左手スケールX", parts[(int32_t)PartsNum::LeftHand].obj.mTransform.scale.x);
		Parameter::Save("左手スケールY", parts[(int32_t)PartsNum::LeftHand].obj.mTransform.scale.y);
		Parameter::Save("左手スケールZ", parts[(int32_t)PartsNum::LeftHand].obj.mTransform.scale.z);
		Parameter::Save("右手スケールX", parts[(int32_t)PartsNum::RightHand].obj.mTransform.scale.x);
		Parameter::Save("右手スケールY", parts[(int32_t)PartsNum::RightHand].obj.mTransform.scale.y);
		Parameter::Save("右手スケールZ", parts[(int32_t)PartsNum::RightHand].obj.mTransform.scale.z);
		Parameter::End();
	}

	ImGui::End();
}

void Boss::Draw()
{
	if (isAlive)
	{
		if (isDrawObj) {
			BrightDraw();
			for (size_t i = 0; i < parts.size(); i++)
			{
				parts[i].Draw();
			}
		}

		DrawCollider();
		for (size_t i = 0; i < parts.size(); i++)
		{
			parts[i].DrawCollider();
		}

		ui.Draw();
	}
}

void Boss::DealDamage(int32_t damage)
{
	//無敵時間さん中ならスキップ
	if (mutekiTimer.GetRun())return;

	//無敵時間開始
	mutekiTimer.Start();
	
	//無敵時間の前半の時間は白く光る演出
	whiteTimer.maxTime_ = mutekiTimer.maxTime_ / 2;
	whiteTimer.Start();

	//かかりカウント加算
	waxSolidCount++;
	//振り払いタイマー開始
	//(初期化も行うため、攻撃を受ける度にタイマーが継続する形に)
	waxShakeOffTimer.Start();

	//一応HPにダメージ(使うか不明)
	hp -= damage;
}

void Parts::Init()
{
}

void Parts::Update()
{
	colliderSize = 20.f;

	UpdateCollider();

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Parts::Draw()
{
	obj.Draw();
}
