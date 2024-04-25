#include "Wax.h"
#include "Camera.h"
#include "Renderer.h"
#include "WaxManager.h"
#include "ParticleManager.h"
#include "Minimap.h"

Color Wax::waxOriginColor = { 0.8f, 0.6f, 0.35f, 1.f };
Color Wax::waxEndColor = { 0.8f, 0.0f, 0.f, 1.f };

Wax::Wax():GameObject(),
	atkSpeed(1.f),
	igniteTimer(0.25f),
	burningTimer(1.0f),
	extinguishTimer(0.5f),
	disolveValue(0.f)
{
	state = std::make_unique<WaxNormal>();
	obj = PaintableModelObj(Model::Load("./Resources/Model/wax/wax.obj", "wax", true));
	obj.mTuneMaterial.mColor = waxOriginColor;
	TextureManager::Load("./Resources/DissolveMap.png","DissolveMap");
}

void Wax::DeadParticle()
{
	ParticleManager::GetInstance()->AddHoming(
		obj.mTransform.position, obj.mTransform.scale,
		1, 0.8f, waxOriginColor, "", 0.8f, 1.5f,
		-Vector3(1, 1, 1) * 0.3f, Vector3(1, 1, 1) * 0.3f,
		0.03f, -Vector3(1, 1, 1) * 0.1f, Vector3(1, 1, 1) * 0.1f, 0.3f, 0.5f);
}

bool Wax::GetIsSolidNow()
{
	if (isSolid && isSolided == false)
	{
		return true;
	}
	return false;
}

void Wax::Init(Transform transform, Vector3 endPos_, float height, float size, float atkTime)
{
	//hp = maxHP;
	obj.mTransform = transform;
	startPos = obj.mTransform.position;
	endPos = endPos_;

	Vector3 endLength = (endPos - startPos);
	//開始地点+終点までの距離/2
	interPos = startPos + (endLength / 2);
	//高さは指定した位置
	interPos.y = height;

	spline.clear();
	spline.push_back(startPos);
	spline.push_back(interPos);
	spline.push_back(endPos);

	atkSize = size;
	atkTimer = atkTime;
	atkTimer.Start();

	/*iconSize = { 0.5f,0.5f };
	minimapIcon.SetTexture(TextureManager::Load("./Resources/circle.png", "circle"));
	minimapIcon.mMaterial.mColor = waxOriginColor;*/
}

void Wax::Update()
{
	moveVec = { 0,0,0 };
	atkTimer.Update();
	
	isSolided = isSolid;	//1F前の状態を保存

	//段々大きくなる
	float atkRange = Easing::OutBack(atkTimer.GetTimeRate());
	Vector3 waxScale = { atkSize,1.f,atkSize };	//蝋の大きさ
	waxScale *= atkRange;
	SetScale(waxScale);								//大きさを反映

	if (atkTimer.GetRun()) {
		obj.mTransform.position = Util::Spline(spline,atkTimer.GetTimeRate());	//飛んでいく
	}

	//地面に埋ってたら
	if (obj.mTransform.position.y - obj.mTransform.scale.y < 0.f)
	{
		//地面に触れるとこまで移動
		obj.mTransform.position.y = 0.f;
		
		isGround = true;
	}

	//燃焼周りのステートの更新
	state->Update(this);
	//変化があったら
	if (changingState) {
		//ステート遷移
		std::swap(state, nextState);
		changingState = false;
		nextState = nullptr;
	}

	//spline以外で動かす場合の足し算
	obj.mTransform.position += moveVec;

	UpdateCollider();

	mDisolveBuff->disolveValue = disolveValue;

	////スクリーン座標を求める
	//screenPos = Minimap::GetInstance()->GetScreenPos(obj.mTransform.position);
	//minimapIcon.mTransform.position = { screenPos.x,screenPos.y,0.f };

	////決めたサイズに
	//iconSize = 
	//{ obj.mTransform.scale.x * 0.5f * WaxManager::GetInstance()->GetSlimeWaxSizeMag(),
	//	obj.mTransform.scale.z * 0.5f * WaxManager::GetInstance()->GetSlimeWaxSizeMag() };
	//minimapIcon.mTransform.scale = Vector3(iconSize.x,iconSize.y,1.f) * Minimap::GetInstance()->iconSize;

	////更新忘れずに
	//minimapIcon.mTransform.UpdateMatrix();
	//minimapIcon.TransferBuffer();

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	
}

void Wax::Draw()
{
	if (isAlive)
	{
		GraphicsPipeline pipe = WaxManager::GetInstance()->CreateDisolvePipeLine();

		for (std::shared_ptr<ModelMesh> data : obj.mModel->mData) {
			std::vector<RootData> rootData = {
				{ RootDataType::SRBUFFER_CBV, obj.mMaterialBuffMap[data->mMaterial.mName].mBuff },
				{ RootDataType::SRBUFFER_CBV, obj.mTransformBuff.mBuff },
				{ RootDataType::SRBUFFER_CBV, obj.mViewProjectionBuff.mBuff },
				{ RootDataType::LIGHT },
				{ TextureManager::Get(data->mMaterial.mTexture).mGpuHandle },
				{ TextureManager::Get("DissolveMap").mGpuHandle},
				{ RootDataType::SRBUFFER_CBV ,mDisolveBuff.mBuff}
			};

			RenderOrder order;
			order.mRootSignature = pipe.mDesc.pRootSignature;
			order.pipelineState = pipe.mPtr.Get();
			order.rootData = rootData;
			order.vertView = &data->mVertBuff.mView;
			order.indexView = &data->mIndexBuff.mView;
			order.indexCount = static_cast<uint32_t>(data->mIndices.size());

			Renderer::DrawCall("Opaque", order);
		}
	}
}

void Wax::DrawUI()
{
	////描画範囲制限
	//Renderer::SetScissorRects({ Minimap::GetInstance()->rect });
	//minimapIcon.Draw();

	////元の範囲に戻してあげる
	//Renderer::SetScissorRects({ Minimap::GetInstance()->defaultRect });
}

bool Wax::IsBurning()
{
	return burningTimer.GetRun();
}

bool Wax::IsNormal() 
{
	return !igniteTimer.GetStarted() &&
		!burningTimer.GetStarted() && 
		!extinguishTimer.GetStarted();
}

Color Wax::SolidBling(const Easing::EaseTimer& timer)
{
	//もう1色は更新し続け
	Color color = Color::kWhite;

	if (timer.GetEnd()) {
		//ここだいぶカス
		isSolid = true;
		return Color::kLightblue;
	}
	else if (timer.GetTimeRate() > 0.7f) {
		//トグルでtrueになったら色変えることで点滅
		isFlashing = ((int)(timer.GetTimeRate() * 1000.0f) % 100 > 50);
		if (isFlashing ^ color == Color::kWhite)
		{
			return Color::kYellow;
		}
	}
	
	return waxOriginColor;
}
