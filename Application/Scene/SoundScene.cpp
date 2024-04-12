#include "SoundScene.h"
#include "RImGui.h"
#include "RInput.h"
#include <Quaternion.h>
#include <RAudio.h>
#include <SimpleDrawer.h>
#include <SpectrumAnalyze.h>

SoundScene::SoundScene()
{
	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void SoundScene::Init()
{
	RAudio::Load("Resources/Internals/testscale.wav", "TestScale");
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void SoundScene::Update()
{
	AudioData* data = RAudio::GetData("TestScale");
	WaveAudio* audio = static_cast<WaveAudio*>(data);

	float height = 150;
	int32_t sampleRange = 512;
	int32_t sampleStep = 1;
	int32_t nowSample = static_cast<int32_t>(RAudio::GetCurrentPosition("TestScale") * audio->wfex.nSamplesPerSec);

	float x = FLT_MAX;
	float y1 = FLT_MAX;
	float y2 = FLT_MAX;
	float v = FLT_MAX;
	float volumeMem = 0;
	float volumeDiffMem = 0;
	for (int i = 0; i < sampleRange; i += sampleStep) {
		int32_t sample = nowSample - sampleRange / 2 + i;
		int32_t block = audio->wfex.nBlockAlign * sample;
		BYTE* nowByte = nullptr;

		float freq1 = 0;
		float freq2 = 0;
		float volume = 0;

		if (block >= 0 && block < audio->buffer.size()) {
			nowByte = &audio->buffer[block];

			if (audio->wfex.wFormatTag == WAVE_FORMAT_PCM) {
				int16_t* freq = reinterpret_cast<int16_t*>(nowByte);
				freq1 = *freq / static_cast<float>(SHORT_MAX);
				freq++;
				freq2 = *freq / static_cast<float>(SHORT_MAX);

				volume = (abs(freq1) + abs(freq2)) / 2.0f;
			}

			if (audio->wfex.wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
				float* freq = reinterpret_cast<float*>(nowByte);
				freq1 = *freq;
				freq++;
				freq2 = *freq;

				volume = (abs(freq1) + abs(freq2)) / 2.0f;
			}
		}

		float volumediff = max(0, volume - volumeMem);
		volumeMem = volume;
		if (volumediff == 0) {
			volumeDiffMem = 0;
		}
		else {
			volumediff = volumeDiffMem + volumediff;
			volumeDiffMem = volumediff;
		}

		float areaSize = height / 2;

		float area1 = 360;
		float area2 = 200 + (areaSize + 50);

		float pPos = static_cast<float>(RWindow::GetWidth() / static_cast<float>(sampleRange) * i);
		float wPos1 = area1 - areaSize * freq1;
		float wPos2 = area2 - areaSize * freq2;
		float vPos1 = area1 + areaSize / 2 - areaSize * volumediff;

		if (x == FLT_MAX) {
			SimpleDrawer::DrawLine(
				-1,
				static_cast<int32_t>(area1),
				static_cast<int32_t>(pPos),
				static_cast<int32_t>(wPos1), 100, 0xff0000);
			/*SimpleDrawer::DrawLine(
				-1,
				static_cast<int32_t>(area2),
				static_cast<int32_t>(pPos),
				static_cast<int32_t>(wPos2), 100, 0xff0000);*/
			//SimpleDrawer::DrawLine(
			//	-1,
			//	static_cast<int32_t>(area1),
			//	static_cast<int32_t>(pPos),
			//	static_cast<int32_t>(vPos1), 105, 0x00ffff);
		}
		else {
			SimpleDrawer::DrawLine(
				static_cast<int32_t>(x),
				static_cast<int32_t>(y1),
				static_cast<int32_t>(pPos),
				static_cast<int32_t>(wPos1), 100, 0xff0000);
			/*SimpleDrawer::DrawLine(
				static_cast<int32_t>(x),
				static_cast<int32_t>(y2),
				static_cast<int32_t>(pPos),
				static_cast<int32_t>(wPos2), 100, 0xff0000);*/
			//SimpleDrawer::DrawLine(
			//	static_cast<int32_t>(x),
			//	static_cast<int32_t>(v),
			//	static_cast<int32_t>(pPos),
			//	static_cast<int32_t>(vPos1), 105, 0x00ffff);
		}
		x = pPos;
		y1 = wPos1;
		y2 = wPos2;
		v = vPos1;
	}

	SimpleDrawer::DrawLine(640, 0, 640, 720, 100, 0xffffff);

	std::vector<float> spectrum = SpectrumAnalyze(data, static_cast<int32_t>(RAudio::GetCurrentPosition("TestScale") * audio->wfex.nSamplesPerSec), 8192);
	//std::vector<float> spectrum = SpectrumAnalyze(data, 0, 8192);

	int32_t slashFreq = 200;
	std::vector<float> peakMax(slashFreq);

	for (int32_t i = 0; i < spectrum.size(); i++) {
		float magnitude = spectrum[i];
		float freq = i * static_cast<float>(audio->wfex.nSamplesPerSec) / 8192;
		for (int32_t m = 0; m < slashFreq; m++) {
			float borderA = m * (1600.0f / slashFreq);
			float borderB = (m+1) * (1600.0f / slashFreq);

			if (freq > borderA && freq <= borderB) {
				if (magnitude > peakMax[m]) {
					peakMax[m] = magnitude;
				}
				break;
			}
			else if (m == slashFreq - 1) {
				if (magnitude > peakMax[m]) {
					peakMax[m] = magnitude;
				}
			}
		}
	}

	SimpleDrawer::DrawLine(0, 620, 1280, 620, 500, 0xffffff);
	for (int i = 0; i < peakMax.size(); i++) {
		float x1 = ((1280.0f / peakMax.size()) * i) + 1;
		float x2 = ((1280.0f / peakMax.size()) * (i + 1)) - 1;
		float y = 0;

		if (peakMax[i] > 0.00000000001f) {
			//y = (120.0f + (log(peakMax[i]) * 10.0f)) / 120.0f * 100;
			y = (1 - (20 * log10(peakMax[i])) / -60.0f) * 100;
			//y = 100 * peakMax[i];
		}
		else {
			y = 0;
		}

		SimpleDrawer::DrawBox(x1, 720.0f - y, x2, 720.0f, 200, 0x00ff00, true);
	}

	//テストGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 300 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("Sound Control");

		if (ImGui::Button("Reset")) {
			loop = false;
			playRange = { 0, 0 };
			loopRange = { 0, 0 };
		}

		ImGui::Separator();

		ImGui::Checkbox("Loop", &loop);
		ImGui::InputFloat("PlayStart", &playRange.x);
		ImGui::InputFloat("PlayEnd", &playRange.y);
		ImGui::InputFloat("LoopStart", &loopRange.x);
		ImGui::InputFloat("LoopEnd", &loopRange.y);
		ImGui::Text(Util::StringFormat("IsPlaying:%d", RAudio::IsPlaying("TestScale")).c_str());
		ImGui::Text(Util::StringFormat("Current:%f", RAudio::GetCurrentPosition("TestScale")).c_str());

		if (ImGui::Button("Play")) {
			RAudio::SetPlayRange("TestScale", playRange.x, playRange.y);
			RAudio::SetLoopRange("TestScale", loopRange.x, loopRange.y);
			RAudio::Play("TestScale", 1, 1, loop);
		}
		if (ImGui::Button("Stop")) {
			RAudio::Stop("TestScale");
		}

		ImGui::DragFloat("Time", &time);
		ImGui::Text(Util::StringFormat("%d", nowSample).c_str());
		//ImGui::Text(Util::StringFormat("%d", nowBlock).c_str());
		//ImGui::Text(Util::StringFormat("%p", nowByte).c_str());
		//ImGui::Text(Util::StringFormat("%d", freqD1).c_str());
		//ImGui::Text(Util::StringFormat("%d", freqD2).c_str());
		//ImGui::Text(Util::StringFormat("%f", freqF1).c_str());
		//ImGui::Text(Util::StringFormat("%f", freqF2).c_str());
		ImGui::Spacing();
		ImGui::Text(Util::StringFormat("cbSize:%d", audio->wfex.cbSize).c_str());
		ImGui::Text(Util::StringFormat("nSamplesPerSec:%d", audio->wfex.nSamplesPerSec).c_str());
		ImGui::Text(Util::StringFormat("wBitsPerSample:%d", audio->wfex.wBitsPerSample).c_str());
		ImGui::Text(Util::StringFormat("nChannels:%d", audio->wfex.nChannels).c_str());
		ImGui::Text(Util::StringFormat("nBlockAlign:%d", audio->wfex.nBlockAlign).c_str());

		ImGui::End();
	}
}

void SoundScene::Draw()
{
}
