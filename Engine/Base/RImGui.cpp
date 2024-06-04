#include "RImGui.h"
#include "RWindow.h"
#include "RDirectX.h"
#include <PathUtil.h>
#include <Util.h>

#include "RInput.h"

bool RImGui::showImGui = false;

RImGui* RImGui::GetInstance()
{
    static RImGui instance;
    return &instance;
}

void RImGui::Init()
{
    GetInstance()->InitInternal();
}

void RImGui::NewFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void RImGui::Render()
{
    if (RInput::GetKeyDown(DIK_F7)) {
        //showImGui = !showImGui;
    }

    RImGui* instance = GetInstance();
    ImGui::Render();
    RDirectX::GetCommandList()->SetDescriptorHeaps(1, &instance->mSrvHeapPtr);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), RDirectX::GetCommandList());
}

void RImGui::Finalize()
{
    GetInstance()->FinalInternal();
}

void RImGui::InitInternal()
{
    mConfigPath = Util::ConvertWStringToString(PathUtil::GetMainPath() / "imgui.ini").c_str();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = mConfigPath.c_str();
    io.Fonts->AddFontFromFileTTF(Util::ConvertWStringToString(PathUtil::ConvertAbsolute("./Resources/Internals/Fonts/PixelMplus12-Regular.ttf")).c_str(), 16, NULL, io.Fonts->GetGlyphRangesJapanese());
    io.Fonts->Build();
    ImGui::StyleColorsDark();
    ImGui::GetStyle().FrameRounding = 5.0f;

    ImGui_ImplWin32_Init(RWindow::GetWindowHandle());

    mDevicePtr = RDirectX::GetDevice();
    mSrvHeapPtr = RDirectX::GetSRVHeap();
    ImGui_ImplDX12_Init(
        RDirectX::GetDevice(),
        static_cast<int32_t>(RDirectX::GetBackBufferSize()),
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        RDirectX::GetSRVHeap(),
        RDirectX::GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(),
        RDirectX::GetSRVHeap()->GetGPUDescriptorHandleForHeapStart()
    );
}

void RImGui::FinalInternal()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

//ImGui独自拡張

struct InputTextCallback_UserData {
    std::string* str;
    ImGuiInputTextCallback chainCallback;
    void* chainCallbackUserData;
};

static int32_t InputTextCallback(ImGuiInputTextCallbackData* data) {
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        std::string* str = user_data->str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->chainCallback)
    {
        data->UserData = user_data->chainCallbackUserData;
        return user_data->chainCallback(data);
    }
    return 0;
}

IMGUI_API bool ImGui::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.str = str;
    cb_user_data.chainCallback = callback;
    cb_user_data.chainCallbackUserData = user_data;
    return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

void ImGui::HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}