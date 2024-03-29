/*
* @file Distortion.h
* @brief 画面全体に歪みをかけるクラス
*/

#pragma once
#include <Texture.h>
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>
#include <SRConstBuffer.h>
#include <RootSignature.h>
#include <GraphicsPipeline.h>

class Distortion
{
public:
	struct BlurSetting {
		float sigma = 0.002f;
		float stepwidth = 0.001f;
	};

	TextureHandle mTexture;

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<BlurSetting> mConstBuff;

	BlurSetting mSetting{};
	uint32_t mLevel = 3;

	Distortion();

	void Draw();

protected:
	static RootSignature& GetRootSignatureA();
	static RootSignature& GetRootSignatureB();
	static GraphicsPipeline& GetGraphicsPipelineA(); //ノイズ
	static GraphicsPipeline& GetGraphicsPipelineB(); //ガウスぼかし
	static GraphicsPipeline& GetGraphicsPipelineC(); //加算合成
};