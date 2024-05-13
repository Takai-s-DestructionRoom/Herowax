#include "EnemyManager.h"
#include "RImGui.h"
#include "Colliders.h"
#include "Parameter.h"
#include "Tank.h"
#include "BombSolider.h"

void EnemyManager::LoadResource()
{
	EnemyUI::LoadResource();
	EnemyAttackState::LoadResource();
	EnemyShot::LoadResource();
}

EnemyManager* EnemyManager::GetInstance()
{
	static EnemyManager instance;
	return &instance;
}

bool EnemyManager::GetNowCollectEnemy()
{
	for (auto& enemy : enemys)
	{
		if (enemy->GetState() == "EnemyCollect") {
			return true;
		}
	}
	
	return false;
}

EnemyManager::EnemyManager()
{
	std::map<std::string, std::string> extract = Parameter::Extract("Enemy");
	slowMag = Parameter::GetParam(extract,"減速率", 0.75f);
	slowCoatingMag = Parameter::GetParam(extract,"ろうまみれ減速率", 0.9f);
	burningBonus = Parameter::GetParam(extract,"敵が燃えたときのボーナス上昇温度",2.f);

	knockTime = Parameter::GetParam(extract, "ノックバックにかかる時間", 0.5f);
	knockRange = Parameter::GetParam(extract, "ノックバックする距離", 0.5f);

	knockRandXS = Parameter::GetParam(extract, "knockRandXS", -Util::PI / 2);
	knockRandXE = Parameter::GetParam(extract, "knockRandXE", Util::PI / 2);
	knockRandZS = Parameter::GetParam(extract, "knockRandZS", Util::PI / 4);
	knockRandZE = Parameter::GetParam(extract, "knockRandZE", Util::PI / 2);
	mutekiTime = Parameter::GetParam(extract, "無敵時間さん", 0.1f);

	enemySize.x = Parameter::GetParam(extract,"敵の大きさX", enemySize.x);
	enemySize.y = Parameter::GetParam(extract,"敵の大きさY", enemySize.y);
	enemySize.z = Parameter::GetParam(extract,"敵の大きさZ", enemySize.z);
	collideSize = Parameter::GetParam(extract,"敵の当たり判定の大きさ", collideSize);
	attackMove = Parameter::GetParam(extract,"突っ込んでくる距離", 100.f);
	tankFindColliderSize = Parameter::GetParam(extract,"tankが攻撃状態になる大きさ", 3.0f);
	rangeFindColliderSize = Parameter::GetParam(extract,"rangeが攻撃状態になる大きさ", 30.f);
	moveSpeed = Parameter::GetParam(extract,"移動速度", 0.1f);

	normalAtkPower = Parameter::GetParam(extract,"敵の攻撃力", 1.f);
	isContactDamage = Parameter::GetParam(extract,"接触ダメージをつけるか",0.0f);

	shotDamage = Parameter::GetParam(extract, "弾の威力", 1.f);
	shotLifeTime = Parameter::GetParam(extract, "弾の生存時間", 2.0f);
	shotMoveSpeed = Parameter::GetParam(extract,"弾の速度", 1.5f);
}

void EnemyManager::CreateEnemyShot(Enemy* enemy)
{
	enemyShots.emplace_back();
	enemyShots.back() = std::make_unique<EnemyShot>();
	enemyShots.back()->Init();
	Vector3 targetVec = enemy->GetTarget()->mTransform.position - enemy->GetPos();
	targetVec.Normalize();
	enemyShots.back()->SetParam(enemy->GetPos(), targetVec);
}

void EnemyManager::SetShotParam(float damage, float moveSpeed_, float lifeTime_)
{
	shotDamage = damage;
	shotMoveSpeed = moveSpeed_;
	shotLifeTime = lifeTime_;
}

void EnemyManager::SetTarget(ModelObj* target_)
{
	target = target_;
}

void EnemyManager::Init()
{
	enemys.clear();
	Model::Load("./Resources/Model/enemy/enemy.obj", "enemy", true);
	Model::Load("./Resources/Model/Cube.obj", "Cube", true);
}

void EnemyManager::Update()
{
	Delete();

	if (!isStop) {

		for (auto& enemy : enemys)
		{
			enemy->SetSlowMag(slowMag);	//減速率まとめて変更
			enemy->SetSlowCoatingMag(slowCoatingMag);	//蝋かかかった時の減速率まとめて変更
			enemy->SetKnockRange(knockRange);
			enemy->SetKnockTime(knockTime);
			enemy->SetMutekiTime(mutekiTime);
			enemy->SetTarget(target);

			enemy->Update();
		}
	}

	for (auto& enemy : enemys)
	{
		enemy->TransfarBuffer();
	}

	for (auto& shot : enemyShots)
	{
		shot->Update();
	}

	burningComboTimer.Update();
	//猶予時間過ぎたらリセット
	if (burningComboTimer.GetEnd())
	{
		burningCombo = 0;
	}

	solidCombo = 0;	//同フレームで固まった敵のみカウントするので逐一0に戻す
	
	static Color changeColor = { 1,1,1,1 };
	static bool hitChecker = false;
	
	static std::string handle = "";

#pragma region ImGui
	if (RImGui::showImGui) {
		ImGui::SetNextWindowSize({ 300, 150 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("Enemy");

		if (ImGui::Button("敵を攻撃状態へ遷移")) {
			for (auto& enemy : enemys)
			{
				enemy->ChangeAttackState<EnemyFindState>();
			}
		}

		if (ImGui::Button("敵を回収状態へ遷移")) {
			for (auto& enemy : enemys)
			{
				enemy->ChangeState<EnemyCollect>();
			}
		}

		ImGui::Text("EnemyNum:%d", enemys.size());
		ImGui::DragFloat3("敵の大きさ", &enemySize.x, 0.1f);
		if (ImGui::TreeNode("攻撃系"))
		{
			ImGui::Checkbox("接触ダメージをつけるか", &isContactDamage);
			ImGui::InputFloat("敵の攻撃力(突進時)", &normalAtkPower, 1.f);
			if (isContactDamage) {
				ImGui::InputFloat("敵の攻撃力(接触時)", &contactAtkPower, 1.f);
			}
			else {
				ImGui::Text("接触時の敵攻撃力の調整項目は、");
				ImGui::Text("接触ダメージをつけるフラグが立っている場合のみ表示されます");
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("当たり判定"))
		{
			ImGui::Checkbox("当たり判定描画", &hitChecker);

			ImGui::DragFloat("敵の当たり判定の大きさ", &collideSize, 0.1f);
			if (ImGui::TreeNode("突っ込んでくる敵"))
			{
				ImGui::DragFloat("攻撃状態になる当たり判定の大きさ", &tankFindColliderSize, 1.0f);
				ImGui::DragFloat("突っ込んでくる距離", &attackMove, 1.0f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("遠距離攻撃する敵"))
			{
				ImGui::DragFloat("攻撃状態になる当たり判定の大きさ", &rangeFindColliderSize, 1.0f);
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("温度系"))
		{
			ImGui::PushItemWidth(100);
			ImGui::InputFloat("敵が燃えたときのボーナス上昇温度", &burningBonus, 1.f);
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("速度系"))
		{
			ImGui::PushItemWidth(100);
			ImGui::InputFloat("移動速度", &moveSpeed, 0.1f);
			ImGui::SliderFloat("減速率", &slowMag, 0.f, 1.f);
			ImGui::SliderFloat("ろうまみれ減速率", &slowCoatingMag, 0.f, 1.f);
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("ノックバック系"))
		{
			ImGui::SliderFloat("ノックバックにかかる時間", &knockTime, 0.0f, 5.0f);
			ImGui::SliderFloat("ノックバックする距離", &knockRange, 0.0f, 10.f);

			ImGui::SliderFloat("knockRandXS", &knockRandXS, -Util::PI, Util::PI);
			ImGui::SliderFloat("knockRandXE", &knockRandXE, -Util::PI, Util::PI);
			ImGui::SliderFloat("knockRandZS", &knockRandZS, -Util::PI, Util::PI);
			ImGui::SliderFloat("knockRandZE", &knockRandZE, -Util::PI, Util::PI);
			ImGui::SliderFloat("knockRandZE", &knockRandZE, -Util::PI, Util::PI);
			ImGui::TreePop();
		}		
		if (ImGui::TreeNode("敵の弾")) {
			ImGui::DragFloat("弾の威力",&shotDamage,0.1f);
			ImGui::DragFloat("弾の生存時間",&shotLifeTime, 0.1f);
			ImGui::DragFloat("弾の速度",&shotMoveSpeed, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("デバッグ")) {
			//敵のモデルをボタンで切り替えられるようにする
			static bool changeModel = false;
			if (ImGui::Button("モデル切り替え")) {
				changeModel = !changeModel;
				if (changeModel) {
					handle = "enemy";
				}
				else {
					handle = "Cube";
				}
			}
			//敵を出現させられるようにする
			if (ImGui::Button("敵出現(ボスの位置)")) {
				CreateEnemy<Enemy>(Vector3(0, 0, 0), { 3,3,3 }, { 0,0,0 }, "test", "Debug");
			}
			
			ImGui::Checkbox("Tankの表示/非表示", &tankSwitch);
			ImGui::Checkbox("BombSoliderの表示/非表示", &bombSoliderSwitch);

			ImGui::TreePop();
		}
		ImGui::SliderFloat("無敵時間さん", &mutekiTime, 0.0f, 1.0f);
		ImGui::ColorEdit4("ロウで固まってるときの色", &changeColor.r);

		if (ImGui::Button("Reset")) {
			enemys.clear();
		}
		if (ImGui::Button("セーブ")) {
			Parameter::Begin("Enemy");
			Parameter::Save("減速率", slowMag);
			Parameter::Save("ろうまみれ減速率", slowCoatingMag);
			Parameter::Save("敵が燃えたときのボーナス上昇温度", burningBonus);
			Parameter::Save("ノックバックにかかる時間", knockTime);
			Parameter::Save("ノックバックする距離", knockRange);
			Parameter::Save("knockRandXS", knockRandXS);
			Parameter::Save("knockRandXE", knockRandXE);
			Parameter::Save("knockRandZS", knockRandZS);
			Parameter::Save("knockRandZE", knockRandZE);
			Parameter::Save("knockRandZE", knockRandZE);
			Parameter::Save("knockRandZE", knockRandZE);
			Parameter::Save("敵の大きさX", enemySize.x);
			Parameter::Save("敵の大きさY", enemySize.y);
			Parameter::Save("敵の大きさZ", enemySize.z);
			Parameter::Save("敵の当たり判定の大きさ", collideSize);
			Parameter::Save("tankが攻撃状態になる大きさ", tankFindColliderSize);
			Parameter::Save("rangeが攻撃状態になる大きさ", rangeFindColliderSize);
			Parameter::Save("突っ込んでくる距離", attackMove);
			Parameter::Save("移動速度", moveSpeed);
			Parameter::Save("敵の攻撃力", normalAtkPower);
			Parameter::Save("弾の威力", shotDamage);
			Parameter::Save("弾の生存時間", shotLifeTime);
			Parameter::Save("弾の速度",shotMoveSpeed);

			Parameter::End();
		}

		ImGui::End();
	}
#pragma endregion
	
	//パラメータを適用
	for (auto& enemy : enemys)
	{
		enemy->changeColor = changeColor;
		enemy->colliderSize = collideSize;
		enemy->obj.mTransform.scale = enemySize;
		enemy->SetDrawCollider(hitChecker);
		if (enemy->enemyTag == Tank::GetEnemyTag()) {
			enemy->attackHitCollider.r = tankFindColliderSize;
		}
		if (enemy->enemyTag == BombSolider::GetEnemyTag()) {
			enemy->attackHitCollider.r = rangeFindColliderSize;
		}
		enemy->attackMovePower = attackMove;
		enemy->SetMoveSpeed(moveSpeed);
	
		if (handle != "") {
			enemy->obj.mModel = ModelManager::Get(handle);
		}
	}

	if (handle != "") {
		handle = "";
	}


	for (auto& shot : enemyShots)
	{
		shot->SetDamage(shotDamage);
		shot->SetLifeTime(shotLifeTime);
		shot->SetMoveSpeed(shotMoveSpeed);
	}
}

void EnemyManager::Draw()
{
	for (auto& enemy : enemys)
	{
		//タンクを描画しようとしてるときにスイッチがオフになってたら飛ばす
		if (!tankSwitch && enemy->enemyTag == Tank::GetEnemyTag()) continue;
		//ボム兵を描画しようとしてるときにスイッチがオフになってたら飛ばす
		if (!bombSoliderSwitch && enemy->enemyTag == BombSolider::GetEnemyTag()) continue;
		
		enemy->Draw();
	}
	for (auto& shot : enemyShots)
	{
		shot->Draw();
	}
}

void EnemyManager::Delete()
{
	collectNum = 0;

	for (auto& enemy : enemys)
	{
		if (enemy->isAlive == false)
		{
			collectNum++;
		}
	}

	//死んでるならリストから削除
	enemys.remove_if([](std::unique_ptr<Enemy>& enemy) {
		return !enemy->GetIsAlive();
		});

	enemyShots.remove_if([](std::unique_ptr<EnemyShot>& shot) {
		return !shot->GetIsAlive();
		});
}
