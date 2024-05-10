#include "TimerUI.h"
#include "Texture.h"
#include "RImGui.h"
#include "Parameter.h"

void ITimerUI::LoadResource()
{
	TextureManager::Load("./Resources/UI/num_0.png", "num_0");
	TextureManager::Load("./Resources/UI/num_1.png", "num_1");
	TextureManager::Load("./Resources/UI/num_2.png", "num_2");
	TextureManager::Load("./Resources/UI/num_3.png", "num_3");
	TextureManager::Load("./Resources/UI/num_4.png", "num_4");
	TextureManager::Load("./Resources/UI/num_5.png", "num_5");
	TextureManager::Load("./Resources/UI/num_6.png", "num_6");
	TextureManager::Load("./Resources/UI/num_7.png", "num_7");
	TextureManager::Load("./Resources/UI/num_8.png", "num_8");
	TextureManager::Load("./Resources/UI/num_9.png", "num_9");
	TextureManager::Load("./Resources/UI/num_colon.png", "num_colon");
}

void ITimerUI::Imgui(const std::string& title)
{
	if (RImGui::showImGui) {
		ImGui::Begin(title.c_str());
		ImGui::Text("minute:%d",minute);
		ImGui::Text("tenSecond:%d",tenSecond);
		ImGui::Text("oneSecond:%d",oneSecond);
		ImGui::DragFloat("タイマーの最大の時間", &timer.maxTime_);
		ImGui::DragFloat3("位置",&basePos.x);
		ImGui::DragFloat3("大きさ",&baseScale.x,0.1f);
		ImGui::DragFloat("間隔", &interX);

		if (ImGui::Button("初期化して開始")) {
			Start();
		}
		if (ImGui::Button("途中から開始")) {
			ReStart();
		}
		if (ImGui::Button("停止")) {
			Stop();
		}

		if (ImGui::Button("セーブ")) {
			Parameter::Begin("TimerUI");
			Parameter::SaveVector3("位置", basePos);
			Parameter::SaveVector3("大きさ", baseScale);
			Parameter::Save("間隔", interX);
			Parameter::End();
		}

		ImGui::End();
	}
}

void ITimerUI::Start()
{
	timer.nowTime_ = timer.maxTime_;
	timer.ReverseStart();
}

void ITimerUI::ReStart()
{
	timer.ReverseStart();
}

void ITimerUI::Stop()
{
	timer.Stop();
}

bool ITimerUI::GetRun()
{
	return timer.GetReverse();
}

bool ITimerUI::GetEnd()
{
	return timer.GetReverseEnd();
}

void TimerUI::Init()
{
	numberTextures = {
		"num_0",
		"num_1",
		"num_2",
		"num_3",
		"num_4",
		"num_5",
		"num_6",
		"num_7",
		"num_8",
		"num_9",
		"num_colon",
	};
	for (int32_t i = 0; i < 4; i++)
	{
		numberSprites[i].Init();
	}
	numberSprites[0].SetTexture(numberTextures[0]);
	numberSprites[1].SetTexture(numberTextures[10]);	//10はコロン
	numberSprites[2].SetTexture(numberTextures[0]);
	numberSprites[3].SetTexture(numberTextures[0]);

	//タイマーを逆で開始
	//Start();

	std::map<std::string, std::string> extract = Parameter::Extract("TimerUI");
	basePos = Parameter::GetVector3Data(extract, "位置", basePos);
	baseScale = Parameter::GetVector3Data(extract, "大きさ", baseScale);
	interX = Parameter::GetParam(extract, "間隔", interX);
}

void TimerUI::Update()
{
	timer.Update();

	float base = 60.f;

	//60で割り、int型に格納して、残った値が分の値
	minute = (uint32_t)(timer.nowTime_ / (uint32_t)base);
	//分をnowTime_に合わせた形式
	oneSecond = (uint32_t)timer.nowTime_ - minute * (uint32_t)base;
	//上だけ取り出す
	int32_t temp = oneSecond;
	oneSecond %= 10;
	//下だけ取り出す
	tenSecond = temp - oneSecond;
	tenSecond /= 10;

	numberSprites[0].SetTexture(numberTextures[minute]);
	numberSprites[2].SetTexture(numberTextures[tenSecond]);
	numberSprites[3].SetTexture(numberTextures[oneSecond]);

	for (int32_t i = 0; i < 4; i++)
	{
		numberSprites[i].mTransform.position = basePos;
		numberSprites[i].mTransform.position.x = basePos.x + interX * i;
		numberSprites[i].mTransform.scale = baseScale;

		numberSprites[i].mTransform.UpdateMatrix();
		numberSprites[i].TransferBuffer();
	}
}

void TimerUI::Draw()
{
	for (int32_t i = 0; i < 4; i++)
	{
		numberSprites[i].Draw();
	}
}