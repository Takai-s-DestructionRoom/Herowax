#include "SceneTrance.h"
#include "RWindow.h"

SceneTrance::SceneTrance()
{
	blackBack_ = std::make_unique<Sprite>();
	blackBack_->SetTexture(TextureManager::Load("./Resources/white2x2.png", "white2x2"));
	blackBack_->mMaterial.mColor = Color::kBlack;
	blackBack_->mTransform.scale = { (float)RWindow::GetWidth(),(float)RWindow::GetHeight(),0 };
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
		
		//前半が終わったなら後半スタート
		if (inTimer_.GetEnd() && outTimer_.GetStarted() == false)
		{
			outTimer_.Start();
			isSceneTrance_ = true;		//シーン切り替えてﾖｼ!!!
		}

		//透明度をタイマーによって変える
		blackBack_->mMaterial.mColor.a =
			inTimer_.GetTimeRate() - outTimer_.GetTimeRate();

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
	if (isSceneTranceNow_)
	{
		blackBack_->Draw();
	}
}

void SceneTrance::Start()
{
	if (isSceneTranceNow_ == false)
	{
		//SEならしたり//

		Init();
		isSceneTrance_ = false;		//まだだ、まだ切り替えるな
		isSceneTranceNow_ = true;
	}
}
