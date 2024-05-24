#pragma once
#include "Sprite.h"
#include "SRConstBuffer.h"
#include "Color.h"
#include "Float4.h"

struct Metaball2DBuffer
{
	Float4 color_;
	Float4 strokecolor_;
	float cutoff_;
	float stroke_;
};

class MetaBall2D
{
public:
	void Init();
	void Update();

	Vector2 GetPos() {
		Vector2 temp = {sprite.mTransform.position.x,sprite.mTransform.position.y};
		return temp;
	};
	Vector2 GetScale() {
		Vector2 temp = { sprite.mTransform.scale.x,sprite.mTransform.scale.y };
		return temp;
	};

public:
	Sprite sprite;
	bool isUse = false;

	float moveSpeed = 1.0f;
	float gravityAccel = 0.03f;
	float boundPower = 2.f;

	//重なっていると数値があがり、この値に応じて重力加速が伸びる
	float overlapPower = 0.01f;

private:
	float friction = 0.99f;	//摩擦係数
	Vector3 moveVec = { 0,0,0 };
	Vector2 velocity = {0,0};
	float gravity = 0.0f;
};

class MetaBall2DManager
{
public:
	static MetaBall2DManager* GetInstance();

	void Init();
	void Update();
	void Draw();

	void TransfarBuffer();
	void Imgui();
	void CraeteMetaBall();
	void CraeteMetaBall(Vector2 pos, Vector2 size);

private:
	MetaBall2DManager();
	~MetaBall2DManager();

	RootSignature GetRootSignature() {
		return mRootSignature;
	}

	GraphicsPipeline GetGraphicsPipeline() {
		return mPipelineState;
	}

public:
	//こいつにボールを追加していく
	std::vector<std::unique_ptr<MetaBall2D>> metaballs;

private:
	const int32_t METABALL_NUM = 300;

	Color color = Color::kGreen;
	Color strokecolor = Color::kWhite;
	float cutoff = 0.3f;
	float stroke = 0.7f;

	float moveSpeed = 1.0f;
	float gravityAccel = 0.03f;
	float boundPower = 2.f;
	
	bool renderTargetFlag = false;
	int32_t createNum = 100;
	Vector2 createSize = { 0.1f,0.1f};
	
	RootSignature mRootSignature;
	GraphicsPipeline mPipelineState;

	SRConstBuffer<Metaball2DBuffer> metaball2DBuffer; 
	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
};

