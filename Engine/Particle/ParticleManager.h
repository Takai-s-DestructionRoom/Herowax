#pragma once
#include "SimpleParticle.h"
#include <unordered_map>

// パーティクルの管理 //
// エミッターを登録して更新処理をまとめてかけてる //

class IEmitter3D;

class ParticleManager final
{
public:
	std::unordered_map<std::string, IEmitter3D*> emitters_;			//パーティクルエミッター群
	std::unordered_map<std::string, IEmitter3D*> polygonEmitters_;	//ポリゴンパーティクルエミッター群

public:
	//シングルトンインスタンス取得
	static ParticleManager* GetInstance();
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw();

	//エミッターをunordered_mapに追加
	void AddEmitter(IEmitter3D* emitter, const std::string& key)
	{
		//同じ名前のやつが登録されてたら追加せずに処理終了
		for (auto& emit : emitters_)
		{
			if (emit.first == key)
			{
				return;
			}
		}
		emitter->Init();					//初期化してから登録
		emitters_.emplace(std::make_pair(key, emitter));
	}
	//エミッターをunordered_mapから削除
	void EraseEmitter(const std::string& key)
	{
		emitters_.erase(key);
	}
	//有効フラグを設定
	void SetIsActive(const std::string& key, bool isActive)
	{
		emitters_[key]->SetIsActive(isActive);
	}

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