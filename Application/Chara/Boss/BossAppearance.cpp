#include "BossAppearance.h"
#include "Boss.h"

BossAppearance::BossAppearance()
{
	priority = 0;
}

void BossAppearance::Update(Boss* boss)
{
	boss->SetStateStr("Appearance");	//ステートがわかるように設定しとく

	//カメラの向く
	Vector3 aVec = Camera::sNowCamera->mViewProjection.mEye - boss->obj.mTransform.position;
	aVec.Normalize();
	aVec.y = 0;

	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(aVec);

	//euler軸へ変換
	boss->obj.mTransform.rotation = aLookat.ToEuler();

	for (size_t i = 0; i < boss->parts.size(); i++)
	{
		//基準座標に回転をかけて親子っぽくしてる
		boss->parts[i].obj.mTransform.position = boss->parts[i].oriPos * Matrix4::RotationY(aLookat.ToEuler().y);
		boss->parts[i].obj.mTransform.rotation = aLookat.ToEuler();
	}
}
