#include "WaxManager.h"
#include "ParticleManager.h"
#include "ImGui.h"
#include "Temperature.h"
#include "Parameter.h"
#include "ColPrimitive3D.h"

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
}

void WaxManager::Init()
{
	waxGroups.clear();
}

void WaxManager::Update()
{
	//燃えている数を初期化
	isBurningNum = 0;

	Delete();

	for (auto& waxGroup : waxGroups)
	{
		waxGroup->Update();
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 350, 180 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Wax", NULL, window_flags);
	ImGui::Text("存在しているロウの数:%d", (int)GetWaxNum());
	ImGui::Text("燃えているロウの数:%d", isBurningNum);
	ImGui::Text("現在の温度:%f", TemperatureManager::GetInstance()->GetTemperature());
	ImGui::PushItemWidth(100);
	ImGui::InputFloat("ロウが燃えたときの上昇温度", &heatUpTemperature, 1.0f);
	ImGui::InputFloat("ボーナス上昇温度", &heatBonus, 1.0f);

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

	if (ImGui::Button("当たり判定の描画")) {
		isViewCol = !isViewCol;
	}

	if (ImGui::Button("Reset")) {
		Init();
	}
	if (ImGui::Button("セーブ")) {
		Parameter::Begin(fileName);
		Parameter::Save("ロウが燃えたときの上昇温度", heatUpTemperature);
		Parameter::Save("ボーナス上昇温度", heatBonus);
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
#pragma endregion
}

void WaxManager::Draw()
{
	for (auto& group : waxGroups)
	{
		group->Draw();
		for (auto& wax : group->waxs)
		{
			wax->Draw();
			if (isViewCol) {
				wax->DrawCollider();
			}
		}
	}
}

void WaxManager::Create(Transform transform, uint32_t power, Vector3 vec,
	float speed, Vector2 range, float size, float atkTime, float solidTime)
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
	waxGroups.back()->waxs.back()->Init(power, vec, speed, range, size, atkTime);
	waxGroups.back()->solidTimer.maxTime_ = solidTime;
}

float WaxManager::GetCalcHeatBonus()
{
	return heatBonus * (float)isBurningNum;
}

bool ReturnCol(ColPrimitive3D::Ray rayCol, ColPrimitive3D::Sphere sphereCol)
{
	Vector3 rayToSphere = sphereCol.pos - rayCol.start;

	//レイとの内積が0以下なら当たってない(レイより後ろに球があるときスルー)
	if (rayToSphere.Dot(rayCol.dir) < 0)
	{
		return false;
	}

	float t = rayCol.dir.GetNormalize().Dot(rayToSphere);

	//垂線を降ろした点
	Vector3 a = rayCol.start + rayCol.dir.GetNormalize() * t;
	//レイから球の最短ベクトル
	Vector3 b = sphereCol.pos - a;

	float len = b.Length();
	if (len - (sphereCol.r + rayCol.radius) <= 0.f)
	{
		return true;
	}

	return false;
}

void WaxManager::Collect(ColPrimitive3D::Ray collider)
{
	Wax* farObj = nullptr;

	for (auto& group : waxGroups)
	{
		for (auto& wax : group->waxs)
		{
			if (ReturnCol(collider, wax->collider))
			{
				if (farObj == nullptr)
				{
					farObj = wax.get();
				}
				float oldLen = (collider.start - farObj->GetPos()).Length();
				float len = (collider.start - wax->GetPos()).Length();
				if (len > oldLen)
				{
					farObj = wax.get();
				}
			}
		}
	}

	ParticleManager::GetInstance()->AddHoming(
		farObj->obj.mTransform.position, farObj->obj.mTransform.scale,
		10, 0.8f, farObj->waxOriginColor, "", 0.8f, 1.5f,
		-Vector3::ONE * 0.3f, Vector3::ONE * 0.3f,
		0.03f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.3f, 0.5f);

	farObj->isAlive = false;
	farObj = nullptr;
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
