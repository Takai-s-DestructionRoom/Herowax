#include "DebugGUI.h"
#include "RImGui.h"
#include <SRBuffer.h>
#include <Util.h>
#include <SceneManager.h>
#include <MainTestScene.h>
#include <ControllerScene.h>
#include <SoundScene.h>
#include <SimpleSceneTransition.h>
#include <Renderer.h>
#include <SpriteScene.h>
#include <RAudio.h>
#include <PaintScene.h>
#include <ParticleEditorScene.h>
#include <ProtoScene.h>

void DebugGUI::Show()
{
	ImGui::ShowDemoWindow();
	ImGui::SetNextWindowSize({ 400, 380 }, ImGuiCond_FirstUseEver);

	ImGui::Begin("Debug");

	ImGui::Text("SceneManager");
	static int32_t sceneNum = 0;
	const char* scenes[] = { "MainTest", "ControllerTest", "Sprite", "SoundTest","PaintTest","ProtoScene","ParticleEditorScene"};
	ImGui::Combo("##SceneNumCombo", &sceneNum, scenes, IM_ARRAYSIZE(scenes));
	ImGui::SameLine();
	if (ImGui::Button("Go!!!")) {
		if (!SceneManager::IsSceneChanging()) {
			switch (sceneNum) {
			case 0:
				SceneManager::Change<MainTestScene, SimpleSceneTransition>();
				break;
			case 1:
				SceneManager::Change<ControllerScene, SimpleSceneTransition>();
				break;
			case 2:
				SceneManager::Change<SpriteScene, SimpleSceneTransition>();
				break;
			case 3:
				SceneManager::Change<SoundScene, SimpleSceneTransition>();
				break;
			case 4:
				SceneManager::Change<PaintScene, SimpleSceneTransition>();
				break;
			case 5:
				SceneManager::Change<ProtoScene, SimpleSceneTransition>();
				break;
			case 6:
				SceneManager::Change<ParticleEditorScene, SimpleSceneTransition>();
				break;
			}
		}
	}

	ImGui::NewLine();
	ImGui::Separator();

	if (ImGui::BeginTabBar("DebugTabBar")) {
		if (ImGui::BeginTabItem("Info")) {
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Spacing();
			ImGui::Text("MousePos:(%.1f,%.1f)", ImGui::GetMousePos().x, ImGui::GetMousePos().y);
			ImGui::Spacing();

			ImGui::NewLine();
			ImGui::Spacing();
			ImGui::Text("DebugCamera Control");
			ImGui::Text("* Valid only in available scenes.");
			ImGui::Text("Left Control Key : Enable / Disable");
			ImGui::Text("WASD Key : Move");
			ImGui::Text("Space Key : Up");
			ImGui::Text("LShift Key : Down");
			ImGui::Text("Mouse : Direction");

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Buffer")) {
			UINT64 buffUsingSize = SRBufferAllocator::GetUsingBufferSize();
			UINT64 buffTotalSize = SRBufferAllocator::GetBufferSize();
			std::string buffUsingSizeStr;
			std::string buffTotalSizeStr;
			if (buffUsingSize < 1024 * 1024 - 1) {
				buffUsingSizeStr = Util::StringFormat("%.2fKB", buffUsingSize / 1024.0f);
			}
			else {
				buffUsingSizeStr = Util::StringFormat("%.2fMB", buffUsingSize / 1024.0f / 1024.0f);
			}
			if (buffTotalSize < 1024 * 1024 - 1) {
				buffTotalSizeStr = Util::StringFormat("%.2fKB", buffTotalSize / 1024.0f);
			}
			else {
				buffTotalSizeStr = Util::StringFormat("%.2fMB", buffTotalSize / 1024.0f / 1024.0f);
			}
			ImGui::Text("SRBuffer");
			ImGui::Text(("Using : " + buffUsingSizeStr + " / " + buffTotalSizeStr + "(%.2lf%%)").c_str(), static_cast<double>(buffUsingSize) / buffTotalSize * 100);
			ImGui::Text("(Raw : %lld / %lld)", buffUsingSize, buffTotalSize);
			ImGui::Text(Util::StringFormat("UsingRegion: %lld", SRBufferAllocator::GetUsingRegionCount()).c_str());
			ImGui::Text(Util::StringFormat("FreeRegion: %lld", SRBufferAllocator::GetFreeRegionCount()).c_str());
			ImGui::ProgressBar(static_cast<float>(buffUsingSize) / buffTotalSize);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Renderer")) {
			// Left
			std::vector<std::string> stages = Renderer::GetRenderStageIDs();
			static int32_t selected = 0;
			{
				ImVec2 parentSize = ImGui::GetWindowSize();
				ImGui::BeginChild("left pane", ImVec2(parentSize.x / 3.0f, 0), true, ImGuiWindowFlags_AlwaysAutoResize);
				for (int32_t i = 0; i < stages.size(); i++)
				{
					IRenderStage* stage = Renderer::GetRenderStage(stages[i]);
					if (ImGui::Selectable(Util::StringFormat("##%s", stage->GetTypeIndentifier().c_str()).c_str(), selected == i)) {
						selected = i;
					}
					ImGui::SameLine();
					if (stage->mFlagEnabled) {
						ImGui::Text((stage->GetTypeIndentifier() + Util::StringFormat(" (%d)", stage->mOrders.size())).c_str());
					}
					else {
						ImGui::TextDisabled((stage->GetTypeIndentifier() + Util::StringFormat(" (%d)", stage->mOrders.size())).c_str());
					}
				}
				ImGui::EndChild();
			}
			ImGui::SameLine();

			// Right
			{
				IRenderStage* stage = Renderer::GetRenderStage(stages[selected]);
				ImGui::BeginGroup();
				ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
				ImGui::Text(stage->GetTypeIndentifier().c_str());
				ImGui::Separator();
				ImGui::Checkbox("Enable", &stage->mFlagEnabled);
				ImGui::Text(Util::StringFormat("OrderCount:%d", stage->mOrders.size()).c_str());
				ImGui::EndChild();
				ImGui::EndGroup();
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Audio")) {
			// Left
			std::vector<RAudio::PlayingInfo> audios = RAudio::GetPlayingInfo();
			static int32_t selected = -1;
			{
				ImVec2 parentSize = ImGui::GetWindowSize();
				ImGui::BeginChild("left pane", ImVec2(parentSize.x / 3.0f, 0), true, ImGuiWindowFlags_AlwaysAutoResize);
				for (int32_t i = 0; i < audios.size(); i++)
				{
					RAudio::PlayingInfo audio = audios[i];
					if (ImGui::Selectable(Util::StringFormat("##%p", audio.pSource).c_str(), selected == i)) {
						selected = i;
					}
					ImGui::SameLine();
					ImGui::Text(audio.handle.c_str());
				}
				ImGui::EndChild();
			}
			ImGui::SameLine();

			// Right
			if (selected >= audios.size()) {
				selected = static_cast<int32_t>(audios.size() - 1);
			}
			if (selected >= 0) {
				RAudio::PlayingInfo audio = audios[selected];
				ImGui::BeginGroup();
				ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
				ImGui::Text(audio.handle.c_str());
				ImGui::Separator();
				float pos = RAudio::GetCurrentPosition(audio);
				float min = floor(pos / 60.0f);
				float sec = floor(pos - min * 60);
				int32_t mili = static_cast<int32_t>((pos - min * 60 - sec) * 1000);
				ImGui::Text(Util::StringFormat("%0.0f:%02.0f.%03d", min, sec, mili).c_str());
				ImGui::Text(Util::StringFormat("Loop:%d", audio.loop).c_str());
				if (ImGui::Button("Stop##item view button")) {
					audio.pSource->Stop();
					audio.pSource->FlushSourceBuffers();
				}
				ImGui::EndChild();
				ImGui::EndGroup();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	
	ImGui::End();
}
