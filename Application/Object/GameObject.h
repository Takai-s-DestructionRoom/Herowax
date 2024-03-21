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
	ModelObj drawerObj;			//当たり判定描画オブジェクト
	
	ColPrimitive3D::Sphere collider;	//Sphereの当たり判定

public:
	GameObject();
	virtual ~GameObject() {};
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	//当たり判定の描画
	void DrawCollider();

	// ゲッター //
	//座標取得
	Vector3 GetPos()const;
	//大きさ取得
	Vector3 GetScale()const;
	//生きてるかフラグ取得
	bool GetIsAlive()const;
	//中心点取得(座標 + 高さ / 2)
	Vector3 GetCenterPos(){
		return Vector3(
			obj.mTransform.position.x,
			obj.mTransform.position.y + obj.mTransform.scale.y,
			obj.mTransform.position.z);
	}

	// セッター //
	//座標設定
	void SetPos(const Vector3& pos) { obj.mTransform.position = pos; }
	//大きさ設定
	void SetScale(const Vector3& scale) { obj.mTransform.scale = scale; }
	//回転設定
	void SetRota(const Vector3& rota) { obj.mTransform.rotation = rota; }
	//生きてるかフラグ設定
	void SetIsAlive(bool alive) { isAlive = alive; }

protected:
	//当たり判定の更新
	void UpdateCollider();

private:
	//コピー禁止
	GameObject& operator=(const GameObject&) = delete;
};

