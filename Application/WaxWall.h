#pragma once
#include "GameObject.h"
#include "Easing.h"

//ボタン押すたびに生成される
class WaxWall : public GameObject
{
public:
	void Init()override;
	void Update()override;
	void Draw()override;

	bool GetParry();
	bool GetLeakOutMode();
	
	/// <summary>
	/// 継続確認
	/// </summary>
	/// <param name="remainWax">減らした後のロウの残量を入れる</param>
	/// <returns></returns>
	bool ContinueCheck(int32_t remainWax);

	bool StartCheck(int32_t remainWax);

	void Start();
	void End();

public:
	const int32_t START_CHECK_WAXNUM = 5;
	const int32_t CONTINUE_CHECK_WAXNUM = 1;

	//パリィ判定
	Easing::EaseTimer parryTimer = 0.1f;

private:
	bool endFlag = false;

	//この秒数のたびにロウが出現する
	Easing::EaseTimer leakOutTimer = 0.5f;

	Vector3 baseScale = { 1,1,1 };
};

