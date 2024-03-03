#include "RAudio.h"
#include "PathUtil.h"
#include <cassert>

using namespace std;

AudioHandle RAudio::Load(const std::string filepath, std::string handle)
{
	RAudio* instance = GetInstance();

	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(filepath));

	std::unique_lock<std::recursive_mutex> lock(GetInstance()->mMutex);

	//一回読み込んだことがあるファイルはそのまま返す
	auto itr = find_if(instance->mAudioMap.begin(), instance->mAudioMap.end(), [&](const std::pair<AudioHandle, shared_ptr<AudioData>>& p) {
		return p.second->filepath == Util::ConvertWStringToString(path.c_str());
		});
	if (itr != instance->mAudioMap.end()) {
		return itr->first;
	}
	lock.unlock();

	std::ifstream file;
	file.open(path, std::ios_base::binary);

	if (file.fail()) {
		return "";
	}

	RiffHeader riff = {};
	file.read((char*)&riff, sizeof(riff));

	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		return "";
	}

	if (strncmp(riff.type, "WAVE", 4) != 0) {
		return "";
	}

	FormatChunk format = {};

	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		return "";
	}

	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	ChunkHeader data = {};
	file.read((char*)&data, sizeof(data));

	while (!file.fail() && strncmp(data.id, "data", 4) != 0) {
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}

	if (file.fail()) {
		return "";
	}

	shared_ptr<WaveAudio> sound = make_shared<WaveAudio>();
	sound->filepath = Util::ConvertWStringToString(path.c_str());
	sound->wfex = format.fmt;
	sound->bufferSize = data.size;

	sound->buffer.resize(data.size != 0 ? data.size : 1);
	file.read(reinterpret_cast<char*>(&sound->buffer[0]), data.size);

	file.close();

	if (handle.empty()) {
		handle = "NoNameHandle_" + Util::ConvertWStringToString(path.c_str());
	}

	lock.lock();
	GetInstance()->mAudioMap[handle] = sound;
	return handle;
}

AudioData* RAudio::GetData(AudioHandle handle)
{
	RAudio* instance = GetInstance();

	std::lock_guard<std::recursive_mutex> lock(instance->mMutex);
	if (instance->mAudioMap.find(handle) == instance->mAudioMap.end()) {
		return nullptr;
	}

	return instance->mAudioMap[handle].get();
}

void RAudio::Play(const AudioHandle handle, float volume, float pitch, bool loop)
{
	RAudio* instance = GetInstance();
	HRESULT result;

	std::lock_guard<std::recursive_mutex> lock(instance->mMutex);
	if (instance->mAudioMap.find(handle) == instance->mAudioMap.end()) {
		return;
	}

	shared_ptr<AudioData> data = instance->mAudioMap[handle];

	if (
		(data->samplePlayLength == 0 && data->samplePlayBegin != 0) // PlayLengthが0ならPlayBeginは0でなければならない
		&& (data->sampleLoopBegin >= data->samplePlayBegin + data->samplePlayLength) //LoopBeginはPlayBegin + PlayLength未満でなければならない
		&& (data->sampleLoopBegin + data->sampleLoopLength <= data->samplePlayBegin) //LoopBegin + LoopLengthはPlayBeginより大きくなければならない
		&& (data->sampleLoopBegin + data->sampleLoopLength >= data->samplePlayBegin + data->samplePlayLength) //LoopBegin + LoopLengthはPlayBegin + PlayLength未満でなければならない
		) {
		Util::DebugLog("ERROR: RAudio::Play() : Invalid Audio PlaySetting.");
		return;
	}

	if (data->type == AudioType::Wave) {
		shared_ptr<WaveAudio> waveData = static_pointer_cast<WaveAudio>(data);

		IXAudio2SourceVoice* pSourceVoice = nullptr;
		result = instance->mXAudio2->CreateSourceVoice(&pSourceVoice, &waveData->wfex);
		assert(SUCCEEDED(result));

		XAUDIO2_BUFFER buf{};
		buf.pAudioData = &waveData->buffer[0];
		buf.AudioBytes = waveData->bufferSize;
		buf.PlayBegin = waveData->samplePlayBegin;
		buf.PlayLength = waveData->samplePlayLength;
		if (loop) {
			buf.LoopBegin = waveData->sampleLoopBegin;
			buf.LoopLength = waveData->sampleLoopLength;
		}
		buf.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
		buf.Flags = XAUDIO2_END_OF_STREAM;

		result = pSourceVoice->SubmitSourceBuffer(&buf);
		assert(SUCCEEDED(result));
		result = pSourceVoice->SetVolume(volume);
		assert(SUCCEEDED(result));
		result = pSourceVoice->SetFrequencyRatio(pitch);
		assert(SUCCEEDED(result));
		result = pSourceVoice->Start();
		assert(SUCCEEDED(result));

		instance->mPlayingList.push_back({ handle, pSourceVoice, loop });
	}
}

void RAudio::Stop(AudioHandle handle)
{
	RAudio* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mMutex);
	for (auto itr = instance->mPlayingList.begin(); itr != instance->mPlayingList.end();) {
		PlayingInfo info = *itr;
		if (info.handle == handle) {
			info.pSource->Stop();
			info.pSource->DestroyVoice();
			itr = instance->mPlayingList.erase(itr);
			continue;
		}
		itr++;
	}
}

void RAudio::Update()
{
	RAudio* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mMutex);
	for (auto itr = instance->mPlayingList.begin(); itr != instance->mPlayingList.end();) {
		PlayingInfo info = *itr;
		XAUDIO2_VOICE_STATE state{};
		info.pSource->GetState(&state);
		if (state.BuffersQueued == 0) {
			itr = instance->mPlayingList.erase(itr);
			continue;
		}
		itr++;
	}
}

bool RAudio::IsPlaying(AudioHandle handle)
{
	RAudio* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mMutex);
	for (auto itr = instance->mPlayingList.begin(); itr != instance->mPlayingList.end();) {
		PlayingInfo info = *itr;
		if (info.handle == handle) {
			XAUDIO2_VOICE_STATE state{};
			info.pSource->GetState(&state);
			if (state.BuffersQueued == 0) {
				itr = instance->mPlayingList.erase(itr);
				continue;
			}
			return true;
		}
		itr++;
	}
	return false;
}

float RAudio::GetCurrentPosition(AudioHandle handle) {
	RAudio* instance = GetInstance();

	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mMutex);
	if (instance->mAudioMap.find(handle) == instance->mAudioMap.end()) {
		Util::DebugLog("ERROR: RAudio::GetCurrentPosition() : Audio[" + handle + "] is not found.");
		return 0;
	}

	shared_ptr<AudioData> data = instance->mAudioMap[handle];
	uint32_t samplePerSec = 0;
	if (data->type == AudioType::Wave) {
		shared_ptr<WaveAudio> waveData = static_pointer_cast<WaveAudio>(data);
		samplePerSec = waveData->wfex.nSamplesPerSec;
	}
	else {
		Util::DebugLog("ERROR: RAudio::SetPlayRange() : Audio[" + handle + "] is unknown AudioType.");
		return 0;
	}
	
	for (auto itr = instance->mPlayingList.begin(); itr != instance->mPlayingList.end();) {
		PlayingInfo info = *itr;
		if (info.handle == handle) {
			XAUDIO2_VOICE_STATE state{};
			info.pSource->GetState(&state);
			if (state.BuffersQueued == 0) {
				itr = instance->mPlayingList.erase(itr);
				continue;
			}

			uint64_t totalSample = state.SamplesPlayed;

			if (info.loop) {
				totalSample += data->samplePlayBegin;

				while (totalSample >= data->sampleLoopBegin + data->sampleLoopLength) {
					totalSample -= data->sampleLoopLength;
				}
				return totalSample / static_cast<float>(samplePerSec);
			}
			else {
				return (data->samplePlayBegin + totalSample) / static_cast<float>(samplePerSec);
			}
		}
		itr++;
	}
	return 0;
}

void RAudio::SetVolume(AudioHandle handle, float volume)
{
	RAudio* instance = GetInstance();

	for (auto itr = instance->mPlayingList.begin(); itr != instance->mPlayingList.end();) {
		PlayingInfo info = *itr;
		if (info.handle == handle) {
			info.pSource->SetVolume(volume);
		}
		itr++;
	}
}

float RAudio::GetLength(AudioHandle handle)
{
	RAudio* instance = GetInstance();

	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mMutex);
	if (instance->mAudioMap.find(handle) == instance->mAudioMap.end()) {
		Util::DebugLog("ERROR: RAudio::GetCurrentPosition() : Audio[" + handle + "] is not found.");
		return 0;
	}

	shared_ptr<AudioData> data = instance->mAudioMap[handle];
	if (data->type == AudioType::Wave) {
		shared_ptr<WaveAudio> waveData = static_pointer_cast<WaveAudio>(data);
		if (waveData->wfex.nAvgBytesPerSec == 0) return 0;
		return waveData->bufferSize / static_cast<float>(waveData->wfex.nAvgBytesPerSec);
	}
	else {
		Util::DebugLog("ERROR: RAudio::GetLength() : Audio[" + handle + "] is unknown AudioType.");
		return 0;
	}
}

void RAudio::SetPlayRange(AudioHandle handle, float startPos, float endPos)
{
	if (endPos != 0 && startPos >= endPos) return;

	RAudio* instance = GetInstance();

	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mMutex);
	if (instance->mAudioMap.find(handle) == instance->mAudioMap.end()) {
		Util::DebugLog("ERROR: RAudio::SetPlayRange() : Audio[" + handle + "] is not found.");
		return;
	}

	shared_ptr<AudioData> data = instance->mAudioMap[handle];

	if (data->type == AudioType::Wave) {
		shared_ptr<WaveAudio> waveData = static_pointer_cast<WaveAudio>(data);

		waveData->samplePlayBegin = static_cast<uint32_t>(startPos * waveData->wfex.nSamplesPerSec);
		
		if (endPos == 0) {
			waveData->samplePlayLength = (waveData->bufferSize / (waveData->wfex.wBitsPerSample / 8) / waveData->wfex.nChannels) - waveData->samplePlayBegin;
		}
		else {
			waveData->samplePlayLength = static_cast<uint32_t>((endPos - startPos) * waveData->wfex.nSamplesPerSec);
		}
		return;
	}
	Util::DebugLog("ERROR: RAudio::SetPlayRange() : Audio[" + handle + "] is unknown AudioType.");
}

void RAudio::SetLoopRange(AudioHandle handle, float startPos, float endPos)
{
	if (endPos != 0 && startPos >= endPos) return;

	RAudio* instance = GetInstance();

	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mMutex);
	if (instance->mAudioMap.find(handle) == instance->mAudioMap.end()) {
		Util::DebugLog("ERROR: RAudio::SetLoopRange() : Audio[" + handle + "] is not found.");
		return;
	}

	shared_ptr<AudioData> data = instance->mAudioMap[handle];

	if (data->type == AudioType::Wave) {
		shared_ptr<WaveAudio> waveData = static_pointer_cast<WaveAudio>(data);

		waveData->sampleLoopBegin = static_cast<uint32_t>(startPos * waveData->wfex.nSamplesPerSec);

		if (endPos == 0) {
			waveData->sampleLoopLength = (waveData->bufferSize / (waveData->wfex.wBitsPerSample / 8) / waveData->wfex.nChannels) - waveData->sampleLoopBegin;
		}
		else {
			waveData->sampleLoopLength = static_cast<uint32_t>((endPos - startPos) * waveData->wfex.nSamplesPerSec);
		}
		return;
	}
	Util::DebugLog("ERROR: RAudio::SetLoopRange() : Audio[" + handle + "] is unknown AudioType.");
}

std::vector<RAudio::PlayingInfo> RAudio::GetPlayingInfo()
{
	std::vector<PlayingInfo> result;

	for (auto itr = RAudio::GetInstance()->mPlayingList.begin(); itr != RAudio::GetInstance()->mPlayingList.end(); itr++) {
		result.push_back(*itr);
	}

	return result;
}

float RAudio::GetCurrentPosition(PlayingInfo info)
{
	RAudio* instance = GetInstance();

	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mMutex);

	shared_ptr<AudioData> data = instance->mAudioMap[info.handle];
	uint32_t samplePerSec = 0;
	if (data->type == AudioType::Wave) {
		shared_ptr<WaveAudio> waveData = static_pointer_cast<WaveAudio>(data);
		samplePerSec = waveData->wfex.nSamplesPerSec;
	}
	else {
		Util::DebugLog("ERROR: RAudio::GetCurrentPosition() : Audio[" + info.handle + "] is unknown AudioType.");
		return 0;
	}

	XAUDIO2_VOICE_STATE state{};
	info.pSource->GetState(&state);
	if (state.BuffersQueued == 0) {
		return 0;
	}

	uint64_t totalSample = state.SamplesPlayed;

	if (info.loop) {
		totalSample += data->samplePlayBegin;

		while (totalSample >= data->sampleLoopBegin + data->sampleLoopLength) {
			totalSample -= data->sampleLoopLength;
		}
		return totalSample / static_cast<float>(samplePerSec);
	}
	else {
		return (data->samplePlayBegin + totalSample) / static_cast<float>(samplePerSec);
	}
	return 0;
}

void RAudio::InitInternal()
{
	HRESULT result;
	result = XAudio2Create(&mXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	result = mXAudio2->CreateMasteringVoice(&mMasteringVoice);
	assert(SUCCEEDED(result));
}

void RAudio::FinalInternal()
{
	mMasteringVoice->DestroyVoice();
	mXAudio2.Reset();
	mAudioMap.clear();
	mPlayingAudioMap.clear();
	mPlayingList.clear();
}
