#include "NumDrawer.h"
#include "Texture.h"
#include "RImGui.h"
#include "Parameter.h"

void NumDrawer::LoadResource()
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
}

void NumDrawer::Init(int32_t maxDigit)
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
	};
	//全部初期化して値は0に
	digit.clear();
	numberSprites.clear();
	for (size_t i = 0; i < (size_t)maxDigit; i++)
	{
		digit.emplace_back();
		numberSprites.emplace_back();
		numberSprites[i].Init();
		numberSprites[i].SetTexture(numberTextures[0]);
	}

	std::map<std::string, std::string> extract = Parameter::Extract("NumDrawer");
	basePos = Parameter::GetVector3Data(extract, "位置", basePos);
	baseScale = Parameter::GetVector3Data(extract, "大きさ", baseScale);
	interX = Parameter::GetParam(extract, "間隔", interX);
}

void NumDrawer::Update()
{
	for (size_t i = 0; i < numberSprites.size(); i++)
	{
		numberSprites[i].mTransform.position = basePos;
		numberSprites[i].mTransform.position.x = basePos.x + interX * i;
		numberSprites[i].mTransform.scale = baseScale;

		numberSprites[i].mTransform.UpdateMatrix();
		numberSprites[i].TransferBuffer();
	}
}

void NumDrawer::Draw()
{
	for (size_t i = 0; i < numberSprites.size(); i++)
	{
		numberSprites[i].Draw();
	}
}

void NumDrawer::Imgui(const std::string& title)
{
	if (RImGui::showImGui) {
		ImGui::Begin(title.c_str());
		ImGui::DragFloat3("位置", &basePos.x);
		ImGui::DragFloat3("大きさ", &baseScale.x, 0.1f);
		ImGui::DragFloat("間隔", &interX);

		if (ImGui::Button("セーブ")) {
			Parameter::Begin("NumDrawer");
			Parameter::SaveVector3("位置", basePos);
			Parameter::SaveVector3("大きさ", baseScale);
			Parameter::Save("間隔", interX);
			Parameter::End();
		}

		ImGui::End();
	}
}

void NumDrawer::SetNum(int32_t num)
{
	int32_t result = num;	//影響出ないように別の変数に格納
	int32_t divide = (int32_t)pow(10, numberSprites.size() - 1);	//最大桁数求める

	result = Util::Clamp(result,0, (int32_t)pow(10, numberSprites.size() - 1));

	//桁数分回す
	for (size_t i = 0; i < numberSprites.size(); i++)
	{
		digit[i] = result / divide;  //各桁ごとの値を順に格納
		result = result % divide;     //格納した桁はバイバイ
		divide /= 10;                 //次の計算のために桁落とす
		//対応したテクスチャを設定しなおす
		numberSprites[i].SetTexture(numberTextures[digit[i]]);
	}
}