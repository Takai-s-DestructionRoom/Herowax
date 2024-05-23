/*
* @file RenderTarget.h
* @brief レンダーターゲットそのものだったりそれを制御したりするクラス群
*/

#pragma once
#include "Texture.h"
#include <vector>

class RenderTexture
{
public:
	std::string mName;
	TextureHandle mTexHandle;
	TextureHandle mDepthTexHandle;
	Color mClearColor;
	uint32_t mHeapIndex = UINT32_MAX;

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle();

	void OpenResourceBarrier();
	void CloseResourceBarrier();
	void ClearRenderTarget();
	void ClearDepthStencil();

	bool IsBarrierClosed() {
		return TextureManager::Get(mTexHandle).GetResourceState() == D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	}

	Texture& GetTexture() {
		return TextureManager::Get(mTexHandle);
	}

	Texture& GetDepthTexture() {
		return TextureManager::Get(mDepthTexHandle);
	}

private:
	bool mFlagDepthOpen = false;
};

class RenderTarget
{
public:
	static void SetToBackBuffer();
	static void SetToTexture(std::string name);
	static void SetToTexture(std::vector<std::string> names);

	static RenderTexture* CreateRenderTexture(const uint32_t width, const uint32_t height, const Color clearColor, TextureHandle name = "", bool highColorRange = false);
	static void DeleteRenderTexture(std::string name);
	static void DeleteRenderTexture(RenderTexture* tex);
	static void DeleteRenderTextureAtEndFrame(std::string name);
	static void DeleteRenderTextureAtEndFrame(RenderTexture* tex);
	static void EndFrameProcess();
	static RenderTexture* GetRenderTexture(std::string name);
	static D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32_t index);
	static D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle(uint32_t index);

	static RenderTarget* GetInstance() {
		static RenderTarget instance;
		return &instance;
	}
private:
	RenderTarget() {
		CreateHeaps();
	};
	~RenderTarget() = default;
	RenderTarget(const RenderTarget&) {};
	RenderTarget& operator=(const RenderTarget&) { return *this; }

	void CreateHeaps();

	static const uint32_t sNUM_DESCRIPTORS = 2048;
	std::recursive_mutex mMutex;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	std::vector<std::string> mCurrentRenderTargets;
	std::map<std::string, RenderTexture> mRenderTargetMap;
	std::list<std::string> mDeleteScheduledList; //DeleteAtEndFrame予定リスト
};

