/*
* @file CrossFilter.h
* @brief 画面全体にクロスフィルタをかけるクラス
*/

#pragma once
#include <Texture.h>
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>
#include <SRConstBuffer.h>
#include <RootSignature.h>
#include <GraphicsPipeline.h>
#include <RenderTarget.h>

class CrossFilter
{
public:
	struct BlurSetting {
		float angle = 0;
		float pickRange = 0.06f;
	};

	TextureHandle mTexture;

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<BlurSetting> mConstBuff;

	BlurSetting mSetting{};

	CrossFilter();

	void Draw();

protected:
	RenderTexture* texA;
	RenderTexture* texB;

	static RootSignature& GetRootSignatureA();
	static RootSignature& GetRootSignatureB();
	static GraphicsPipeline& GetGraphicsPipelineA(); //明るさ抽出
	static GraphicsPipeline& GetGraphicsPipelineB(); //直線状ぼかし
	static GraphicsPipeline& GetGraphicsPipelineC(); //加算合成
};
