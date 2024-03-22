#pragma once
#include "SimpleParticle.h"
#include <unordered_map>

// パーティクルの管理 //
// エミッターを登録して更新処理をまとめてかけてる //

class IEmitter3D;

class ParticleManager final
{
public:
	std::list<std::unique_ptr<IEmitter3D>> emitters_;			//パーティクルエミッター群
	std::list<std::unique_ptr<IEmitter3D>> polygonEmitters_;	//ポリゴンパーティクルエミッター群

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

	////エミッターをunordered_mapに追加
	//void AddEmitter(IEmitter3D emitter, const std::string& key)
	//{
	//	//同じ名前のやつが登録されてたら追加せずに処理終了
	//	for (auto& emit : emitters_)
	//	{
	//		if (emit.first == key)
	//		{
	//			return;
	//		}
	//	}
	//	emitter->Init();					//初期化してから登録
	//	emitters_.emplace(std::make_pair(key, emitter));
	//}
	////エミッターをunordered_mapから削除
	//void EraseEmitter(const std::string& key)
	//{
	//	emitters_.erase(key);
	//}
	////有効フラグを設定
	//void SetIsActive(const std::string& key, bool isActive)
	//{
	//	emitters_[key]->SetIsActive(isActive);
	//}

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