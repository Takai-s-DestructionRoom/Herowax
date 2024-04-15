/*
* @file RootSignature.h
* @brief ルートシグネチャのラッパークラス
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <unordered_map>
#pragma warning(pop)

typedef D3D12_DESCRIPTOR_RANGE DescriptorRange;
typedef D3D12_STATIC_SAMPLER_DESC StaticSamplerDesc;
typedef std::vector<DescriptorRange> DescriptorRanges;
typedef std::vector<StaticSamplerDesc> StaticSamplerDescs;

struct RootParamater {
    D3D12_ROOT_PARAMETER_TYPE ParameterType;
    DescriptorRanges DescriptorTable;
    D3D12_ROOT_CONSTANTS Constants = {};
    D3D12_ROOT_DESCRIPTOR Descriptor = {};
    D3D12_SHADER_VISIBILITY ShaderVisibility = {};

    static DescriptorRanges OneTex(uint32_t shaderRegister) {
        DescriptorRange descriptorRange{};
        descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
        descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRange.BaseShaderRegister = shaderRegister; //テクスチャレジスタ0番
        descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        return DescriptorRanges{ descriptorRange };
    }
};

typedef std::vector<RootParamater> RootParamaters;

struct RootSignatureDesc {
    RootParamaters RootParamaters;
    StaticSamplerDescs StaticSamplers;
    D3D12_ROOT_SIGNATURE_FLAGS Flags;
};

class RootSignature
{
public:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mPtr = nullptr;
    RootSignatureDesc mDesc{};

	void Create();

    //生成済みなら取得、そうでないなら生成して登録してから取得
    static RootSignature& GetOrCreate(std::string id, RootSignatureDesc desc);

private:
    static std::unordered_map<std::string, RootSignature> sRootSignatureMap;
};

