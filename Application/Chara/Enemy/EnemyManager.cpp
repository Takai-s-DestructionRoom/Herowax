#include "EnemyManager.h"
#include "RImGui.h"
#include "Colliders.h"
#include "Parameter.h"

void EnemyManager::LoadResource()
{
	EnemyUI::LoadResource();
	EnemyAttackState::LoadResource();
}

EnemyManager* EnemyManager::GetInstance()
{
	static EnemyManager instance;
	return &instance;
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
	attackHitColliderSize = Parameter::GetParam(extract,"敵がプレイヤーと当たる判定の大きさ", 3.0f);
	moveSpeed = Parameter::GetParam(extract,"移動速度", 0.1f);

	normalAtkPower = Parameter::GetParam(extract,"敵の攻撃力", 1.f);
	isContactDamage = Parameter::GetParam(extract,"接触ダメージをつけるか",0.0f);
}

void EnemyManager::SetTarget(ModelObj* target_)
{
	target = target_;
}

void EnemyManager::Init()
{
	enemys.clear();
}

void EnemyManager::Update()
{
	Delete();

	for (auto& enemy : enemys)
	{
		enemy->SetSlowMag(slowMag);	//減速率まとめて変更
		enemy->SetSlowCoatingMag(slowCoatingMag);	//蝋かかかった時の減速率まとめて変更
		enemy->SetKnockRange(knockRange);
		enemy->SetKnockTime(knockTime);
		enemy->SetMutekiTime(mutekiTime);

		enemy->Update();
	}

	burningComboTimer.Update();
	//猶予時間過ぎたらリセット
	if (burningComboTimer.GetEnd())
	{
		burningCombo = 0;
	}

	solidCombo = 0;	//同フレームで固まった敵のみカウントするので逐一0に戻す

#pragma region ImGui
	ImGui::SetNextWindowSize({ 300, 150 }, ImGuiCond_FirstUseEver);

	ImGui::Begin("Enemy");

	if (ImGui::Button("敵を攻撃状態へ遷移")) {
		for (auto& enemy : enemys)
		{
			enemy->ChangeAttackState<EnemyFindState>();
		}
	}

	ImGui::Text("EnemyNum:%d", enemys.size());
	ImGui::DragFloat3("敵の大きさ", &enemySize.x, 0.1f);
	static bool hitChecker = false;
	if (ImGui::TreeNode("攻撃系"))
	{
		ImGui::Checkbox("接触ダメージをつけるか", &isContactDamage);
		ImGui::InputFloat("敵の攻撃力", &normalAtkPower, 1.f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("当たり判定"))
	{
		ImGui::Checkbox("当たり判定描画", &hitChecker);
		
		ImGui::InputFloat("敵の当たり判定の大きさ", &collideSize,0.1f);
		ImGui::InputFloat("敵が自分と当たる当たり判定の大きさ", &attackHitColliderSize, 1.0f);
		ImGui::InputFloat("突っ込んでくる距離", &attackMove, 1.0f);
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
		ImGui::InputFloat("移動速度", &moveSpeed,0.1f);
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
	ImGui::SliderFloat("無敵時間さん", &mutekiTime, 0.0f,1.0f);
	static Color changeColor = { 1,1,1,1 };
	ImGui::ColorEdit4("ロウで固まってるときの色", &changeColor.r);

	for (auto& enemy : enemys)
	{
		ImGui::Text("ステート:%s", enemy->GetState().c_str());
		enemy->changeColor = changeColor;
		enemy->colliderSize = collideSize;
		enemy->obj.mTransform.scale = enemySize;
		enemy->SetDrawCollider(hitChecker);
		enemy->attackHitCollider.r = attackHitColliderSize;
		enemy->attackMovePower = attackMove;
		enemy->SetMoveSpeed(moveSpeed);
	}

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
		Parameter::Save("敵がプレイヤーと当たる判定の大きさ", attackHitColliderSize);
		Parameter::Save("突っ込んでくる距離", attackMove);
		Parameter::Save("移動速度", moveSpeed);
		Parameter::Save("敵の攻撃力", normalAtkPower);

		Parameter::End();
	}

	ImGui::End();
#pragma endregion
}

void EnemyManager::Draw()
{
	for (auto& enemy : enemys)
	{
		enemy->Draw();
	}
}

void EnemyManager::Delete()
{
	//死んでるならリストから削除
	enemys.remove_if([](std::unique_ptr<Enemy>& enemy) {
		return !enemy->GetIsAlive();
		});
}
