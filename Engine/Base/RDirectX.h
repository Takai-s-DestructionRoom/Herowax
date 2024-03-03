/*
* @file RDirectX.h
* @brief DirectX制御の基盤となるクラス
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <wrl.h>
#include <vector>
#pragma warning(pop)

#include "Texture.h"
#include "RootSignature.h"
#include "GraphicsPipeline.h"
#include "Shader.h"
#include "Color.h"

using namespace Microsoft::WRL;

class RDirectX
{
private:
	ComPtr<ID3D12Device> mDevice = nullptr;
	ComPtr<IDXGIFactory7> mDxgiFactory = nullptr;
	ComPtr<IDXGISwapChain4> mSwapChain = nullptr;
	ComPtr<ID3D12CommandAllocator> mCmdAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> mCmdList = nullptr;
	ComPtr<ID3D12CommandQueue> mCmdQueue = nullptr;
	ComPtr<ID3D12DescriptorHeap> mRtvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> mSrvHeap = nullptr;
	std::vector<ComPtr<ID3D12Resource>> mBackBuffers;
	ComPtr<ID3D12Resource> mDepthBuff = nullptr;
	ComPtr<ID3D12DescriptorHeap> mDsvHeap;
	ComPtr<ID3D12Fence> mFence = nullptr;
	size_t mFenceVal = 0;
	Shader mBasicVS;
	Shader mBasicPS;
	RootSignature mRootSignature;
	GraphicsPipeline mPipelineState;

public:
	//DirectX初期化
	static void Init();
	static RDirectX* GetInstance();

	//デバイスを取得する
	static ID3D12Device* GetDevice();

	//コマンドリストを取得する
	static ID3D12GraphicsCommandList* GetCommandList();

	//ほぼImGui専用みたいになってるmSrvHeapをプレゼント
	static ID3D12DescriptorHeap* GetSRVHeap();

	//基本ルートシグネチャを取得する
	static RootSignature GetDefRootSignature();

	//基本パイプラインを取得する
	static GraphicsPipeline GetDefPipeline();

	//何か良い感じに描画前処理をする
	static void PreDraw();

	//何か良い感じに描画後処理をする
	static void PostDraw();

	//バックバッファの数を取得する
	static size_t GetBackBufferSize();

	//現在のバックバッファ番号を取得する
	static UINT GetCurrentBackBufferIndex();

	//現在のバックバッファのリソースを取得する
	static ID3D12Resource* GetCurrentBackBufferResource();

	//現在のバックバッファのRTVハンドルを取得する
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferHandle();

	//バックバッファのDSVハンドルを取得する
	static D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferDSVHandle();

	//リソースバリアをPRESENT->RENDER_TARGETにする処理
	static void OpenResorceBarrier(ID3D12Resource* mResource);

	//リソースバリアをRENDER_TARGET->PRESENTにする処理
	static void CloseResourceBarrier(ID3D12Resource* mResource);

	//バックバッファをクリアする
	static void ClearBackBuffer(Color color);

	//コマンドリストを閉じて実行し、フリップして、コマンドリストを再び開ける
	static void RunDraw();

private:
	RDirectX() {};
	~RDirectX() = default;
	RDirectX(const RDirectX& a) = delete;
	RDirectX& operator=(const RDirectX&) = delete;

	D3D12_CPU_DESCRIPTOR_HANDLE mNowRtvHandle = {};

	void InitInternal();
};
