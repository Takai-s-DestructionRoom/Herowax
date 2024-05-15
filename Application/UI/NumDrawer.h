#pragma once
#include "Sprite.h"
#include <array>
#include <vector>
#include "Easing.h"

	/// <summary>
	/// 汎用タイマーUI
	/// 9:59まで対応
	/// </summary>
class NumDrawer
{
	public:
		static void LoadResource();

		//maxDigit:最大桁数(表示桁数)
		void Init(int32_t maxDigit);
		void Update();
		void Draw();
		~NumDrawer() {};

		void Imgui(const std::string& title = "数字描画機能");

		void SetNum(int32_t num);

		//図る時間を設定
		void SetMaxTime(float maxTime_) { timer.maxTime_ = maxTime_; };

	protected:
		std::vector<Sprite> numberSprites;

		std::array<std::string, 10> numberTextures;

		Vector3 basePos;
		Vector3 baseScale;

		float interX = 0;	//間隔

		int32_t number = 0;
		std::vector<int32_t> digit;	//各桁の数字格納用

		Easing::EaseTimer timer = 60.f;
};
