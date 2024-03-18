#include "GameObject.h"
#include "Camera.h"
#include <Renderer.h>

//クラス名に入れ忘れていたら"unset"を返すように
GameObject::GameObject() : isAlive(true) ,classname("_unset_")
{
	drawerObj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
}

Vector3 GameObject::GetPos() const
{
	return obj.mTransform.position;
}

Vector3 GameObject::GetScale() const
{
	return obj.mTransform.scale;
}

bool GameObject::GetIsAlive() const
{
	return isAlive;
}

bool GameObject::GetIsViewCol() const
{
	return isViewCol;
}

std::string GameObject::GetObjectName()
{
	//未設定ならassert
	if (classname == "_unset_") {
		assert(0 + "classnameにクラス名を入れ忘れています");
	}
	//設定しているなら通す
	return classname;
}

void GameObject::UpdateCollider()
{
	collider.pos = GetPos();

	//もっともデカいスケールを半径とする
	float maxR = GetScale().x;
	if (maxR < GetScale().y)maxR = GetScale().y;
	if (maxR < GetScale().z)maxR = GetScale().z;
	
	collider.r = maxR;

	drawerObj.mTransform.position = collider.pos;
	drawerObj.mTransform.scale = { collider.r ,collider.r ,collider.r };

	drawerObj.mTransform.UpdateMatrix();
	drawerObj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void GameObject::DrawCollider()
{
	//非表示状態ならスキップ
	if (!isViewCol)return;

	//パイプラインをワイヤーフレームに
	PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
	pipedesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("WireObject", pipedesc);
	for (RenderOrder& order : drawerObj.GetRenderOrder()) {
		order.pipelineState = pipe.mPtr.Get();
		Renderer::DrawCall("Opaque", order);
	}
}
