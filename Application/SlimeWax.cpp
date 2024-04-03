#include "SlimeWax.h"
#include "Camera.h"
#include "RImGui.h"
#include "Renderer.h"
#include "Parameter.h"

void SlimeWax::Init()
{
	//スクリーンの準備
	screen.mUseBillboardY = false;
	screen.Init("white2x2", { 1.f, 1.f });
	screen.mImage.mMaterial.mColor = Color::kWhite;

	screen.mTransform.position = { 0,0,0 };
	screen.mTransform.scale = { 1,1,1 };
	screen.mTransform.rotation = { 0,0,0 };

	//パラメータ読み込み
	std::map <std::string, std::string> extract = Parameter::Extract("slimeWax");
	sphereNum = (int32_t)Parameter::GetParam(extract,"sphereNum", (float)sphereNum);
	changeNum = sphereNum;
	slimeValue = Parameter::GetParam(extract,"slimeValue", slimeValue);
	rayMatchNum = (int32_t)Parameter::GetParam(extract,"rayMatchNum", (float)rayMatchNum);
	clipValue = Parameter::GetParam(extract,"clipValue", clipValue);
	rayMarchClampNum = (int32_t)Parameter::GetParam(extract,"rayMarchClampNum", (float)rayMarchClampNum);

	spheres.clear();
	for (int32_t i = 0; i < sphereNum; i++)
	{
		spheres.emplace_back();
		spheres.back().Init();
	}
}

void SlimeWax::Update()
{
	//クランプ数を超えるならクランプする
	if (rayMatchNum * spheres.size() >= rayMarchClampNum) {
		rayMatchNum = rayMarchClampNum / (int32_t)spheres.size();
	}

	//カメラからビルボードの距離をそのままサイズにぶち込んで
	//絶対にビルボードがカメラ画角すべてを埋めるようにする
	Vector3 billboardVec = screen.mTransform.position - Camera::sNowCamera->mViewProjection.mEye;
	float billboardLength = billboardVec.LengthSq() / 2;
	screen.mImage.SetSize(Vector2(billboardLength, billboardLength));

	//こうしているのは、描画したいメタボールが描画の基準となっている
	//オブジェクトから離れすぎると描画されなくなってしまうため
	//ビルボードのワールドの位置は変えずに、無理やり画角に収まるようにしたパワー

	//一番近い位置に配置
	float minLength = 10000000.f;
	for (auto& sphere : spheres)
	{
		Vector3 sphereVec = sphere.collider.pos - Camera::sNowCamera->mViewProjection.mEye;
		float rfMinLength = sphereVec.LengthSq() / 2;
		if (minLength > rfMinLength) {
			minLength = rfMinLength;
			screen.mTransform.position = sphere.collider.pos - sphereVec.GetNormalize() * 5;
		}
	}

	screen.Update(Camera::sNowCamera->mViewProjection);

	//バッファにデータ転送(今はメッシュ描画も行いたいのでモデルオブジェで
	//作っているが、後々posとradiusだけのデータに置き換えて送るようにする)
	for (int32_t i = 0; i < sphereNum; i++)
	{
		spheres[i].Update();

		slimeBuff->spheres[i].x = spheres[i].collider.pos.x;
		slimeBuff->spheres[i].y = spheres[i].collider.pos.y;
		slimeBuff->spheres[i].z = spheres[i].collider.pos.z;
		//wをradiusとして扱うようにしてデータ圧縮
		slimeBuff->spheres[i].w = spheres[i].collider.r;
	}

	ImGui::SetNextWindowSize({ 300, 200 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("RayMarchTest", NULL);
	ImGui::Text("CameraPos x:%f y:%f z:%f",
		Camera::sNowCamera->mViewProjection.mEye.x,
		Camera::sNowCamera->mViewProjection.mEye.y,
		Camera::sNowCamera->mViewProjection.mEye.z);
	ImGui::Text("CameraTarget x:%f y:%f z:%f",
		Camera::sNowCamera->mViewProjection.mTarget.x,
		Camera::sNowCamera->mViewProjection.mTarget.y,
		Camera::sNowCamera->mViewProjection.mTarget.z);
	ImGui::SliderInt("球の数(再生成しないと変わらない)", &changeNum,1,128);
	if (ImGui::Button("球再生成")) {
		sphereNum = changeNum;
		spheres.clear();
		for (int32_t i = 0; i < sphereNum; i++)
		{
			spheres.emplace_back();
			spheres.back().Init();
		}
		rayMatchNum = rayMarchClampNum / (int32_t)spheres.size();
	}
	ImGui::Checkbox("板描画切り替え", &isPlaneDraw);
	ImGui::DragFloat3("position", &screen.mTransform.position.x);
	ImGui::Text("size x:%f y:%f", screen.mImage.GetSize().x, 
		screen.mImage.GetSize().y);
	ImGui::DragFloat3("rotation", &screen.mTransform.rotation.x, 0.01f);
	ImGui::SliderFloat("slimeValue", &slimeValue,0.01f,1.0f);
	ImGui::SliderInt("rayMatchNum", &rayMatchNum,1,64);
	ImGui::InputFloat("clipValue", &clipValue,0.001f);
	ImGui::SliderInt("rayMarchClampNum", &rayMarchClampNum,0,2048);
	if (ImGui::Button("セーブ")) {
		Parameter::Begin("slimeWax");
		Parameter::Save("sphereNum", sphereNum);
		Parameter::Save("slimeValue", slimeValue);
		Parameter::Save("rayMatchNum", rayMatchNum);
		Parameter::Save("clipValue", clipValue);
		Parameter::Save("rayMarchClampNum", rayMarchClampNum);
		Parameter::End();
	}

	ImGui::End();

	//バッファへ転送
	slimeBuff->sphereNum = sphereNum;
	slimeBuff->slimeValue = slimeValue;
	slimeBuff->rayMatchNum = rayMatchNum;
	slimeBuff->clipValue = clipValue;
}

void SlimeWax::Draw()
{
	if (isPlaneDraw) {
		screen.Draw();
	}

	GraphicsPipeline pipe = SlimeShaderPipeLine();
	RenderOrder order;
	order.pipelineState = pipe.mPtr.Get();
	order.mRootSignature = pipe.mDesc.pRootSignature;
	order.vertBuff = screen.mImage.mVertBuff;
	order.indexBuff = screen.mImage.mIndexBuff;
	order.indexCount = 6;
	order.rootData = {
		{RootDataType::SRBUFFER_CBV, screen.mImage.mMaterialBuff.mBuff },
		{RootDataType::SRBUFFER_CBV, screen.mImage.mTransformBuff.mBuff },
		{RootDataType::SRBUFFER_CBV, screen.mImage.mViewProjectionBuff.mBuff },
		{RootDataType::LIGHT},
		{TextureManager::Get(screen.mImage.GetTexture()).mGpuHandle},
		{RootDataType::SRBUFFER_CBV, slimeBuff.mBuff },
	};

	Renderer::DrawCall("Opaque", order);
}

GraphicsPipeline SlimeWax::SlimeShaderPipeLine()
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
	moveVec = Util::GetRandVector3({ 1,1,1 }, -1.0f, 1.0f);

	collider.r = Util::GetRand(1.0f, 3.0f);
	moveSpeed = Util::GetRand(0.01f, 0.05f);

	timer.Start();
}

void RandomSphere::Update()
{
	collider.pos += moveVec * moveSpeed;

	timer.Update();
	if (timer.GetEnd()) {

		timer.Start();
		moveVec = -moveVec;
		moveSpeed = Util::GetRand(0.01f, 0.05f);

		collider.r = Util::GetRand(1.0f, 3.0f);
	}
}