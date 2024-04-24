#include "Boss.h"
#include "BossAppearance.h"
#include "BossDead.h"
#include "BossNormal.h"
#include "BossPunch.h"
#include "Camera.h"
#include "ParticleManager.h"
#include "RInput.h"
#include "RImGui.h"
#include "TimeManager.h"
#include "Parameter.h"
#include "GameCamera.h"
#include "WaxManager.h"
#include "BossCollectStandState.h"
#include "EventCaller.h"
#include "BossDeadScene.h"
#include "BossAppearanceScene.h"

Boss::Boss() : GameObject(),
moveSpeed(0.1f), hp(0), maxHP(10.f)
{
	state = std::make_unique<BossNormal>();
	nextState = nullptr;
	obj = PaintableModelObj(Model::Load("./Resources/Model/bossBody/bossBody.obj", "bossBody", true));
	obj.mPaintDissolveMapTex = TextureManager::Load("./Resources/DissolveMap.png", "DissolveMapTex");
	obj.mTransform.scale = Vector3::ONE * 8.f;

	//モデル設定
	parts[(int32_t)PartsNum::RightHand].obj = PaintableModelObj(Model::Load("./Resources/Model/leftArm/leftArm.obj", "leftArm", true));
	parts[(int32_t)PartsNum::LeftHand].obj = PaintableModelObj(Model::Load("./Resources/Model/rightArm/rightArm.obj", "rightArm", true));

	parts[(size_t)PartsNum::LeftHand].oriPos = { -50.f,20.f,0.f };
	parts[(size_t)PartsNum::RightHand].oriPos = { 50.f,20.f,0.f };

	for (size_t i = 0; i < parts.size(); i++)
	{
		parts[i].Init();
	}

	targetCircle = ModelObj(Model::Load("./Resources/Model/targetMark/targetMark.obj", "targetMark", true));

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
	standTimer = Parameter::GetParam(extract,"モーション待機時間", 3.f);
	punchTimer = Parameter::GetParam(extract,"パンチにかかる時間", 0.7f);
	punchStayTimer = Parameter::GetParam(extract,"パンチ後留まる時間", 1.5f);

	bossSpawnTimer = Parameter::GetParam(extract, "ボスが出現するまでの時間", 60.0f);

	extract = Parameter::Extract("Player");
	solidTime = Parameter::GetParam(extract, "固まるまでの時間", 5.f);
	atkPower = (int32_t)Parameter::GetParam(extract, "敵に与えるダメージ", 10.0f);
	atkRange = Parameter::GetParam(extract, "攻撃範囲", 3.f);
	atkSize = 0.f;
	atkSpeed = Parameter::GetParam(extract, "射出速度", 1.f);
	atkTime = Parameter::GetParam(extract, "攻撃時間", 0.5f);
}

Boss::~Boss()
{
}

void Boss::Init()
{
	hp = maxHP;
	isAlive = false;
	bossSpawnTimer.Start();
	
	mutekiTimer.Reset();

	waxSolidCount = 0;
	waxShakeOffTimer.Reset();

	shakeTimer.Reset();
	waxScatterTimer.Reset();

	shake = Vector3::ZERO;

	standTimer.Reset();
	punchTimer.Reset();
	punchStayTimer.Reset();

	isAppearance = false;
	isDead = false;

	isOldBodySolid = false;

	ChangeState<BossNormal>();

	ai.Init();

	for (size_t i = 0; i < parts.size(); i++)
	{
		parts[i].Init();
	}

	deadEventCall = false;
}

void Boss::AllStateUpdate()
{
	//全ステート共通処理
	//シェイクを初期化
	obj.mTransform.position -= shake;
	shake = { 0,0,0 };

	//タイマー更新
	mutekiTimer.Update();
	whiteTimer.Update();
	waxShakeOffTimer.Update();
	shakeTimer.Update();
	waxScatterTimer.Update();

	//かかっているロウを振り払う
	//10段階かかったら固まる
	if (waxSolidCount > 0 && !GetIsSolid(PartsNum::Max)) {
		//振り払うタイマーが終わったら
		if (waxShakeOffTimer.GetEnd())
		{
			//振り払う
			waxShakeOffTimer.Start();
			//減るのは一段階
			waxSolidCount--;

			//体だけ残っているならシェイクも入れる
			if (GetIsOnlyBody()) {
				shakeTimer.Start();
			}
		}
	}

	//シェイク中なら適当な値を入れる
	if (shakeTimer.GetRun()) {
		shake.x = Util::GetRand(-1.0f, 1.0f);
		shake.y = Util::GetRand(-1.0f, 1.0f);
		shake.z = Util::GetRand(-1.0f, 1.0f);

		//本体はロウ出さない
		//Vector3 atkVec = Util::GetRandVector3({ 0,0,0 }, -0.1f, 0.1f, { 1,0,1 });

		////ついでにその場にロウをばらまく(勝手実装)
		////生成
		//if (!waxScatterTimer.GetRun()) {
		//	waxScatterTimer.Start();
		//	Transform spawnTrans = obj.mTransform;
		//	spawnTrans.position.y -= spawnTrans.scale.y * 2;
		//	WaxManager::GetInstance()->Create(
		//		spawnTrans, atkPower, atkVec, atkSpeed,
		//		atkRange, atkSize, atkTime, solidTime);
		//	//シェイク中に自分が出したロウに当たらないように無敵にする
		//	mutekiTimer.Start();
		//}
	}

	//固まったなら
	if (GetIsSolid(PartsNum::LeftHand)) 
	{
		//状態を右手オンリーに
		ai.SetSituation(BossSituation::OnlyRight);

		//吸収可能に
		parts[(int32_t)PartsNum::LeftHand].isCollected = true;
		
	}
	if (GetIsSolid(PartsNum::RightHand)) 
	{
		//状態を左手オンリーに
		ai.SetSituation(BossSituation::OnlyLeft);

		//吸収可能に
		parts[(int32_t)PartsNum::RightHand].isCollected = true;

	}
	//両方固まってるなら
	if (GetIsSolid(PartsNum::LeftHand) && GetIsSolid(PartsNum::RightHand))
	{
		ai.SetSituation(BossSituation::NoArms);
	}

	//本体が固まったら撃破演出を呼び出し
	if (GetStateStr() == "Collected" && !deadEventCall) {
		EventCaller::EventCall(BossDeadScene::GetEventCallStr());
		deadEventCall = true;
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

	//座標加算
	obj.mTransform.position += shake;

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);
	obj.mPaintDataBuff->dissolveVal = waxSolidCount >= requireWaxSolidCount ? 1.0f : 0.3f / (requireWaxSolidCount - 1) * waxSolidCount;
	obj.mPaintDataBuff->color = Color(0.8f, 0.6f, 0.35f, 1.0f);
	obj.mPaintDataBuff->slide += TimeManager::deltaTime;

	for (size_t i = 0; i < parts.size(); i++)
	{
		//無敵時間をボス本体と合わせる
		parts[i].SetMutekiMaxTime(mutekiTimer.maxTime_);
		//描画フラグをボスと合わせて切り替える
		parts[i].SetDrawCollider(isDrawCollider);
		//更新
		parts[i].Update();
	}
}

void Boss::Update()
{
	//タイマーが終わったらボスを出現させる
	bossSpawnTimer.Update();
	if (bossSpawnTimer.GetNowEnd()) {
		EventCaller::EventCall(BossAppearanceScene::GetEventCallStr());
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 300, 250 }, ImGuiCond_FirstUseEver);

	ImGui::Begin("Boss");
	ImGui::Text("ボスが出現するまでの時間:%f", bossSpawnTimer.nowTime_);

	ImGui::Text("ボス本体");
	ImGui::Text("1:待機\n2:左パンチ\n3:右パンチ");
	ImGui::Checkbox("オブジェクト描画", &isDrawObj);
	ImGui::Checkbox("当たり判定描画", &isDrawCollider);

	if (ImGui::TreeNode("調整項目_ボス")) {
		ImGui::DragFloat("ボスが出現するまでの時間(最大)", &bossSpawnTimer.maxTime_,0.1f);
		if (ImGui::Button("出現タイマーリセット")) {
			bossSpawnTimer.Start();
		}
		ImGui::DragFloat3("スケール", &obj.mTransform.scale.x, 0.1f);
		ImGui::DragFloat("当たり判定", &colliderSize, 0.1f);
		ImGui::InputFloat("無敵時間", &mutekiTimer.maxTime_, 0.1f);
		ImGui::InputFloat("モーション待機時間", &standTimer.maxTime_, 0.1f);
		ImGui::InputFloat("パンチにかかる時間", &punchTimer.maxTime_, 0.1f);
		ImGui::InputFloat("パンチ後留まる時間", &punchStayTimer.maxTime_, 0.1f);
		ImGui::TreePop();
	}

	ImGui::Text("------------------------");
	ImGui::Text("手");
	ImGui::Text("手のコライダー座標\nx:%f\ny:%f\nz:%f",
		parts[0].collider.pos.x, parts[0].collider.pos.y, parts[0].collider.pos.z);
	ImGui::Text("手のコライダーサイズ:%f", parts[0].collider.r);
	ImGui::Text("手のコライダー描画フラグ:%d", parts[0].GetIsDrawCollider());
	ImGui::Text("lefthandWaxSolid:%d", parts[(int32_t)PartsNum::LeftHand].GetWaxSolidCount());
	ImGui::Text("righthandWaxSolid:%d", parts[(int32_t)PartsNum::RightHand].GetWaxSolidCount());
	if (ImGui::TreeNode("調整項目_手")) {
		ImGui::DragFloat3("右手スケール", &parts[(int32_t)PartsNum::LeftHand].obj.mTransform.scale.x, 0.1f);
		ImGui::DragFloat3("左手スケール", &parts[(int32_t)PartsNum::RightHand].obj.mTransform.scale.x, 0.1f);
		ImGui::TreePop();
	}

	if (ImGui::Button("Reset")) {
		Init();
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
		Parameter::Save("モーション待機時間", standTimer.maxTime_);
		Parameter::Save("パンチにかかる時間", punchTimer.maxTime_);
		Parameter::Save("パンチ後留まる時間", punchStayTimer.maxTime_);
		Parameter::Save("ボスが出現するまでの時間", bossSpawnTimer.GetTimeRate());

		Parameter::End();
	}

	ImGui::End();
#pragma endregion

	//ImGuiと出現呼び出し以外は、ボスの出現後に呼び出すため、isAliveがtrueでなければ通さない
	if (!isAlive)return;

	isOldBodySolid = GetIsSolid(PartsNum::Max);

	ai.Update(this);

	//各ステート時の固有処理
	state->Update(this);

	//全ステートの共通処理
	AllStateUpdate();

	// モーションの変更(デバッグ用)
	if (Util::debugBool) {
		if (RInput::GetInstance()->GetKeyDown(DIK_1))
		{
			ChangeState<BossNormal>();
		}
		else if (RInput::GetInstance()->GetKeyDown(DIK_2))
		{
			nextState = std::make_unique<BossPunch>(true);
			changingState = true;
		}
		else if (RInput::GetInstance()->GetKeyDown(DIK_3))
		{
			nextState = std::make_unique<BossPunch>(false);
			changingState = true;
		}
	}

	//出現中なのに出現ステートになってないなら
	if (isAppearance && stateStr != "Appearance")
	{
		ChangeState<BossAppearance>();
	}
	//逆に出現中じゃないのに出現ステートになってたら
	else if (isAppearance == false && stateStr == "Appearance")
	{
		ChangeState<BossNormal>();
	}

	//撃破演出中なのに撃破演出ステートになってないなら
	if (isDead && stateStr != "Dead")
	{
		ChangeState<BossDead>();
	}

	if (changingState) {
		//ステートを変化させる
		std::swap(state, nextState);
		changingState = false;
		nextState = nullptr;
	}

	targetCircle.mTransform.UpdateMatrix();
	targetCircle.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Boss::Draw()
{
	if (!isAlive)return;
	
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

	//パンチ中のみ描画
	if (punchTimer.GetRun())
	{
		targetCircle.Draw();
	}

	ui.Draw();
}

bool Boss::GetIsSolid(PartsNum num)
{
	switch (num)
	{
	case PartsNum::RightHand:
		return parts[(int32_t)PartsNum::RightHand].GetWaxSolidCount() >= 
			parts[(int32_t)PartsNum::RightHand].requireWaxSolidCount;
		break;
	case PartsNum::LeftHand:
		return  parts[(int32_t)PartsNum::LeftHand].GetWaxSolidCount() >=
			parts[(int32_t)PartsNum::LeftHand].requireWaxSolidCount;
		break;
	default:
		return waxSolidCount >= requireWaxSolidCount;
		break;
	}
}

bool Boss::GetIsOnlyBody()
{
	return GetIsSolid(PartsNum::RightHand) && GetIsSolid(PartsNum::LeftHand);
}

Boss* Boss::GetInstance()
{
	static Boss instance;
	return &instance;
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
	waxSolidCount += damage;
	//振り払いタイマー開始
	//(初期化も行うため、攻撃を受ける度にタイマーが継続する形に)
	
	//腕が残っていないなら通常タイマー
	if (GetIsOnlyBody()) {
		waxShakeOffTimer.maxTime_ = armBreakOffTime;
	}
	//残っているなら超短いタイマー
	else {
		waxShakeOffTimer.maxTime_ = armRemainOffTime;
	}
	
	waxShakeOffTimer.Start();

	//一応HPにダメージ(使うか不明)
	hp -= damage;

	//固まってるなら
	if (GetIsSolid(PartsNum::Max) && !isOldBodySolid) {
		ChangeState<BossCollectStandState>();
	}
}