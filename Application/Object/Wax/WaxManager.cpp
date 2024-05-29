#include "WaxManager.h"
#include "ParticleManager.h"
#include "ImGui.h"
//#include "Temperature.h"
#include "Parameter.h"
#include "ColPrimitive3D.h"
#include "Player.h"
#include "Quaternion.h"
#include <RAudio.h>
#include "RImGui.h"

WaxManager* WaxManager::GetInstance()
{
	static WaxManager instance;
	return &instance;
}

bool WaxManager::CheckHitWaxGroups(std::unique_ptr<WaxGroup>& group1,
	std::unique_ptr<WaxGroup>& group2)
{
	//一応内部で同じかどうか確認
	if (group1 == group2) {
		return false;
	}
	for (auto& wax1 : group1->waxs) {
		for (auto& wax2 : group2->waxs) {

			//どっちも固まり始めておらず
			if (!wax1->isSolid && !wax2->isSolid)
			{
				//どちらも地面にいないとダメ
				if (wax1->isGround && wax2->isGround)
				{
					//どれか一つでも当たっていたら
					if (ColPrimitive3D::CheckSphereToSphere(wax1->collider, wax2->collider))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

GraphicsPipeline WaxManager::CreateDisolvePipeLine()
{
	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
	pipedesc.VS = Shader::GetOrCreate("DisolveVS", "Shader/Disolve/DisolveVS.hlsl", "main", "vs_5_0");
	pipedesc.PS = Shader::GetOrCreate("DisolvePS", "Shader/Disolve/DisolvePS.hlsl", "main", "ps_5_0");

	RootParamaters rootParams(7);
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
	//テクスチャ(マスクテクスチャ)
	descriptorRange.BaseShaderRegister = 1;	//テクスチャレジスタ1番
	rootParams[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[5].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//定数バッファ4番(disolve)
	rootParams[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[6].Descriptor.ShaderRegister = 4; //定数バッファ番号
	rootParams[6].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	RootSignatureDesc rootDesc = RDirectX::GetDefRootSignature().mDesc;
	rootDesc.RootParamaters = rootParams;
	RootSignature mRootSig = RootSignature::GetOrCreate("disolveSignature", rootDesc);

	pipedesc.pRootSignature = mRootSig.mPtr.Get();

	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("Disolve", pipedesc);

	return pipe;
}

GraphicsPipeline WaxManager::SlimeShaderPipeLine()
{
	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
	pipedesc.VS = Shader::GetOrCreate("SlimeWaxLightVS", "Shader/SlimeWax_Light/SlimeWaxLightVS.hlsl", "main", "vs_5_0");
	pipedesc.PS = Shader::GetOrCreate("SlimeWaxLightPS", "Shader/SlimeWax_Light/SlimeWaxLightPS.hlsl", "main", "ps_5_0");

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

void WaxManager::Delete()
{
	//死んでいるグループがあれば消す
	waxGroups.remove_if([](std::unique_ptr<WaxGroup>& waxgroup) {
		return !waxgroup->GetIsAlive();
		});
}

WaxManager::WaxManager() :
	heatUpTemperature(10.f),
	heatBonus(5.f),
	waxDamage(10)
{
	//生成時に変数をセーブデータから引っ張ってくる
	std::map<std::string, std::string> extract = Parameter::Extract(fileName);
	heatUpTemperature = Parameter::GetParam(extract, "ロウが燃えたときの上昇温度", 5.f);
	heatBonus = Parameter::GetParam(extract, "ボーナス上昇温度", 2.f);
	accelAmount = Parameter::GetParam(extract, "回収時の加速度", 0.1f);
	colliderSize = Parameter::GetParam(extract, "当たり判定の大きさ", 2.5f);
	slimeWaxSizeMag = Parameter::GetParam(extract, "ロウの見た目の大きさの係数", 1.0f);

	for (int i = 0; i < 10; i++)
	{
		waxTime.emplace_back();
		waxTime.back() = 0.0f;
	}
	for (int i = 0; i < 10; i++)
	{
		std::string waxnum = std::to_string(i + 1) + "体の時";
		if (i + 1 >= 10) {
			waxnum = std::to_string(i + 1) + "体以上の時";
		}
		waxTime[i] = Parameter::GetParam(extract, waxnum.c_str(), 0.0f);
	}

	//ディゾルブで使うパイプラインを生成する
	CreateDisolvePipeLine();

	RAudio::Load("Resources/Sounds/SE/P_collect.wav", "Collect");
}

void WaxManager::Init()
{
	waxGroups.clear();

	slimeWax.Init();
	//notCollect = true;
}

void WaxManager::Update()
{
	//燃えている数を初期化
	isBurningNum = 0;

	Delete();

	for (auto& waxGroup : waxGroups)
	{
		for (auto& wax : waxGroup->waxs)
		{
			wax->colliderSize = colliderSize;
			wax->collectTimer.maxTime_ = collectTime;
		}
		waxGroup->Update();
	}

	//ロウを入れる
	for (auto& group : waxGroups)
	{
		for (auto& wax : group->waxs)
		{
			slimeWax.spheres.emplace_back();
			//コライダー基準でデータを送るように
			slimeWax.spheres.back().collider.pos = wax->collider.pos;
			slimeWax.spheres.back().collider.r = wax->collider.r * slimeWaxSizeMag * (float)maxWaxStock / 50.f;
		}
	}

	slimeWax.Update();

#pragma region ImGui
	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 350, 180 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("Wax");
		ImGui::Text("存在しているロウの数:%d", (int)GetWaxNum());
		ImGui::Text("slimeWax_sphere:%d", (int32_t)slimeWax.spheres.size());
		ImGui::PushItemWidth(100);

		ImGui::InputFloat("回収時の加速度", &accelAmount, 0.05f);
		ImGui::InputFloat("当たり判定の大きさ", &colliderSize, 0.1f);
		ImGui::InputFloat("ロウの見た目の大きさの係数", &slimeWaxSizeMag, 0.1f);

		ImGui::Checkbox("当たり判定の描画", &isViewCol);
		ImGui::Checkbox("ロウの見た目の描画", &isViewSlimeWax);
		ImGui::Checkbox("オブジェクトロウの描画", &isViewObjectWax);

		ImGui::Text("敵を捕まえたときの固まっている秒数");
		for (int i = 0; i < 10; i++)
		{
			std::string waxnum = std::to_string(i + 1) + "体の時";
			if (i + 1 >= 10) {
				waxnum = std::to_string(i + 1) + "体以上の時";
			}
			ImGui::InputFloat(waxnum.c_str(), &waxTime[i], 1.0f);
		}
		ImGui::Text("ロウグループ数:%d", (int)waxGroups.size());
		ImGui::PopItemWidth();

		if (ImGui::Button("Reset")) {
			Init();
		}
		if (ImGui::Button("セーブ")) {
			Parameter::Begin(fileName);
			Parameter::Save("ロウが燃えたときの上昇温度", heatUpTemperature);
			Parameter::Save("ボーナス上昇温度", heatBonus);
			Parameter::Save("回収時の加速度", accelAmount);
			Parameter::Save("当たり判定の大きさ", colliderSize);
			Parameter::Save("ロウの見た目の大きさの係数", slimeWaxSizeMag);

			for (int i = 0; i < 10; i++)
			{
				std::string waxnum = std::to_string(i + 1) + "体の時";
				if (i + 1 >= 10) {
					waxnum = std::to_string(i + 1) + "体以上の時";
				}
				Parameter::Save(waxnum.c_str(), waxTime[i]);
			}
			Parameter::End();
		}

		ImGui::End();
	}
#pragma endregion
}

void WaxManager::Draw()
{
	for (auto& group : waxGroups)
	{
		group->DrawUI();
		if (isViewObjectWax) {
			group->Draw();
		}
		for (auto& wax : group->waxs)
		{
			if (isViewCol) {
				wax->SetDrawCollider(true);
				wax->DrawCollider();
			}
		}
	}

	if (isViewSlimeWax) {
		slimeWax.Draw();
	}
}

void WaxManager::Create(Transform transform,Vector3 endPos,float height, float size, float atkTime, float solidTime)
{
	//生成時にグループ作成
	waxGroups.emplace_back();
	waxGroups.back() = std::make_unique<WaxGroup>();
	//生成したロウをグループへ格納
	waxGroups.back()->waxs.emplace_back();
	waxGroups.back()->waxs.back() = std::make_unique<Wax>();

	//指定された状態に
	waxGroups.back()->waxs.back()->obj.mTransform = transform;
	//情報を受け取って格納
	waxGroups.back()->waxs.back()->Init(transform, endPos, height, size, atkTime);
	waxGroups.back()->solidTimer.maxTime_ = solidTime;
}

float WaxManager::GetCalcHeatBonus()
{
	return heatBonus * (float)isBurningNum;
}


bool WaxManager::Collect(ColPrimitive3D::Ray collider)
{
	Wax* farObj = nullptr;
	//bool isExistence = false;

	for (auto& group : waxGroups)
	{
		for (auto& wax : group->waxs)
		{
			if (ColPrimitive3D::RayToSphereCol(collider, wax->collider))
			{
				//今一番遠いロウがないなら入れておく
				if (farObj == nullptr)
				{
					farObj = wax.get();
				}
				//現状一番遠いロウとの距離
				float oldLen = (collider.start - farObj->GetPos()).Length();
				//今のロウとの距離
				float len = (collider.start - wax->GetPos()).Length();

				//一番遠いロウが更新されたら入れ替える
				if (len > oldLen)
				{
					farObj = wax.get();
				}
			}
		}
	}

	//もし範囲内にロウがあった時
	if (farObj != nullptr)
	{
		//回収モードにする
		farObj->collectPos = collider.start;
		farObj->ChangeState<WaxCollect>();
		//farObj = nullptr;	//消す

		//notCollect = false;
		return true;
	}

	return false;
}

int32_t WaxManager::Collect(ColPrimitive3D::Ray collider, float waxCollectVertical, GameObject* target)
{
	int32_t getNum = 0;
	bool isCollect = false;

	//回収ターゲットを入れる
	WaxManager::GetInstance()->collectTarget = target;

	for (auto& group : waxGroups)
	{
		for (auto& wax : group->waxs)
		{
			if (RayToSphereCol(collider, wax->collider))
			{
				//今のロウとの距離
				float len = (collider.start - wax->GetPos()).Length();

				//見たロウが範囲外ならスキップ
				if (waxCollectVertical < len) {
					continue;
				}

				//回収モードにする
				isCollect = true;
				wax.get()->collectPos = collider.start;
				wax.get()->ChangeState<WaxCollect>();
				getNum++;

				notCollect = false;
			}
		}
	}

	if (isCollect == true)
	{
		RAudio::Play("Collect", 0.6f);
	}
	
	return getNum;
}

uint32_t WaxManager::GetWaxNum()
{
	int32_t waxNum = 0;
	for (auto& group : waxGroups)
	{
		waxNum += (uint32_t)group->waxs.size();
	}
	return waxNum;
}
