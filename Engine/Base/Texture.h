/*
* @file Texture.h
* @brief テクスチャ管理クラス
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <d3d12.h>
#include <string>
#include <vector>
#include <list>
#include <wrl.h>
#include <memory>
#include <map>
#include <mutex>
#pragma warning(pop)

#include "Color.h"
#include <Rect.h>
#include <Vector2.h>

typedef std::string TextureHandle;

class Texture
{
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> mResource; //テクスチャのリソース
	D3D12_CPU_DESCRIPTOR_HANDLE mCpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE(); //SRVのハンドル(CPU側)
	D3D12_GPU_DESCRIPTOR_HANDLE mGpuHandle = D3D12_GPU_DESCRIPTOR_HANDLE(); //SRVのハンドル(GPU側)
	uint32_t mHeapIndex = UINT32_MAX;
	std::string mFilePath; //ファイルへのパス
	bool mNowLoading = false;

	Texture() : mState(D3D12_RESOURCE_STATE_COMMON) {};
	Texture(D3D12_RESOURCE_STATES firstState) : mState(firstState) {};

	/// <summary>
	/// テクスチャのリソースステートを変更します
	/// この関数以外からリソースステートを変更すると良くないことが起きます
	/// </summary>
	/// <param name="state">変更先リソースステート</param>
	/// <param name="cmdList>使用するコマンドリスト(通常指定する必要はなし</param>
	void ChangeResourceState(D3D12_RESOURCE_STATES state, ID3D12GraphicsCommandList* cmdList = nullptr);

	// テクスチャの現在のリソースステートを取得します
	D3D12_RESOURCE_STATES GetResourceState() const {
		return mState;
	}

	/// <summary>
	/// テクスチャをコピーします
	/// </summary>
	/// <param name="dest">コピー先テクスチャ</param>
	/// <param name="srcRect">コピー元領域</param>
	/// <param name="destPos">コピー先位置</param>
	void Copy(Texture* dest, RRect srcRect, Vector2 destPos = {0, 0});

private:
	D3D12_RESOURCE_STATES mState;
};

class TextureManager
{
public:
	//TextureManagerを取得する
	static TextureManager* GetInstance() {
		static TextureManager instance;
		return &instance;
	}

	//読み込んだテクスチャをGPUへ転送
	static void Transfer();

	static Texture GetEmptyTexture();
	static Texture GetHogeHogeTexture();

	/// <summary>
	/// 指定色で塗りつぶされたテクスチャを作る
	/// </summary>
	/// <param name="color">塗りつぶす色</param>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="handle">必要なら、任意ハンドル名指定</param>
	/// <returns>作られたテクスチャのハンドル</returns>
	static TextureHandle Create(const Color color, const size_t width, const uint32_t height, const std::string handle = "");

	/// <summary>
	/// 色情報の配列によってテクスチャを作る
	/// </summary>
	/// <param name="pSource">Color配列</param>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="filepath">あれば、ファイルパス</param>
	/// <param name="handle">必要なら、任意ハンドル名指定</param>
	/// <returns>作られたテクスチャのハンドル</returns>
	static TextureHandle Create(const Color* pSource, const size_t width, const uint32_t height, const std::string filepath = "", const std::string handle = "");

	/// <summary>
	/// ファイルからテクスチャを読み込んで登録する
	/// </summary>
	/// <param name="filepath">ファイルへのパス</param>
	/// <param name="handle">必要なら、任意ハンドル名指定</param>
	/// <returns>読み込んだテクスチャのハンドル</returns>
	static TextureHandle Load(const std::string filepath, const std::string handle = "");

	/// <summary>
	/// メモリからテクスチャを読み込んで登録する
	/// </summary>
	/// <param name="pSource">そのメモリへのポインタ</param>
	/// <param name="size">サイズ</param>
	/// <param name="filepath">あれば、ファイルへのパス</param>
	/// <param name="handle">必要なら、任意ハンドル名指定</param>
	/// <returns>読み込んだテクスチャのハンドル</returns>
	static TextureHandle Load(const void* pSource, const size_t size, const std::string filepath = "", const std::string handle = "");

	/// <summary>
	/// 登録済みのテクスチャを取得する
	/// </summary>
	/// <param name="handle">取得するハンドル</param>
	/// <returns></returns>
	static Texture& Get(const TextureHandle& handle);

	/// <summary>
	/// テクスチャを登録する
	/// </summary>
	/// <param name="texture">登録するテクスチャ</param>
	/// <param name="handle">登録先のテクスチャハンドル</param>
	/// <returns>登録先のテクスチャハンドル</returns>
	static TextureHandle Register(Texture texture, TextureHandle handle = "");

	/// <summary>
	/// 登録済みのテクスチャを即座に破棄する
	/// </summary>
	/// <param name="handle">破棄するテクスチャのハンドル</param>
	static void UnRegister(const TextureHandle& handle);

	/// <summary>
	/// 登録済みのテクスチャをそのフレーム終了時に破棄する
	/// そのフレームの描画に使うけどそれ以降いらない場合はこっち
	/// </summary>
	/// <param name="handle">破棄するテクスチャのハンドル</param>
	static void UnRegisterAtEndFrame(const TextureHandle& handle);

	//フレーム終了時の処理を行う(呼べ)
	static void EndFrameProcess();

	//読み込んだテクスチャを全て即座に破棄する
	static void UnRegisterAll();

	ID3D12DescriptorHeap* GetSRVHeap() {
		return mSrvHeap.Get();
	}

private:
	TextureManager() {
		Init();
	};
	~TextureManager() = default;
	TextureManager(const TextureManager&) {};
	TextureManager& operator=(const TextureManager&) { return *this; }

	void Init();
	
	TextureHandle CreateInternal(const Color color, const size_t width, const uint32_t height, const std::string handle = "");
	TextureHandle CreateInternal(const Color* pSource, const size_t width, const uint32_t height, const std::string filepath = "", const std::string handle = "");
	TextureHandle LoadInternal(const std::string filepath, const std::string handle = "");
	TextureHandle LoadInternal(const void* pSource, const size_t size, const std::string filepath, const std::string handle = "");
	Texture& GetInternal(const TextureHandle& handle);
	TextureHandle RegisterInternal(Texture texture, TextureHandle handle = "");
	void UnRegisterInternal(const TextureHandle& handle);
	void UnRegisterAtEndFrameInternal(const TextureHandle& handle);
	void EndFrameProcessInternal();

	std::recursive_mutex mMutex;

	//テクスチャ転送用のリスト達
	bool mRequireTransfer = false;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCmdAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCmdList = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCmdQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence = nullptr;
	size_t mFenceVal = 0;

	//ヒープくん
	static const uint32_t NUM_SRV_DESCRIPTORS = 2048; //デスクリプタヒープの数
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap; //テクスチャ用SRVデスクリプタヒープ
	std::map<TextureHandle, Texture> mTextureMap;
	std::vector<TextureHandle> mNowLoadingTextures;
	std::map<ID3D12Resource*, Microsoft::WRL::ComPtr<ID3D12Resource>> mIntermediateMap;
	std::list<TextureHandle> mUnregisterScheduledList; //UnRegisterAtEndFrame予定リスト
};
