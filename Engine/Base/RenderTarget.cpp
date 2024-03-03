#include "RenderTarget.h"
#include "RDirectX.h"
#include <Util.h>

void RenderTarget::SetToBackBuffer()
{
	RenderTarget* manager = GetInstance();

	for (std::string name : manager->mCurrentRenderTargets) {
		if (name.empty()) {
			RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());
			continue;
		}

		RenderTexture* tex = manager->GetRenderTexture(name);
		if (tex != nullptr) tex->CloseResourceBarrier();
	}
	manager->mCurrentRenderTargets.clear();

	RDirectX::OpenResorceBarrier(RDirectX::GetCurrentBackBufferResource());
	D3D12_CPU_DESCRIPTOR_HANDLE backBuffHandle = RDirectX::GetCurrentBackBufferHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE backBuffDSVHandle = RDirectX::GetBackBufferDSVHandle();
	RDirectX::GetCommandList()->OMSetRenderTargets(1, &backBuffHandle, false, &backBuffDSVHandle);
	manager->mCurrentRenderTargets.push_back("");
}

void RenderTarget::SetToTexture(std::string name)
{
	RenderTarget* manager = GetInstance();

	for (std::string current : manager->mCurrentRenderTargets) {
		if (current.empty()) {
			RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());
			continue;
		}

		RenderTexture* tex = manager->GetRenderTexture(current);
		if (tex != nullptr) tex->CloseResourceBarrier();
	}
	manager->mCurrentRenderTargets.clear();

	RenderTexture* tex = manager->GetRenderTexture(name);

	if (tex == nullptr) {
		//ないよ
#ifdef _DEBUG
		OutputDebugStringA(("RKEngine ERROR: Failed SetToTexture. RenderTargetTexture(" + name + ") isn't exist.\n").c_str());
#endif
		return;
	}

	tex->OpenResourceBarrier();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = tex->GetRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = tex->GetDSVHandle();
	RDirectX::GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	manager->mCurrentRenderTargets.push_back(name);
}

void RenderTarget::SetToTexture(std::vector<std::string> names)
{
	RenderTarget* manager = GetInstance();

	for (std::string name : manager->mCurrentRenderTargets) {
		if (name.empty()) {
			RDirectX::CloseResourceBarrier(RDirectX::GetCurrentBackBufferResource());
			continue;
		}

		RenderTexture* tex = manager->GetRenderTexture(name);
		if (tex != nullptr) tex->CloseResourceBarrier();
	}
	manager->mCurrentRenderTargets.clear();

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> dsvHandles;

	for (std::string& name : names) {
		RenderTexture* tex = manager->GetRenderTexture(name);

		if (tex == nullptr) {
			//ないよ
#ifdef _DEBUG
			OutputDebugStringA(("RKEngine ERROR: Failed SetToTexture. RenderTargetTexture(" + name + ") isn't exist.\n").c_str());
#endif
			return;
		}

		tex->OpenResourceBarrier();
		rtvHandles.push_back(tex->GetRTVHandle());
		dsvHandles.push_back(tex->GetDSVHandle());
		manager->mCurrentRenderTargets.push_back(name);
	}

	RDirectX::GetCommandList()->OMSetRenderTargets((UINT)names.size(), &rtvHandles[0], false, &dsvHandles[0]);
}

RenderTexture* RenderTarget::CreateRenderTexture(const uint32_t width, const uint32_t height, const Color clearColor, TextureHandle name)
{
	RenderTarget* manager = GetInstance();

	HRESULT result;

	RenderTexture renderTarget;
	Texture texture = Texture(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	Texture depthTexture = Texture(D3D12_RESOURCE_STATE_DEPTH_WRITE);

	uint32_t useIndex = UINT32_MAX;

	std::unique_lock<std::recursive_mutex> lock(manager->mMutex);
	auto itr = manager->mRenderTargetMap.find(name);
	if (itr != manager->mRenderTargetMap.end()) {
		useIndex = itr->second.mHeapIndex;
	}
	else {
		for (uint32_t i = 0; i < sNUM_DESCRIPTORS; i++) {
			bool ok = true;
			for (std::pair<const std::string, RenderTexture>& p : manager->mRenderTargetMap) {
				if (p.second.mHeapIndex == i) {
					ok = false;
					break;
				}
			}

			if (ok) {
				useIndex = i;
				break;
			}
		}
	}
	lock.unlock();

	if (useIndex == UINT32_MAX) {
		//Over.
		return nullptr;
	}

	renderTarget.mHeapIndex = useIndex;

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureResourceDesc.Width = width;
	textureResourceDesc.Height = height;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;
	textureResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	D3D12_CLEAR_VALUE textureClearValue{};
	textureClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureClearValue.Color[0] = clearColor.r;
	textureClearValue.Color[1] = clearColor.g;
	textureClearValue.Color[2] = clearColor.b;
	textureClearValue.Color[3] = clearColor.a;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&textureClearValue,
		IID_PPV_ARGS(&texture.mResource)
	);
	assert(SUCCEEDED(result));

	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = width;
	depthResDesc.Height = height;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //デプスステンシル

	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	result = RDirectX::GetDevice()->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthTexture.mResource)
	);
	assert(SUCCEEDED(result));

	if (name.empty()) {
		name = "NoNameRenderTargetTex_Index" + std::to_string(useIndex);
	}

	TextureHandle texHandleA = "RenderTargetTex_" + name;
	TextureHandle texHandleB = "DepthTex_" + name;
	if (texHandleA == "RenderTargetTex_") texHandleA += "NoName";
	if (texHandleB == "DepthTex_") texHandleB += "NoName";
	TextureManager::Register(texture, texHandleA);
	TextureManager::Register(depthTexture, texHandleB);

	renderTarget.mName = name;
	renderTarget.mTexHandle = texHandleA;
	renderTarget.mDepthTexHandle = texHandleB;
	renderTarget.mClearColor = clearColor;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	size_t rtvincrementSize = RDirectX::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle = manager->mRtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHeapHandle.ptr += useIndex * rtvincrementSize;
	RDirectX::GetDevice()->CreateRenderTargetView(texture.mResource.Get(), &rtvDesc, rtvHeapHandle);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	size_t dsvincrementSize = RDirectX::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle = manager->mDsvHeap->GetCPUDescriptorHandleForHeapStart();
	dsvHeapHandle.ptr += useIndex * dsvincrementSize;
	RDirectX::GetDevice()->CreateRenderTargetView(texture.mResource.Get(), &rtvDesc, rtvHeapHandle);

	RDirectX::GetDevice()->CreateDepthStencilView(depthTexture.mResource.Get(), &dsvDesc, dsvHeapHandle);

	lock.lock();
	manager->mRenderTargetMap[name] = renderTarget;
	return &manager->mRenderTargetMap[name];
}

RenderTexture* RenderTarget::GetRenderTexture(std::string name) {
	RenderTarget* manager = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(manager->mMutex);
	auto itr = manager->mRenderTargetMap.find(name);
	if (itr != manager->mRenderTargetMap.end()) {
		return &itr->second;
	}

	//ないよ
	Util::DebugLog("RKEngine ERROR : RenderTarget::GetRenderTexture() : Failed find from map.");
	return nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetRTVHandle(uint32_t index)
{
	RenderTarget* manager = RenderTarget::GetInstance();

	size_t rtvincrementSize = RDirectX::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle = manager->mRtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHeapHandle.ptr += index * rtvincrementSize;
	return rtvHeapHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetDSVHandle(uint32_t index)
{
	RenderTarget* manager = RenderTarget::GetInstance();

	size_t dsvincrementSize = RDirectX::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle = manager->mDsvHeap->GetCPUDescriptorHandleForHeapStart();
	dsvHeapHandle.ptr += index * dsvincrementSize;
	return dsvHeapHandle;
}

void RenderTarget::CreateHeaps()
{
	HRESULT result;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = sNUM_DESCRIPTORS;

	result = RDirectX::GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap));
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = sNUM_DESCRIPTORS;

	result = RDirectX::GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap));
	assert(SUCCEEDED(result));
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTexture::GetRTVHandle()
{
	return RenderTarget::GetRTVHandle(mHeapIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTexture::GetDSVHandle()
{
	return RenderTarget::GetDSVHandle(mHeapIndex);
}

void RenderTexture::OpenResourceBarrier()
{
	GetTexture().ChangeResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);
	GetDepthTexture().ChangeResourceState(D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void RenderTexture::CloseResourceBarrier()
{
	GetTexture().ChangeResourceState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	GetDepthTexture().ChangeResourceState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void RenderTexture::ClearRenderTarget()
{
	OpenResourceBarrier();
	FLOAT color[] = { mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.a };
	RDirectX::GetCommandList()->ClearRenderTargetView(GetRTVHandle(), color, 0, nullptr);
	CloseResourceBarrier();
}

void RenderTexture::ClearDepthStencil()
{
	OpenResourceBarrier();
	RDirectX::GetCommandList()->ClearDepthStencilView(
		GetDSVHandle(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0, nullptr);
	CloseResourceBarrier();
}
