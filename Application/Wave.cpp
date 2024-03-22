#include "Wave.h"
#include "SpawnerManager.h"
#include "LevelLoader.h"
#include "Level.h"

Wave::Wave() :
	startPostponement(10),
	endPostponement(10),
	nowLevel(0),
	waveTimer(0.0f),
	transitionTimer(1.0f)
{
	
}

void Wave::Load()
{
	LevelLoader::Get()->Load("./Level/test.json", "test");

	levelFiles.push_back("test");
	levelFiles.push_back("test");
	levelFiles.push_back("test");
}

void Wave::Restart()
{
	isStartPost = false;
	isEndPost = false;
	isEndMain = false;
	waveTimer.Start();
}

void Wave::SetWaveTime()
{
	spawnerMaxTime = 0;
	for (auto& spawner : SpawnerManager::GetInstance()->spawners)
	{
		float spawnerTime = spawner.lifeTimer.maxTime_;
		if (spawnerMaxTime <= spawnerTime) {
			spawnerMaxTime = spawnerTime;
		}
	}

	waveTimer.maxTime_ = spawnerMaxTime + startPostponement + endPostponement;
}

void Wave::Update()
{
	waveTimer.Update();
	transitionTimer.Update();

	//開始猶予を超えたら
	if (GetStartPostponement()) {
		//演出
		
	}

	//終了時間の10秒前になったら
	if (GetEndMainGame(10.f)) {
		//カウントダウン演出

	}

	//終了猶予を超えたら
	if (GetEndPostponement()) {
		//演出


		//遷移猶予開始
		transitionTimer.Start();
	}

	//飛び上がり

	//半分まで進んだタイミングで遷移
	if (transitionTimer.GetTimeRate() > 0.5f) {
		//次のウェーブ読み込み
		nowLevel++;
		nowLevel = Util::Clamp(nowLevel, 0, levelFiles.size());
		//ここのハンドルをレベルに読み込ませる
		Level::Get()->Extract(levelFiles[nowLevel]);
	}

	if (transitionTimer.GetEnd()) {
		transitionTimer.Reset();
	}
}

bool Wave::GetStartPostponement()
{
	if (isStartPost)return false;
	isStartPost = true;
	return waveTimer.nowTime_ < startPostponement;
}

bool Wave::GetEndPostponement()
{
	if (isEndPost)return false;
	isEndPost = true;
	return waveTimer.nowTime_ < 
		endPostponement + spawnerMaxTime;
}

bool Wave::GetEndMainGame(float time)
{
	if (isEndMain)return false;
	isEndMain = true;
	return waveTimer.nowTime_ >= 
		waveTimer.maxTime_ - endPostponement - time;
}
