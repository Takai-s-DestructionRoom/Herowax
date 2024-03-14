#include "ParticleManager.h"
#include "Particle3D.h"
#include "RImGui.h"

ParticleManager* ParticleManager::GetInstance()
{
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Init()
{
	for (auto& emitter : emitters_)
	{
		emitter.second->ClearParticles();
		emitter.second->Init();
	}

	for (auto& emitter : polygonEmitters_)
	{
		emitter.second->ClearParticles();
		emitter.second->Init();
	}
}

void ParticleManager::Update()
{
	for (auto& emitter : emitters_)
	{
		//パーティクルがあるときだけ更新処理回す
		if (emitter.second->GetParticlesDead() == false)
		{
			emitter.second->Update();
		}
	}

	for (auto& emitter : polygonEmitters_)
	{
		//パーティクルがあるときだけ更新処理回す
		if (emitter.second->GetParticlesDead() == false)
		{
			emitter.second->Update();
		}
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 150, 70 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	// パーティクル //
	ImGui::Begin("Particle", NULL, window_flags);

	for (auto& emitter : emitters_)
	{
		ImGui::Text("生存個体数:%d", emitter.second->GetParticlesSize());
	}

	ImGui::End();
#pragma endregion
}

void ParticleManager::Draw()
{
	for (auto& emitter : emitters_)
	{
		//パーティクルがあるときだけ描画処理回す
		if (emitter.second->GetParticlesDead() == false)
		{
			emitter.second->Draw();
		}
	}

	//ポリゴン
	for (auto& emitter : polygonEmitters_)
	{
		emitter.second->Draw();
	}
}
