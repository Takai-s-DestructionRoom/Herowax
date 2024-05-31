#include "WaxSceneTransition.h"
#include "RWindow.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include "MetaBall2D.h"
#include "Parameter.h"

WaxSceneTransition::WaxSceneTransition()
{
	MetaBall2DManager::GetInstance()->Init();
}

void WaxSceneTransition::Update()
{
	MetaBall2DManager::GetInstance()->Update();

	for (auto& brush : brushs)
	{
		brush.Update();
	}
	brushTimer.Roop();
	openTimer.Update();

	if (inClose)
	{
		if (brushTimer.GetRoopEnd() && (nowNum < num)) {
			brushs[nowNum].Close({
				brushs[nowNum].sprite.mTransform.position.x,
				brushs[nowNum].sprite.mTransform.position.y
				}, { 0.f,0.f },
				{ brushs[nowNum].sizeMax.x,
				brushs[nowNum].sizeMax.y });
			nowNum++;
		}

		if (nowNum >= num && brushs.back().GetCloseEnd()) {
			inProgress = false;
			mIsClosed = true;
		}
	}
	if (inOpen)
	{
		for (auto& brush : brushs)
		{
			brush.sprite.mMaterial.mColor.a = Easing::OutQuad(1.0f,0.0f, openTimer.GetTimeRate());
		}
		if (IsOpened()) {
			inProgress = false;
		}
	}
}

void WaxSceneTransition::Draw()
{
	MetaBall2DManager::GetInstance()->Draw();

	for (auto& brush : brushs)
	{
		brush.Draw();
	}
}

void WaxSceneTransition::Open()
{
	mIsClosed = false;
	inOpen = true;
	inClose = false;
	openTimer.Start();

	inProgress = true;
}

bool WaxSceneTransition::IsOpened()
{
	return openTimer.GetEnd();
}

void WaxSceneTransition::Close()
{
 	brushs.clear();
	
	std::map<std::string, std::string> extract = Parameter::Extract("WaxSceneTransition");
	num = (int32_t)Parameter::GetParam(extract, "num", 0.f);

	Color colors[4] =
	{
		Color::kWaxColor,
		Color(0xE99730),
		Color(0xE67C33),
		Color(0xCC7F59)
	};

	for (int32_t i = 0; i < num; i++)
	{
		brushs.emplace_back();
		brushs.back().Init();
		std::string string = "pos_";
		string += std::to_string(i);
		brushs.back().sprite.mTransform.position = Parameter::GetVector3Data(extract, string,{ 0.0f,0.0f,0.0f });
		brushs.back().sprite.mTransform.position.z = 114514;
		string = "scale_";
		string += std::to_string(i);
		brushs.back().sprite.mTransform.scale = { 0,0,0 };
		brushs.back().sizeMax = Parameter::GetVector3Data(extract, string, { 0.0f,0.0f,0.0f });
		
		int32_t j = i;
		if (j >= 4)j -= 4;
		brushs.back().sprite.mMaterial.mColor = colors[j];
	}

	inProgress = true;
	inOpen = false;
	inClose = true;

	nowNum = 0;
}

bool WaxSceneTransition::IsClosed()
{
	return mIsClosed;
}

bool WaxSceneTransition::InProgress()
{
	return inProgress;
}
