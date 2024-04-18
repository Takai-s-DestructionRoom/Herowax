#include "SceneTrance.h"

SceneTrance::SceneTrance()
{
	blackBack_ = std::make_unique<Sprite>();
	blackBack_->SetTexture(TextureManager::Load("./Resources/white2x2.png", "white2x2"));
	blackBack_->mMaterial.mColor = Color::kBlack;
}

SceneTrance* SceneTrance::GetInstance()
{
	static SceneTrance instance;
	return &instance;
}

void SceneTrance::Init()
{
	isSceneTrance_ = false;
	isSceneTranceNow_ = false;

	inTimer_.Reset();
	outTimer_.Reset();
}

void SceneTrance::Update()
{
	if (isSceneTranceNow_)
	{
		//タイマー更新
		inTimer_.Update();
		outTimer_.Update();

		//まだタイマースタートしてないなら起動
		if (inTimer_.GetStarted() == false)
		{
			inTimer_.Start();
		}

		//前半のタイマーが動いてる時
		if (inTimer_.GetRun())
		{
			//暗幕で覆う
		}

		//前半が終わったなら後半スタート
		if (inTimer_.GetEnd() && outTimer_.GetStarted() == false)
		{
			outTimer_.Start();
			isSceneTrance_ = true;		//シーン切り替えてﾖｼ!!!
		}

		//後半のタイマーが動いてる時
		if (outTimer_.GetRun())
		{
			//暗幕晴らす
		}

		//終わり
		if (outTimer_.GetEnd())
		{
			isSceneTrance_ = false;
			isSceneTranceNow_ = false;
		}
	}

	blackBack_->mTransform.UpdateMatrix();
	blackBack_->TransferBuffer();
}

void SceneTrance::Draw()
{
}

void SceneTrance::Start()
{
}
