#include "ParticleManager.h"
#include "Particle3D.h"
#include "RImGui.h"
#include "RingParticle.h"
#include "SimpleParticle.h"
#include "SimpleParticle2D.h"
#include "HomingParticle.h"
#include "HomingParticle2D.h"

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

	for (auto& emitter2D : emitters2D_)
	{
		emitter2D->ClearParticles();
		emitter2D->Init();
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

	//エミッター群のイテレーター
	std::list<std::unique_ptr<IEmitter2D>>::iterator emit2D = emitters2D_.begin();

	//イテレーターが最後になるまで回す
	while (emit2D != emitters2D_.end()) {
		//パーティクルがもうなくなってたら
		if (emit2D->get()->GetParticlesDead()) {
			emit2D = emitters2D_.erase(emit2D);	//殺す
		}
		//パーティクルが生きてるなら
		else
		{
			//更新処理して次の要素へ
			emit2D->get()->Update();
			++emit2D;
		}
	}

#pragma region ImGui
	if (RImGui::showImGui)
	{

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
	}
#pragma endregion
}

void ParticleManager::Draw()
{
	for (auto& emitter : emitters_)
	{
		emitter->Draw();
	}

	for (auto& emitter2D : emitters2D_)
	{
		emitter2D->Draw();
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

void ParticleManager::AddSimple2D(
	Vector2 emitPos, Vector2 emitScale, uint32_t addNum, float life,
	Color color, TextureHandle tex, float minScale, float maxScale,
	Vector2 minVelo, Vector2 maxVelo, float accelPower,
	float minRot, float maxRot, float growingTimer, float endScale, bool isGravity)
{
	emitters2D_.emplace_back();
	emitters2D_.back() = std::make_unique<SimpleParticle2D>();
	emitters2D_.back()->SetPos(emitPos);
	emitters2D_.back()->SetScale(emitScale);
	emitters2D_.back()->Add(
		addNum, life, color, tex, minScale, maxScale,
		minVelo, maxVelo, accelPower, minRot, maxRot,
		growingTimer, endScale, isGravity);
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
		pdata.growingTimer, pdata.endScale, pdata.isGravity, pdata.isBillboard,pdata.rejectRadius);
}

void ParticleManager::AddRing(Vector3 emitPos, std::string pDataHandle)
{
	emitters_.emplace_back();
	emitters_.back() = std::make_unique<RingParticle>();
	emitters_.back()->SetPos(emitPos);
	RingPData pdata = ParticleEditor::LoadRing(pDataHandle);
	emitters_.back()->AddRing(
		pdata.addNum, pdata.life, pdata.color, pdata.tex, pdata.startRadius, pdata.endRadius,
		pdata.minScale, pdata.maxScale, pdata.minVeloY, pdata.maxVeloY, pdata.minRot, pdata.maxRot,
		pdata.growingTimer, pdata.endScale, pdata.isGravity, pdata.isBillboard);
}

void ParticleManager::AddHoming(Vector3 emitPos, std::string pDataHandle, Vector3 targetPos, bool useSlimeWax)
{
	HomingPData pdata = ParticleEditor::LoadHoming(pDataHandle);
	emitters_.emplace_back();

	//エミッター座標に吸い寄せられるやつなら
	if (pdata.isTargetEmitter)
	{
		emitters_.back() = std::make_unique<HomingParticle>(emitPos, useSlimeWax);
	}
	//それ以外なら指定されたターゲット
	else
	{
		emitters_.back() = std::make_unique<HomingParticle>(targetPos, useSlimeWax);
	}
	emitters_.back()->SetPos(emitPos);
	emitters_.back()->SetScale(pdata.emitScale);
	emitters_.back()->Add(
		pdata.addNum, pdata.life, pdata.color, pdata.tex, pdata.minScale, pdata.maxScale,
		pdata.minVelo, pdata.maxVelo, pdata.accelPower, pdata.minRot, pdata.maxRot,
		pdata.growingTimer, pdata.endScale, pdata.isGravity, pdata.isBillboard,pdata.rejectRadius);
}

void ParticleManager::AddHoming(
	Vector3 emitPos, Vector3 emitScale, uint32_t addNum, float life, Color color, TextureHandle tex,
	float minScale, float maxScale, Vector3 minVelo, Vector3 maxVelo, Vector3 targetPos,
	float accelPower, Vector3 minRot, Vector3 maxRot,
	float growingTimer, float endScale, bool isGravity, bool isBillboard, bool useSlimeWax)
{
	emitters_.emplace_back();
	emitters_.back() = std::make_unique<HomingParticle>(targetPos, useSlimeWax);
	emitters_.back()->SetPos(emitPos);
	emitters_.back()->SetScale(emitScale);
	emitters_.back()->Add(
		addNum, life, color, tex, minScale, maxScale,
		minVelo, maxVelo, accelPower, minRot, maxRot,
		growingTimer, endScale, isGravity, isBillboard);
}

void ParticleManager::AddHoming2D(Vector2 emitPos, Vector2 emitScale, uint32_t addNum, float life, Color color, TextureHandle tex, float minScale, float maxScale, Vector2 minVelo, Vector2 maxVelo, Vector2 targetPos, float accelPower, float minRot, float maxRot, float growingTimer, float endScale, bool isGravity)
{
	emitters2D_.emplace_back();
	emitters2D_.back() = std::make_unique<HomingParticle2D>(targetPos);
	emitters2D_.back()->SetPos(emitPos);
	emitters2D_.back()->SetScale(emitScale);
	emitters2D_.back()->Add(
		addNum, life, color, tex, minScale, maxScale,
		minVelo, maxVelo, accelPower, minRot, maxRot,
		growingTimer, endScale, isGravity);
}