#include "GraphicsPipeline.h"
#include "RDirectX.h"

std::unordered_map<std::string, GraphicsPipeline> GraphicsPipeline::sPipelineMap;

GraphicsPipeline& GraphicsPipeline::GetOrCreate(std::string id, PipelineStateDesc desc)
{
    auto& map = GraphicsPipeline::sPipelineMap;
    auto itr = map.find(id);
    if (itr != map.end()) {
        return itr->second;
    }

    GraphicsPipeline newPipeline;
    newPipeline.mDesc = desc;
    newPipeline.Create();
    map[id] = newPipeline;
    return map[id];
}

void GraphicsPipeline::Create()
{
	HRESULT result;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC _desc{};

    _desc.pRootSignature = mDesc.pRootSignature;

    if (mDesc.VS.mSucceeded) {
        _desc.VS.pShaderBytecode = mDesc.VS.mShaderBlob->GetBufferPointer();
        _desc.VS.BytecodeLength  = mDesc.VS.mShaderBlob->GetBufferSize();
    }

    if (mDesc.PS.mSucceeded) {
        _desc.PS.pShaderBytecode = mDesc.PS.mShaderBlob->GetBufferPointer();
        _desc.PS.BytecodeLength  = mDesc.PS.mShaderBlob->GetBufferSize();
    }

    if (mDesc.DS.mSucceeded) {
        _desc.DS.pShaderBytecode = mDesc.DS.mShaderBlob->GetBufferPointer();
        _desc.DS.BytecodeLength  = mDesc.DS.mShaderBlob->GetBufferSize();
    }

    if (mDesc.HS.mSucceeded) {
        _desc.HS.pShaderBytecode = mDesc.HS.mShaderBlob->GetBufferPointer();
        _desc.HS.BytecodeLength  = mDesc.HS.mShaderBlob->GetBufferSize();
    }

    if (mDesc.GS.mSucceeded) {
        _desc.GS.pShaderBytecode = mDesc.GS.mShaderBlob->GetBufferPointer();
        _desc.GS.BytecodeLength  = mDesc.GS.mShaderBlob->GetBufferSize();
    }

    _desc.StreamOutput = mDesc.StreamOutput;
    _desc.BlendState = mDesc.BlendState;
    _desc.SampleMask = mDesc.SampleMask;
    _desc.RasterizerState = mDesc.RasterizerState;
    _desc.DepthStencilState = mDesc.DepthStencilState;

    if (mDesc.InputLayout.size() > 0) {
        _desc.InputLayout.pInputElementDescs = &mDesc.InputLayout[0];
        _desc.InputLayout.NumElements = (UINT)mDesc.InputLayout.size();
    }
    else {
        _desc.InputLayout.pInputElementDescs = nullptr;
        _desc.InputLayout.NumElements = 0;
    }

    _desc.IBStripCutValue = mDesc.IBStripCutValue;
    _desc.PrimitiveTopologyType = mDesc.PrimitiveTopologyType;
    _desc.NumRenderTargets = mDesc.NumRenderTargets;

    for (int32_t i = 0; i < 8; i++) {
        _desc.RTVFormats[i] = mDesc.RTVFormats[i];
    }
    
    _desc.DSVFormat = mDesc.DSVFormat;
    _desc.SampleDesc = mDesc.SampleDesc;
    _desc.NodeMask = mDesc.NodeMask;
    _desc.CachedPSO = mDesc.CachedPSO;
    _desc.Flags = mDesc.Flags;

	result = RDirectX::GetDevice()->CreateGraphicsPipelineState(&_desc, IID_PPV_ARGS(&mPtr));
	assert(SUCCEEDED(result));
}
