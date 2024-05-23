#pragma once
#include "SimpleParticle.h"
#include <unordered_map>
#include "ParticleEditor.h"

// パーティクルの管理 //
// エミッターを登録して更新処理をまとめてかけてる //

class IEmitter3D;
class IEmitter2D;

class ParticleManager final
{
public:
	std::list<std::unique_ptr<IEmitter3D>> emitters_;			//パーティクルエミッター群
	std::list<std::unique_ptr<IEmitter2D>> emitters2D_;			//パーティクルエミッター群

	Vector3 playerPos;

public:
	//シングルトンインスタンス取得
	static ParticleManager* GetInstance();
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw();

	//リング状パーティクル追加(固有処理にしたかったらoverrideで上書きする)
	//life:秒数指定なので注意
	void AddRing(Vector3 emitPos, uint32_t addNum, float life, Color color, TextureHandle tex,
		float startRadius, float endRadius, float minScale, float maxScale,
		float minVeloY, float maxVeloY, Vector3 minRot = {}, Vector3 maxRot = {},
		float growingTimer = 0.f, float endScale = 0.f, bool isGravity = false, bool isBillboard = false);

	//パーティクルの追加
	void AddSimple(Vector3 emitPos, Vector3 emitScale, uint32_t addNum, float life,
		Color color, TextureHandle tex, float minScale, float maxScale, Vector3 minVelo, Vector3 maxVelo,
		float accelPower = 0.f, Vector3 minRot = {}, Vector3 maxRot = {},
		float growingTimer = 0.f, float endScale = 0.f, bool isGravity = false, bool isBillboard = false);

	//パーティクルの追加
	void AddSimple2D(Vector2 emitPos, Vector2 emitScale, uint32_t addNum, float life,
		Color color, TextureHandle tex, float minScale, float maxScale, Vector2 minVelo, Vector2 maxVelo,
		float accelPower = 0.f, float minRot = 0.f, float maxRot = 0.f,
		float growingTimer = 0.f, float endScale = 0.f, bool isGravity = false);

	//パーティクルの追加(外部データを使用)
	void AddSimple(Vector3 emitPos, std::string pDataHandle);
	void AddRing(Vector3 emitPos, std::string pDataHandle);
	void AddHoming(Vector3 emitPos, std::string pDataHandle, Vector3 targetPos = {}, bool useSlimeWax = false);

	//パーティクルの追加
	void AddHoming(Vector3 emitPos, Vector3 emitScale, uint32_t addNum, float life,
		Color color, TextureHandle tex, float minScale, float maxScale, Vector3 minVelo, Vector3 maxVelo, Vector3 targetPos,
		float accelPower = 0.f, Vector3 minRot = {}, Vector3 maxRot = {},
		float growingTimer = 0.f, float endScale = 0.f, bool isGravity = false, bool isBillboard = false,bool useSlimeWax = false);
	
	//パーティクルの追加
	void AddHoming2D(Vector2 emitPos, Vector2 emitScale, uint32_t addNum, float life,
		Color color, TextureHandle tex, float minScale, float maxScale, Vector2 minVelo, Vector2 maxVelo, Vector2 targetPos,
		float accelPower = 0.f, float minRot = 0.f, float maxRot = 0.f,
		float growingTimer = 0.f, float endScale = 0.f, bool isGravity = false);
	void SetPlayerPos(Vector3 pos) { playerPos = pos; }

	//パーティクルの追加
	void AddDirectional(Vector3 emitPos, std::string pDataHandle);

private:
	//コンストラクタ
	ParticleManager() = default;
	//コピー禁止
	ParticleManager(const ParticleManager&) = delete;
	//デストラクタ
	~ParticleManager() = default;
	//コピー禁止
	ParticleManager& operator=(const ParticleManager&) = delete;
};