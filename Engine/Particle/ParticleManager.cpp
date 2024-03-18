#include "ParticleManager.h"
#include "Particle3D.h"
#include "RImGui.h"
#include "RingParticle.h"
#include "SimpleParticle.h"

ParticleManager* ParticleManager::GetInstance()
{
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Init()
{
	for (auto& emitter : emitters_)
	{
		emitter->ClearParticles();
		emitter->Init();
	}
}

void ParticleManager::Update()
{
	//エミッター群のイテレーター
	std::list<std::unique_ptr<IEmitter3D>>::iterator emit = emitters_.begin();

	//イテレーターが最後になるまで回す
	while (emit != emitters_.end()) {
		//パーティクルがもうなくなってたら
		if (emit->get()->GetParticlesDead()) {
			emit = emitters_.erase(emit);	//殺す
		}
		//パーティクルが生きてるなら
		else
		{
			//更新処理して次の要素へ
			emit->get()->Update();
			++emit;
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
		ImGui::Text("生存個体数:%d", emitter->GetParticlesSize());
	}

	ImGui::End();
#pragma endregion
}

void ParticleManager::Draw()
{
	for (auto& emitter : emitters_)
	{
		//パーティクルがあるときだけ描画処理回す
		if (emitter->GetParticlesDead() == false)
		{
			emitter->Draw();
		}
	}
}

void ParticleManager::AddRing(Vector3 emitPos, uint32_t addNum, float life, Color color,
	float startRadius, float endRadius, float minScale, float maxScale,
	float minVeloY, float maxVeloY, Vector3 minRot, Vector3 maxRot,
	float growingTimer, bool isGravity, bool isBillboard)
{
	emitters_.emplace_back();
	emitters_.back() = std::make_unique<RingParticle>();
	emitters_.back()->SetPos(emitPos);
	emitters_.back()->AddRing(
		addNum, life, color, startRadius, endRadius, minScale, maxScale,
		minVeloY, maxVeloY, minRot, maxRot, growingTimer, isGravity, isBillboard);
}

void ParticleManager::AddSimple(Vector3 emitPos, Vector3 emitScale, uint32_t addNum, float life,
	Color color, float minScale, float maxScale, Vector3 minVelo, Vector3 maxVelo,
	float accelPower, Vector3 minRot, Vector3 maxRot,
	float growingTimer, bool isGravity, bool isBillboard)
{
	emitters_.emplace_back();
	emitters_.back() = std::make_unique<SimpleParticle>();
	emitters_.back()->SetPos(emitPos);
	emitters_.back()->SetScale(emitScale);
	emitters_.back()->Add(
		addNum, life, color, minScale, maxScale,
		minVelo, maxVelo, accelPower, minRot, maxRot,
		growingTimer, isGravity, isBillboard);
}
