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

	Color brightColor = {0,0,0,0};

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

	//光る値をシェーダーへ送信する専用バッファー
	void BrightTransferBuffer(const ViewProjection& view);
	
	//自身が持っているobjのDrawを専用のシェーダーで描画するもの
	void BrightDraw(std::string stageID = "Opaque");

	//自身が持っているobjのDrawを専用のシェーダーで描画するもの
	void BrightDrawTrans(std::string stageID = "Transparent");

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
			obj.mTransform.position.y - obj.mTransform.scale.y,
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

	//当たり判定の更新
	void UpdateCollider();

	/// <summary>
	/// 押し戻しを作成
	/// </summary>
	/// <param name="back">押し戻す対象</param>
	/// <param name="hit">押し戻しを行う判定(壁など)</param>
	static bool GetHitBack(GameObject& back,const GameObject& hit);

protected:
	void InitCollider();
	
private:
	RootSignature* GetRootSig();

	GraphicsPipeline* GetPipeline();
	GraphicsPipeline* GetPipelineTrans();

	std::vector<RenderOrder> GameObject::GetRenderOrder();

private:
	//コピー禁止
	GameObject(const GameObject& a) = delete;
	GameObject& operator=(const GameObject&) = delete;
};

