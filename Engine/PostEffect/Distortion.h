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
	struct NoiseSetting {
		float fineness = 48.f;		//ノイズの細かさ(値が大きいほど細かくなる)
		float time = 0.f;			//uvずらす用タイマー
	};

	struct BlurSetting {
		float sigma = 0.01f;
		float stepwidth = 0.001f;
	};

	TextureHandle mTexture;

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<NoiseSetting> noiseConstBuff;
	SRConstBuffer<BlurSetting> blurConstBuff;

	NoiseSetting noiseSetting{};
	BlurSetting blurSetting{};
	uint32_t mLevel = 1;

	Distortion();

	void Draw();

	void SetTimer(float time) { noiseSetting.time = time; }

protected:
	static RootSignature& GetRootSignatureNoise();
	static RootSignature& GetRootSignatureBlur();
	static GraphicsPipeline& GetGraphicsPipelineNoise();	//ノイズ
	static GraphicsPipeline& GetGraphicsPipelineBlur();		//ガウスぼかし
	static GraphicsPipeline& GetGraphicsPipelineAdd();		//加算合成
};