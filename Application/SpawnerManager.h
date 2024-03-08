#pragma once
#include "Singleton.h"
#include "EnemySpawner.h"
#include <list>

class SpawnerManager final : public Singleton
{
public:
	static SpawnerManager* GetInstance();

	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw();
	//指定座標に配置
	void Create(const Vector3& position, const Vector3& scale,const Vector3& rotation);

public:
	std::list<EnemySpawner> spawners;	//スポナーども
	//今は見えるオブジェクトとしておいているけど、
	//今後見えなくなったり、移動したりするかも？
};

