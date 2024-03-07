#pragma once
#include "ModelObj.h"
#include "ColPrimitive3D.h"

//モデルを持ち、画面に描画されるオブジェクトの基底クラス
class GameObject
{
public:
	bool isAlive;				//生きてるか否か

	//------------ その他 ------------//
	ModelObj obj;				//オブジェクト
	
	ColPrimitive3D::Sphere collider;	//Sphereの当たり判定

public:
	GameObject();
	virtual ~GameObject() {};
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	// ゲッター //
	//座標取得
	Vector3 GetPos()const { return obj.mTransform.position; }
	//大きさ取得
	Vector3 GetScale()const { return obj.mTransform.scale; }
	//生きてるかフラグ取得
	bool GetIsAlive()const { return isAlive; }

	// セッター //
	//座標設定
	void SetPos(const Vector3& pos) { obj.mTransform.position = pos; }
	//大きさ設定
	void SetScale(const Vector3& scale) { obj.mTransform.scale = scale; }
	//生きてるかフラグ設定
	void SetIsAlive(bool alive) { isAlive = alive; }

protected:
	//当たり判定の更新
	void UpdateCollider();

private:
	//コピー禁止
	GameObject& operator=(const GameObject&) = delete;
};

