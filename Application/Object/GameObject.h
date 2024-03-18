#pragma once
#include "ModelObj.h"
#include "PaintableModelObj.h"
#include "ColPrimitive3D.h"

//モデルを持ち、画面に描画されるオブジェクトの基底クラス
class GameObject
{
public:
	bool isAlive;				//生きてるか否か

	//------------ その他 ------------//
	PaintableModelObj obj;		//オブジェクト
	ModelObj drawerObj;			//当たり判定描画オブジェクト
	
	ColPrimitive3D::Sphere collider;	//Sphereの当たり判定

protected:
	//型名を文字列として保存(本体の直接参照はエラーが検知できないので非推奨)
	std::string classname;

private:
	bool isViewCol = false;		//コライダーを表示するか

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
	//当たり判定表示フラグ取得
	bool GetIsViewCol()const;
	//自身のクラス名を取得(コンストラクタでclassnameに入れるの必須)
	std::string GetObjectName();

	// セッター //
	//座標設定
	void SetPos(const Vector3& pos) { obj.mTransform.position = pos; }
	//大きさ設定
	void SetScale(const Vector3& scale) { obj.mTransform.scale = scale; }
	//回転設定
	void SetRota(const Vector3& rota) { obj.mTransform.rotation = rota; }
	//生きてるかフラグ設定
	void SetIsAlive(bool alive) { isAlive = alive; }
	//当たり判定表示フラグ設定
	void SetIsViewCol(bool viewCol) { isViewCol = viewCol; };

protected:
	//当たり判定の更新
	void UpdateCollider();

private:
	//コピー禁止
	GameObject& operator=(const GameObject&) = delete;
};

