#include "RayMarchTestScene.h"
#include "RInput.h"
#include "RImGui.h"
#include "InstantDrawer.h"
#include "TimeManager.h"
#include "Camera.h"
#include "Renderer.h"
#include "Quaternion.h"

void RayMarchTestScene::Init()
{
	InstantDrawer::PreCreate();
	plane = ModelObj(Model::Load("./Resources/Model/Cube.obj", "firewisp", true));
	
	//plane.mUseBillboardY = true;
	//plane.Init("white2x2", { 1.f, 1.f });
	//plane.mImage.mMaterial.mColor = Color::kWhite;

	plane.mTransform.position = { 0,0,0 };
	plane.mTransform.scale = { 10,10,10 };
	plane.mTransform.rotation = { 0,0,0 };

	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;

	slimeBuff->slimeValue = 7.5f;
	slimeBuff->sphereNum = 64;
	slimeBuff->rayMatchNum = 16;

	spheres.clear();
	for (int32_t i = 0; i < sphereNum; i++)
	{
		spheres.emplace_back();
		spheres.back().Init();
	}
}

void RayMarchTestScene::Update()
{
	InstantDrawer::DrawInit();

	camera.Update();
	light.Update();

	Vector3 cameraVec = camera.mViewProjection.mTarget - camera.mViewProjection.mEye;
	cameraVec.Normalize();

	static float cameraDist = 0.5f;
	static bool checkLookAt = false;

	//plane.mTransform.position = camera.mViewProjection.mEye + (cameraVec * cameraDist);

	//plane.Update(camera.mViewProjection);
	plane.mTransform.UpdateMatrix();
	plane.TransferBuffer(camera.mViewProjection);

	//バッファにデータ転送(今はメッシュ描画も行いたいのでモデルオブジェで
	//作っているが、後々posとradiusだけのデータに置き換えて送るようにする)
	for (int32_t i = 0; i < sphereNum; i++)
	{
		spheres[i].Update();

		slimeBuff->spheres[i].x = spheres[i].obj.mTransform.position.x;
		slimeBuff->spheres[i].y = spheres[i].obj.mTransform.position.y;
		slimeBuff->spheres[i].z = spheres[i].obj.mTransform.position.z;
		//wをradiusとして扱うようにしてデータ圧縮
		slimeBuff->spheres[i].w = spheres[i].obj.mTransform.scale.x;
	}

	ImGui::SetNextWindowSize({ 300, 200 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("RayMarchTest", NULL);
	ImGui::Text("CameraPos x:%f y:%f z:%f",
		camera.mViewProjection.mEye.x,
		camera.mViewProjection.mEye.y,
		camera.mViewProjection.mEye.z);
	ImGui::Text("CameraTarget x:%f y:%f z:%f",
		camera.mViewProjection.mTarget.x,
		camera.mViewProjection.mTarget.y,
		camera.mViewProjection.mTarget.z);
	ImGui::Checkbox("板描画切り替え", &isPlaneDraw);
	ImGui::Checkbox("球描画切り替え", &isSphereMeshDraw);
	ImGui::SliderFloat("板のカメラまでの距離", &cameraDist,0.0f,10.f);
	ImGui::DragFloat3("position", &plane.mTransform.position.x);
	ImGui::DragFloat3("scale", &plane.mTransform.scale.x);
	ImGui::DragFloat3("rotation", &plane.mTransform.rotation.x);
	/*static Vector2 size = plane.mImage.GetSize();
	ImGui::DragFloat2("size", &size.x, 0.1f);
	plane.mImage.SetSize(size);*/
	ImGui::SliderFloat("slimeValue", &slimeBuff->slimeValue,1.f,50.f);
	ImGui::SliderInt("sphereNum", &sphereNum,1,256);
	ImGui::SliderInt("rayMatchNum", &slimeBuff->rayMatchNum,1,64);
	ImGui::SliderFloat("clipValue", &slimeBuff->clipValue,0.0001f,1.0f);
	ImGui::End();

	slimeBuff->sphereNum = sphereNum;
}

void RayMarchTestScene::Draw()
{
	if (isPlaneDraw) {
		plane.Draw();
	}

	if (isSphereMeshDraw) {
		for (int32_t i = 0; i < sphereNum; i++)
		{
			spheres[i].Draw();
		}
	}

	GraphicsPipeline pipe = SlimeShaderPipeLine();

	for (std::shared_ptr<ModelMesh> data : plane.mModel->mData) {
		std::vector<RootData> rootData = {
			{ RootDataType::SRBUFFER_CBV, plane.mMaterialBuffMap[data->mMaterial.mName].mBuff },
			{ RootDataType::SRBUFFER_CBV, plane.mTransformBuff.mBuff },
			{ RootDataType::SRBUFFER_CBV, plane.mViewProjectionBuff.mBuff },
			{ RootDataType::LIGHT },
			{ TextureManager::Get(data->mMaterial.mTexture).mGpuHandle },
			{ RootDataType::SRBUFFER_CBV ,slimeBuff.mBuff}
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

	/*RenderOrder order;
	order.pipelineState = pipe.mPtr.Get();
	order.mRootSignature = pipe.mDesc.pRootSignature;
	order.vertBuff = plane.mImage.mVertBuff;
	order.indexBuff = plane.mImage.mIndexBuff;
	order.indexCount = 6;
	order.rootData = {
		{RootDataType::SRBUFFER_CBV, plane.mImage.mMaterialBuff.mBuff },
		{RootDataType::SRBUFFER_CBV, plane.mImage.mTransformBuff.mBuff },
		{RootDataType::SRBUFFER_CBV, plane.mImage.mViewProjectionBuff.mBuff },
		{RootDataType::LIGHT},
		{TextureManager::Get(plane.mImage.GetTexture()).mGpuHandle},
		{RootDataType::SRBUFFER_CBV,slimeBuff.mBuff}
	};

	Renderer::DrawCall("Opaque", order);
	*/

	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}

void RayMarchTestScene::Finalize()
{
}

GraphicsPipeline RayMarchTestScene::SlimeShaderPipeLine()
{
	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
	pipedesc.VS = Shader::GetOrCreate("SlimeShaderVS", "Shader/SlimeShader/SlimeShaderVS.hlsl", "main", "vs_5_0");
	pipedesc.PS = Shader::GetOrCreate("SlimeShaderPS", "Shader/SlimeShader/SlimeShaderPS.hlsl", "main", "ps_5_0");

	RootParamaters rootParams(6);
	//マテリアル
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ1番(Transform)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[1].Descriptor.ShaderRegister = 1; //定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ2番(ViewProjection)
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[2].Descriptor.ShaderRegister = 2; //定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ3番(Light)
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[3].Descriptor.ShaderRegister = 3; //定数バッファ番号
	rootParams[3].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//テクスチャ(本体テクスチャ)
	descriptorRange.BaseShaderRegister = 0;	//テクスチャレジスタ1番
	rootParams[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[4].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//定数バッファ4番(slime)
	rootParams[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[5].Descriptor.ShaderRegister = 4; //定数バッファ番号
	rootParams[5].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える


	RootSignatureDesc rootDesc = RDirectX::GetDefRootSignature().mDesc;
	rootDesc.RootParamaters = rootParams;
	RootSignature mRootSig = RootSignature::GetOrCreate("SlimeShaderSignature", rootDesc);

	pipedesc.pRootSignature = mRootSig.mPtr.Get();

	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("SlimeShader", pipedesc);

	return pipe;
}

void RandomSphere::Init()
{
	obj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));

	moveVec = Util::GetRandVector3({ 1,1,1 }, -1.0f, 1.0f);

	float radius = Util::GetRand(1.0f, 3.0f);
	obj.mTransform.scale = { radius,radius,radius };

	timer.Start();
}
void RandomSphere::Update() 
{
	obj.mTransform.position += moveVec * 0.03f;

	timer.Update();
	if (timer.GetEnd()) {
		
		//obj.mTransform.position = { 0,0,0 };
		timer.Start();
		moveVec = -moveVec;
		
		float radius = Util::GetRand(1.0f, 3.0f);
		obj.mTransform.scale = { radius,radius,radius };
	}

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}
void RandomSphere::Draw() 
{
	obj.Draw();
}