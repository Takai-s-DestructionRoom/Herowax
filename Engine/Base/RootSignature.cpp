#include "RootSignature.h"
#include "RDirectX.h"
#include <vector>

std::unordered_map<std::string, RootSignature> RootSignature::sRootSignatureMap;

RootSignature& RootSignature::GetOrCreate(std::string id, RootSignatureDesc desc)
{
	auto& map = RootSignature::sRootSignatureMap;
	auto itr = map.find(id);
	if (itr != map.end()) {
		return itr->second;
	}

	RootSignature newRootSignature;
	newRootSignature.mDesc = desc;
	newRootSignature.Create();
	map[id] = newRootSignature;
	return map[id];
}

void RootSignature::Create()
{
	HRESULT result;

	D3D12_ROOT_SIGNATURE_DESC _desc{};

	std::vector<D3D12_ROOT_PARAMETER> rootParams;
	for (RootParamater& param : mDesc.RootParamaters) {
		D3D12_ROOT_PARAMETER _param{};
		_param.ParameterType = param.ParameterType;
		switch (param.ParameterType) {
		case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
		{
			D3D12_ROOT_DESCRIPTOR_TABLE table{};
			if (param.DescriptorTable.size() > 0) {
				table.NumDescriptorRanges = (UINT)param.DescriptorTable.size();
				table.pDescriptorRanges = &param.DescriptorTable[0];
			}
			else {
				table.NumDescriptorRanges = 0;
				table.pDescriptorRanges = nullptr;
			}
			_param.DescriptorTable = table;
			break;
		}	
		case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
			_param.Constants = param.Constants;
			break;
		case D3D12_ROOT_PARAMETER_TYPE_CBV:
		case D3D12_ROOT_PARAMETER_TYPE_SRV:
		case D3D12_ROOT_PARAMETER_TYPE_UAV:
			_param.Descriptor = param.Descriptor;
			break;
		}
		_param.ShaderVisibility = param.ShaderVisibility;
		rootParams.emplace_back(_param);
	}

	if (rootParams.size() > 0) {
		_desc.pParameters = &rootParams[0];
		_desc.NumParameters = (UINT)rootParams.size();
	}
	else {
		_desc.pParameters = nullptr;
		_desc.NumParameters = 0;
	}
	
	if (mDesc.StaticSamplers.size() > 0) {
		_desc.pStaticSamplers = &mDesc.StaticSamplers[0];
		_desc.NumStaticSamplers = (UINT)mDesc.StaticSamplers.size();
	}
	else {
		_desc.pStaticSamplers = nullptr;
		_desc.NumStaticSamplers = 0;
	}
	
	_desc.Flags = mDesc.Flags;

	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&_desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, nullptr);
	assert(SUCCEEDED(result));

	result = RDirectX::GetDevice()->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&mPtr));
	assert(SUCCEEDED(result));
}
