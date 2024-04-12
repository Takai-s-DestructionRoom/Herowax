#include "ParticleManager.h"
#include "Particle3D.h"
#include "RImGui.h"
#include "RingParticle.h"
#include "SimpleParticle.h"
#include "HomingParticle.h"

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
			emit->get()->SetTargetPos(playerPos);
			emit->get()->Update();
			++emit;
		}
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 150, 70 }, ImGuiCond_FirstUseEver);

	// パーティクル //
	ImGui::Begin("Particle");

	size_t particleSize = 0;
	for (auto& emitter : emitters_)
	{
		particleSize += emitter->GetParticlesSize();
	}

	ImGui::Text("生存個体数:%d", particleSize);

	ImGui::End();
#pragma endregion
}

void ParticleManager::Draw()
{
	for (auto& emitter : emitters_)
	{
		emitter->Draw();
	}
}

void ParticleManager::AddRing(
	Vector3 emitPos, uint32_t addNum, float life, Color color, TextureHandle tex,
	float startRadius, float endRadius, float minScale, float maxScale,
	float minVeloY, float maxVeloY, Vector3 minRot, Vector3 maxRot,
	float growingTimer, float endScale, bool isGravity, bool isBillboard)
{
	emitters_.emplace_back();
	emitters_.back() = std::make_unique<RingParticle>();
	emitters_.back()->SetPos(emitPos);
	emitters_.back()->AddRing(
		addNum, life, color, tex, startRadius, endRadius, minScale, maxScale,
		minVeloY, maxVeloY, minRot, maxRot, growingTimer, endScale, isGravity, isBillboard);
}

void ParticleManager::AddSimple(
	Vector3 emitPos, Vector3 emitScale, uint32_t addNum, float life,
	Color color, TextureHandle tex, float minScale, float maxScale,
	Vector3 minVelo, Vector3 maxVelo, float accelPower,
	Vector3 minRot, Vector3 maxRot,
	float growingTimer, float endScale, bool isGravity, bool isBillboard)
{
	emitters_.emplace_back();
	emitters_.back() = std::make_unique<SimpleParticle>();
	emitters_.back()->SetPos(emitPos);
	emitters_.back()->SetScale(emitScale);
	emitters_.back()->Add(
		addNum, life, color, tex, minScale, maxScale,
		minVelo, maxVelo, accelPower, minRot, maxRot,
		growingTimer, endScale, isGravity, isBillboard);
}

void ParticleManager::AddSimple(Vector3 emitPos, std::string pDataHandle)
{
	emitters_.emplace_back();
	emitters_.back() = std::make_unique<SimpleParticle>();
	emitters_.back()->SetPos(emitPos);
	SimplePData pdata = ParticleEditor::LoadSimple(pDataHandle);
	emitters_.back()->SetScale(pdata.emitScale);
	emitters_.back()->Add(
		pdata.addNum, pdata.life, pdata.color, pdata.tex, pdata.minScale, pdata.maxScale,
		pdata.minVelo, pdata.maxVelo, pdata.accelPower, pdata.minRot, pdata.maxRot,
		pdata.growingTimer, pdata.endScale, pdata.isGravity, pdata.isBillboard);
}

void ParticleManager::AddHoming(Vector3 emitPos, Vector3 emitScale, uint32_t addNum, float life, Color color, TextureHandle tex, float minScale, float maxScale, Vector3 minVelo, Vector3 maxVelo, float accelPower, Vector3 minRot, Vector3 maxRot, float growingTimer, float endScale, bool isGravity, bool isBillboard)
{
	emitters_.emplace_back();
	emitters_.back() = std::make_unique<HomingParticle>();
	emitters_.back()->SetPos(emitPos);
	emitters_.back()->SetScale(emitScale);
	emitters_.back()->Add(
		addNum, life, color, tex, minScale, maxScale,
		minVelo, maxVelo, accelPower, minRot, maxRot,
		growingTimer, endScale, isGravity, isBillboard);
}
