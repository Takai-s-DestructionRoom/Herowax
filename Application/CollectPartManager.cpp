#include "CollectPartManager.h"
#include "RImGui.h"

void CollectPartManager::LoadResouces()
{
    //モデル読み込み
    Model::Load("./Resources/Model/hexagonBolt/hexagonBolt.obj", "hexagonBolt");
    Model::Load("./Resources/Model/gear/gear.obj", "gear");
    Model::Load("./Resources/Model/plusScrew/plusScrew.obj", "plusScrew");

    TextureManager::Load("./Resources/anvil.png", "anvil");
    TextureManager::Load("./Resources/hammer.png", "hammer");
}

void CollectPartManager::Init()
{
    for (auto& part : parts)
    {
        part->Init();
    }

    zone.Init();
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
        ImGui::InputInt("作るのに必要な個数", &requireCreateNum);
        ImGui::InputFloat("作るまでにかかる時間", &zone.hammerTimer.maxTime_);
        ImGui::DragFloat3("位置", &zone.pos.x,1.f);
        ImGui::DragFloat2("大きさ", &zone.scale.x,1.f);
        ImGui::DragFloat("透明度", &zone.obj.mTuneMaterial.mColor.a,0.1f);
    }

    ImGui::End();
}

CollectPartManager* CollectPartManager::GetInstance()
{
    static CollectPartManager instance;
    return &instance;
}
