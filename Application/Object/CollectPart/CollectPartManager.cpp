#include "CollectPartManager.h"
#include "RImGui.h"
#include "Parameter.h"
#include "Player.h"
#include "RInput.h"

void CollectPartManager::LoadResouces()
{
    //モデル読み込み
    Model::Load("./Resources/Model/hexagonBolt/hexagonBolt.obj", "hexagonBolt");
    Model::Load("./Resources/Model/gear/gear.obj", "gear");
    Model::Load("./Resources/Model/plusScrew/plusScrew.obj", "plusScrew");


}

void CollectPartManager::Init()
{
    for (auto& part : parts)
    {
        part->Init();
    }

    zone.Init();

    parts.clear();

    std::map<std::string, std::string> extract = Parameter::Extract("zone");
    maxCarryingNum = (int32_t)Parameter::GetParam(extract,"一度に運べる数", 1);
    requireCreateNum = (int32_t)Parameter::GetParam(extract,"作るのに必要な個数", 10);
    zone.hammerTimer.maxTime_ = Parameter::GetParam(extract,"作るまでにかかる時間", zone.hammerTimer.maxTime_);
    zone.pos.x = Parameter::GetParam(extract,"位置X", zone.pos.x);
    zone.pos.y = Parameter::GetParam(extract,"位置Y", zone.pos.y);
    zone.pos.z = Parameter::GetParam(extract,"位置Z", zone.pos.z);
    zone.scale.x = Parameter::GetParam(extract,"大きさX", zone.scale.x);
    zone.scale.y = Parameter::GetParam(extract,"大きさZ", zone.scale.y);
    zone.obj.mTuneMaterial.mColor.a = Parameter::GetParam(extract,"透明度", zone.obj.mTuneMaterial.mColor.a);
    zone.healPower = Parameter::GetParam(extract, "回復量", 5.f);
}

void CollectPartManager::Update()
{
    for (auto itr = parts.begin(); itr != parts.end();)
    {
        //死んでたら殺す
        if (!(*itr)->isAlive) {
            itr = parts.erase(itr);
        }
        else {
            itr++;
        }
    }

    for (auto& part : parts)
    {
        part->Update();
    }

    zone.Update();

    //制作報酬を渡す
    CreateReward();

    ImGui();
}

void CollectPartManager::Draw()
{
    for (auto& part : parts)
    {
        part->Draw();
    }

    zone.Draw();
}

void CollectPartManager::Craete(const Vector3& spawnPos)
{
    parts.emplace_back();
    parts.back() = std::make_unique<CollectPart>();
    parts.back()->obj.mTransform.position = spawnPos;
}

bool CollectPartManager::GetAllowCreate()
{
    return CollectPartManager::GetInstance()->zone.GetPartsNum() >= requireCreateNum;
}

void CollectPartManager::CratePostDelete()
{
    //ゾーン内の数が要求数より少なければ全削除
    if (zone.GetPartsData()->size() <= requireCreateNum) {
        for (auto& part : *zone.GetPartsData())
        {
            part->SetIsAlive(false);
        }
    }
    //そうでなければ要求数分だけ減らす
    else {
        int32_t i = 0;
        for (auto& part : *zone.GetPartsData())
        {
            if (i >= 10)break;
            part->SetIsAlive(false);
            i++;
        }
    }

    zone.GodModeCreateStop();
}

void CollectPartManager::ImGui()
{
    if (RImGui::showImGui) {

        ImGui::SetNextWindowSize({ 400, 200 }, ImGuiCond_FirstUseEver);

        // デバッグモード //
        ImGui::Begin("部品集める");
        ImGui::Text("無敵モードへの遷移は");
        ImGui::Text("ゾーン内の数が10以上の状態で");
        ImGui::Text("パッドならRB,キーならEを長押しすると");
        ImGui::Text("10個消費して無敵モードへ遷移します");
        ImGui::Text("経過時間 %f", zone.hammerTimer.GetTimeRate());
        ImGui::Text("ゾーン内の数 %d", zone.GetPartsNum());
        ImGui::InputInt("一度に運べる数", &maxCarryingNum, 1);
        if (ImGui::TreeNode("調整項目_ゾーン")) {
            ImGui::InputFloat("回復量", &zone.healPower, 1.0f);
            ImGui::InputInt("作るのに必要な個数", &requireCreateNum);
            ImGui::InputFloat("作るまでにかかる時間", &zone.hammerTimer.maxTime_);
            ImGui::DragFloat3("位置", &zone.pos.x, 1.f);
            ImGui::DragFloat2("大きさ", &zone.scale.x, 1.f);
            ImGui::DragFloat("透明度", &zone.obj.mTuneMaterial.mColor.a, 0.1f);
        }
        if (ImGui::Button("セーブ")) {
            Parameter::Begin("zone");
            Parameter::Save("一度に運べる数", maxCarryingNum);
            Parameter::Save("作るのに必要な個数", requireCreateNum);
            Parameter::Save("作るまでにかかる時間", zone.hammerTimer.maxTime_);
            Parameter::Save("位置X", zone.pos.x);
            Parameter::Save("位置Y", zone.pos.y);
            Parameter::Save("位置Z", zone.pos.z);
            Parameter::Save("大きさX", zone.scale.x);
            Parameter::Save("大きさZ", zone.scale.y);
            Parameter::Save("透明度", zone.obj.mTuneMaterial.mColor.a);
            Parameter::Save("回復量", zone.healPower);
            Parameter::End();
        }

        ImGui::End();
    }
}

int32_t CollectPartManager::GetCarryingNum()
{
    int32_t temp = 0;
    for (auto& part : parts)
    {
        temp += part->IsCollected();
    }
    return temp;
}

void CollectPartManager::CreateReward()
{
    //10個溜まったら(後で制作状態も作る)
    if (CollectPartManager::GetInstance()->GetAllowCreate()) {

        //範囲内に居て
        if (ColPrimitive3D::CheckSphereToAABB(player->collider,
            CollectPartManager::GetInstance()->zone.aabbCol))
        {
            //ボタンを押しているなら
            if (RInput::GetKey(DIK_E) ||
                RInput::GetPadButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
            {
                //制作
                CollectPartManager::GetInstance()->zone.GodModeCreate();
            }
            else {
                CollectPartManager::GetInstance()->zone.GodModeCreateStop();
            }
        }

        //ゴッドモードへ
        if (CollectPartManager::GetInstance()->zone.GodModeCreateEnd()) {
            //player->SetIsGodmode(true);
            //回復に変更
            player->hp += zone.healPower;

            CollectPartManager::GetInstance()->CratePostDelete();
        }
    }
}

CollectPartManager* CollectPartManager::GetInstance()
{
    static CollectPartManager instance;
    return &instance;
}
