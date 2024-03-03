#include "RDirectX.h"
#include "RWindow.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "RootSignature.h"
#include "Vertex.h"
#include "SimpleDrawer.h"
#include "RenderTarget.h"

using namespace std;
using namespace DirectX;

RDirectX* RDirectX::GetInstance() {
	static RDirectX instance;
	return &instance;
}

ID3D12Device* RDirectX::GetDevice()
{
	return GetInstance()->mDevice.Get();
}

ID3D12GraphicsCommandList* RDirectX::GetCommandList()
{
	return GetInstance()->mCmdList.Get();
}

ID3D12DescriptorHeap* RDirectX::GetSRVHeap()
{
	return GetInstance()->mSrvHeap.Get();
}

RootSignature RDirectX::GetDefRootSignature()
{
	return GetInstance()->mRootSignature;
}

GraphicsPipeline RDirectX::GetDefPipeline()
{
	return GetInstance()->mPipelineState;
}

void RDirectX::PreDraw()
{
	RenderTarget::SetToBackBuffer();

	//プリミティブ形状設定
	RDirectX::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//SRVヒープの設定コマンド
	ID3D12DescriptorHeap* _heap = TextureManager::GetInstance()->GetSRVHeap();
	RDirectX::GetCommandList()->SetDescriptorHeaps(1, &_heap);

	RDirectX::GetCommandList()->SetGraphicsRootSignature(RDirectX::GetDefRootSignature().mPtr.Get());
	RDirectX::GetCommandList()->SetPipelineState(RDirectX::GetDefPipeline().mPtr.Get());
}

void RDirectX::PostDraw()
{
	RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());

	RDirectX::RunDraw();

	RDirectX::OpenResorceBarrier(RDirectX::GetCurrentBackBufferResource());
}

size_t RDirectX::GetBackBufferSize()
{
	return GetInstance()->mBackBuffers.size();
}

void RDirectX::Init() {
	GetInstance()->InitInternal();
}

void RDirectX::InitInternal() {
	HRESULT result;

	//DXGIファクトリー生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&mDxgiFactory));
	assert(SUCCEEDED(result));

	vector<ComPtr<IDXGIAdapter4>> adapters;
	ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

	//パフォーマンスが高い順に全てのアダプターを列挙する
	for (uint32_t i = 0;
		mDxgiFactory->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++) {
		adapters.push_back(tmpAdapter);
	}

	//妥当なアダプタの選別
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;

		//アダプターの情報を取得する
		adapters[i]->GetDesc3(&adapterDesc);

		//ソフトウェアデバイスを除外して、
		//最初に見つかったアダプタ（パフォーマンスが最も高い）を採用する
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			tmpAdapter = adapters[i];
#ifdef _DEBUG
			OutputDebugStringW(adapterDesc.Description);
			OutputDebugStringW(L"\n");
#endif
			break;
		}
	}

	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++) {
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&mDevice));
		if (result == S_OK) {
			featureLevel = levels[i];
			break;
		}
	}


	//コマンドアロケータを生成
	result = mDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&mCmdAllocator));
	assert(SUCCEEDED(result));


	//コマンドリストを生成
	result = mDevice->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mCmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&mCmdList));
	assert(SUCCEEDED(result));


	//コマンドキュー
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//生成
	result = mDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&mCmdQueue));
	assert(SUCCEEDED(result));


	//スワップチェーン
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = RWindow::GetWidth();
	swapChainDesc.Height = RWindow::GetHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //色情報の書式
	swapChainDesc.SampleDesc.Count = 1; //マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; //バックバッファ用
	swapChainDesc.BufferCount = 2; //バッファ数を2に
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //フリップ後は破棄
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//生成
	ComPtr<IDXGISwapChain1> _swapChain1;
	result = mDxgiFactory->CreateSwapChainForHwnd(
		mCmdQueue.Get(), RWindow::GetWindowHandle(), &swapChainDesc, nullptr, nullptr,
		&_swapChain1);
	_swapChain1.As(&mSwapChain);
	assert(SUCCEEDED(result));


	//デスクリプタヒープ(RTV)
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //レンダーターゲットビュー
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount; //裏表の二つ
	//生成
	mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap));
	assert(SUCCEEDED(result));


	//バックバッファ
	mBackBuffers.resize(swapChainDesc.BufferCount);

	//スワップチェーンの全てのバッファに対して処理
	for (size_t i = 0; i < mBackBuffers.size(); i++) {
		//スワップチェーンからバッファを取得
		mSwapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&mBackBuffers[i]));
		//デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mRtvHeap->GetCPUDescriptorHandleForHeapStart();
		//裏か表かでアドレスをずらす
		rtvHandle.ptr += i * mDevice->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		//レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//レンダーターゲットビューの生成
		mDevice->CreateRenderTargetView(mBackBuffers[i].Get(), &rtvDesc, rtvHandle);
	}


	//デスクリプタヒープ(SRV)
	const size_t kMaxSRVCount = 1; //シェーダーリソースビューの最大個数
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダーから見える
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//生成
	mSrvHeap = nullptr;
	result = mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvHeap));
	assert(SUCCEEDED(result));


	//深度バッファ
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = RWindow::GetWidth();
	depthResourceDesc.Height = RWindow::GetHeight();
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //デプスステンシル

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	//バッファ生成
	result = mDevice->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&mDepthBuff)
	);
	assert(SUCCEEDED(result));

	//ヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; //デプスステンシル
	result = mDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap));
	assert(SUCCEEDED(result));

	//ビュー生成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	mDevice->CreateDepthStencilView(
		mDepthBuff.Get(),
		&dsvDesc,
		mDsvHeap->GetCPUDescriptorHandleForHeapStart()
	);


	//フェンスの生成
	size_t fenceVal_ = 0;
	result = mDevice->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));

	//ルートシグネチャ
	// デスクリプタレンジの設定
	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ルートパラメータの設定
	RootParamaters rootParams(5);
	//テクスチャレジスタ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //デスクリプタテーブル
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ0番(Material)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[1].Descriptor.ShaderRegister = 0; //定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ1番(Transform)
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[2].Descriptor.ShaderRegister = 1; //定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ2番(ViewProjection)
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[3].Descriptor.ShaderRegister = 2; //定数バッファ番号
	rootParams[3].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ3番(Light)
	rootParams[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[4].Descriptor.ShaderRegister = 3; //定数バッファ番号
	rootParams[4].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	// テクスチャサンプラーの設定
	StaticSamplerDesc samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //横繰り返し（タイリング）
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //縦繰り返し（タイリング）
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //奥行繰り返し（タイリング）
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //リニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f; //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //ピクセルシェーダーからだけ見える

	// ルートシグネチャの設定
	mRootSignature.mDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	mRootSignature.mDesc.RootParamaters = rootParams;
	mRootSignature.mDesc.StaticSamplers = StaticSamplerDescs{samplerDesc};

	mRootSignature.Create();

	//Basicシェーダ達の読み込みとコンパイル
	mBasicVS = Shader("Shader/BasicVS.hlsl", "main", "vs_5_0");
	if (!mBasicVS.mSucceeded) {
		string error;
		error.resize(mBasicVS.mErrorBlob->GetBufferSize());

		copy_n((char*)mBasicVS.mErrorBlob->GetBufferPointer(),
			mBasicVS.mErrorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	mBasicPS = Shader("Shader/BasicPS.hlsl", "main", "ps_5_0");
	if (!mBasicPS.mSucceeded) {
		string error;
		error.resize(mBasicPS.mErrorBlob->GetBufferSize());

		copy_n((char*)mBasicPS.mErrorBlob->GetBufferPointer(),
			mBasicPS.mErrorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	Shader::Register("basicVS", mBasicVS);
	Shader::Register("basicPS", mBasicPS);

	// グラフィックスパイプライン設定
	InputLayout inputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};

	// シェーダーの設定
	mPipelineState.mDesc.VS = mBasicVS;
	mPipelineState.mDesc.PS = mBasicPS;

	// サンプルマスクの設定
	mPipelineState.mDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; //標準

	// ラスタライザの設定
	mPipelineState.mDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; //背面カリング
	mPipelineState.mDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	mPipelineState.mDesc.RasterizerState.DepthClipEnable = true;
	mPipelineState.mDesc.BlendState.AlphaToCoverageEnable = true;

	// ブレンドステート(半透明合成)
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = mPipelineState.mDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //RGBA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	// 頂点レイアウトの設定
	mPipelineState.mDesc.InputLayout = inputLayout;

	// 図形の形状設定
	mPipelineState.mDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// デプスステンシルステート設定
	mPipelineState.mDesc.DepthStencilState.DepthEnable = true;
	mPipelineState.mDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	mPipelineState.mDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	mPipelineState.mDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// その他の設定
	mPipelineState.mDesc.NumRenderTargets = 1;
	mPipelineState.mDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	mPipelineState.mDesc.SampleDesc.Count = 1; //1ピクセルにつき1回サンプリング

	//セット
	mPipelineState.mDesc.pRootSignature = mRootSignature.mPtr.Get();

	mPipelineState.Create();
}

UINT RDirectX::GetCurrentBackBufferIndex() {
	return GetInstance()->mSwapChain->GetCurrentBackBufferIndex();
}

ID3D12Resource* RDirectX::GetCurrentBackBufferResource() {
	return GetInstance()->mBackBuffers[GetCurrentBackBufferIndex()].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE RDirectX::GetCurrentBackBufferHandle() {
	RDirectX* instance = GetInstance();

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = instance->mRtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += static_cast<size_t>(GetCurrentBackBufferIndex()) * instance->mDevice->GetDescriptorHandleIncrementSize(instance->mRtvHeap->GetDesc().Type);

	return rtvHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE RDirectX::GetBackBufferDSVHandle()
{
	return GetInstance()->mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void RDirectX::OpenResorceBarrier(ID3D12Resource* mResource) {
	D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = mResource;
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; //Before:表示から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; //After:描画へ
	RDirectX::GetCommandList()->ResourceBarrier(1, &barrierDesc);
}

void RDirectX::CloseResourceBarrier(ID3D12Resource* mResource) {
	D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = mResource;
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; //Before:描画から
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; //After:表示へ
	RDirectX::GetCommandList()->ResourceBarrier(1, &barrierDesc);
}

void RDirectX::ClearBackBuffer(Color color)
{
	RDirectX* instance = GetInstance();

	FLOAT clearColor[] = { color.r, color.g, color.b, color.r };
	instance->mCmdList->ClearRenderTargetView(GetCurrentBackBufferHandle(), clearColor, 0, nullptr);
	instance->mCmdList->ClearDepthStencilView(
		instance->mDsvHeap->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0, nullptr);
}

void RDirectX::RunDraw() {
	HRESULT result;

	RDirectX* instance = GetInstance();

	//命令のクローズ
	result = instance->mCmdList->Close();
	assert(SUCCEEDED(result));
	//コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { instance->mCmdList.Get() };
	instance->mCmdQueue->ExecuteCommandLists(1, cmdLists);

	//フリップ
	result = instance->mSwapChain->Present(1, 0);
	assert(SUCCEEDED(result));

	instance->mCmdQueue->Signal(instance->mFence.Get(), ++instance->mFenceVal);
	if (instance->mFence->GetCompletedValue() != instance->mFenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		if (event != NULL) {
			instance->mFence->SetEventOnCompletion(instance->mFenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		else {
			assert(event != NULL);
		}
	}

	//キューをクリア
	result = instance->mCmdAllocator->Reset();
	assert(SUCCEEDED(result));
	// 再びコマンドリストを貯める準備
	result = instance->mCmdList->Reset(instance->mCmdAllocator.Get(), nullptr);
	assert(SUCCEEDED(result));

	TextureManager::EndFrameProcess();
	SimpleDrawer::ClearData();
}
