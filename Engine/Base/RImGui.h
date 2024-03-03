/*
* @file RImGui.h
* @brief ImGuiを制御するクラス
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include <string>
#pragma warning(pop)

class RImGui
{
public:
	static RImGui* GetInstance();

	//初期化処理まとめ
	static void Init();

	//フレーム開始時処理
	static void NewFrame();

	//描画処理まとめ
	static void Render();

	//終了処理まとめ
	static void Finalize();

private:
	ID3D12Device* mDevicePtr = nullptr;
	ID3D12DescriptorHeap* mSrvHeapPtr = nullptr;

	std::string mConfigPath;

	void InitInternal();
	void FinalInternal();

	RImGui() {};
	~RImGui() = default;
	RImGui(const RImGui& a) = delete;
	RImGui& operator=(const RImGui&) = delete;
	RImGui(const RImGui&& a) = delete;
	RImGui& operator=(const RImGui&&) = delete;
};

//ImGui独自拡張
namespace ImGui
{
	IMGUI_API bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);

	void HelpMarker(const char* desc);
}