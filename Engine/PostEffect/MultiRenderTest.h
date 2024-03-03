/*
* @file MultiRenderTest.h
* @brief 描画のテスト用クラス, これいる？
*/

#pragma once
#include <Texture.h>
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>
#include <SRConstBuffer.h>
#include <RootSignature.h>
#include <GraphicsPipeline.h>
#include <RenderTarget.h>

class MultiRenderTest
{
public:
	struct Setting {
		float useDepthColor = false;
		float focusDepth = 0.95f;
		float nearFocusWidth = 0.02f;
		float farFocusWidth = 0.04f;
	};

	TextureHandle mTexture;

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<Setting> mConstBuff;

	Setting mSetting{};

	MultiRenderTest();

	void Draw();

protected:
	RenderTexture* texA;
	RenderTexture* texB;

	static RootSignature& GetRootSignatureA();
	static RootSignature& GetRootSignatureB();
	static GraphicsPipeline& GetGraphicsPipelineA();
	static GraphicsPipeline& GetGraphicsPipelineB();
};
