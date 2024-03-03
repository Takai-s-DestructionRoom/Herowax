/*
* @file DepthTest.h
* @brief 画面全体に被写界深度をかけるクラス
*/

#pragma once
#include <Texture.h>
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>
#include <SRConstBuffer.h>
#include <RootSignature.h>
#include <GraphicsPipeline.h>
#include <RenderTarget.h>

class DepthTest
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

	DepthTest();

	void Draw();

protected:
	RenderTexture* tex;

	static RootSignature& GetRootSignatureA();
	static GraphicsPipeline& GetGraphicsPipelineA();
	static GraphicsPipeline& GetGraphicsPipelineB();
};
