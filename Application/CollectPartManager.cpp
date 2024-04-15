#include "CollectPartManager.h"

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
}

void CollectPartManager::Update()
{
    for (auto& part : parts)
    {
        part->Update();
    }

    zone.Update();
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

CollectPartManager* CollectPartManager::GetInstance()
{
    static CollectPartManager instance;
    return &instance;
}
