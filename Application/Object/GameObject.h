#pragma once
#include "ModelObj.h"
#include "ColPrimitive3D.h"
#include "PaintableModelObj.h"
#include "SRConstBuffer.h"
#include "Float4.h"
#include "Camera.h"

struct BlightBuffer
{
	Float4 addColor;
};

//モデルを持ち、画面に描画されるオブジェクトの基底クラス
class GameObject
{
public:
	bool isAlive;				//生きてるか否か

	//------------ その他 ------------//
	PaintableModelObj obj;				//オブジェクト
	ModelObj drawerObj;					//当たり判定描画オブジェクト
	
	ColPrimitive3D::Sphere collider;	//Sphereの当たり判定
	float colliderSize;					//当たり判定のサイズ

	Color blightColor = {0,0,0,0};

	static RootSignature GameObjectRootSignature();
	static GraphicsPipeline GameObjectPipeLine();

protected:
	bool isDrawCollider = false;	//当たり判定描画の切り替え

private:
	SRConstBuffer<BlightBuffer> AddColor;	//加算する色

public:
	GameObject();
	virtual ~GameObject() {};
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	void GameObjectTransferBuffer(const ViewProjection& view);
	void ObjDraw();	//自身が持っているobjのDrawを専用のシェーダーで描画するもの

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
	Vector3 GetFrontVec();
	bool GetIsDrawCollider() { return isDrawCollider; }

	void SetDrawCollider(bool isCollider);

	// セッター //
	//座標設定
	void SetPos(const Vector3& pos) { obj.mTransform.position = pos; }
	//大きさ設定
	void SetScale(const Vector3& scale) { obj.mTransform.scale = scale; }
	//回転設定
	void SetRota(const Vector3& rota) { obj.mTransform.rotation = rota; }
	//生きてるかフラグ設定
	void SetIsAlive(bool alive) { isAlive = alive; }
	//コライダーの大きさ設定(元の大きさからの比率)
	void SetColSize(float size) { colliderSize = size; }

protected:
	void InitCollider();
	//当たり判定の更新
	void UpdateCollider();

private:
	//コピー禁止
	GameObject(const GameObject& a) = delete;
	GameObject& operator=(const GameObject&) = delete;
};

