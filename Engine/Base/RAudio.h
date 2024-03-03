/*
* @file RAudio.h
* @brief XAudio2を制御するクラス
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <xaudio2.h>
#include <fstream>
#include <wrl.h>
#include <map>
#include <string>
#include <memory>
#include <list>
#include <mutex>
#pragma warning(pop)

#include "Util.h"

typedef std::string AudioHandle;
typedef std::string PlayingAudioHandle;

struct ChunkHeader
{
	char id[4]; //チャンクID
	int32_t size;
};

struct RiffHeader
{
	ChunkHeader chunk;
	char type[4];
};

struct FormatChunk
{
	ChunkHeader chunk;
	WAVEFORMATEX fmt;
};

enum class AudioType {
	Wave, Other
};

struct AudioData {
	std::string filepath;
	AudioType type{};
	uint32_t samplePlayBegin = 0;
	uint32_t samplePlayLength = 0;
	uint32_t sampleLoopBegin = 0;
	uint32_t sampleLoopLength = 0;
};

struct WaveAudio : public AudioData
{
	WAVEFORMATEX wfex{};
	std::vector<BYTE> buffer;
	uint32_t bufferSize = 0;
};

class RAudio
{
public:
	static RAudio* GetInstance() {
		static RAudio instance;
		return &instance;
	}

	static void Init() {
		GetInstance()->InitInternal();
	}

	static void Update();

	static void Final() {
		GetInstance()->FinalInternal();
	}

	static AudioHandle Load(const std::string filepath, std::string handle = "");

	static AudioData* GetData(AudioHandle handle);

	static void Play(AudioHandle handle, float volume = 1.0f, float pitch = 1.0f, bool loop = false);
	static void Stop(AudioHandle handle);
	static bool IsPlaying(AudioHandle handle);
	static float GetCurrentPosition(AudioHandle handle);
	static void SetVolume(AudioHandle handle, float volume);

	static float GetLength(AudioHandle handle);

	static void SetPlayRange(AudioHandle handle, float startPos, float endPos);
	static void SetLoopRange(AudioHandle handle, float startPos, float endPos);

	static size_t GetPlayingSoundCount() {
		return GetInstance()->mPlayingList.size();
	}

private:
	Microsoft::WRL::ComPtr<IXAudio2> mXAudio2;
	IXAudio2MasteringVoice* mMasteringVoice = nullptr;

	std::recursive_mutex mMutex;
	std::map<AudioHandle, std::shared_ptr<AudioData>> mAudioMap;
	std::map<PlayingAudioHandle, std::shared_ptr<AudioData>> mPlayingAudioMap;

public:
	struct PlayingInfo {
		AudioHandle handle;
		IXAudio2SourceVoice* pSource;
		bool loop = false;
	};
	static std::vector<PlayingInfo> GetPlayingInfo();
	static float GetCurrentPosition(PlayingInfo info);

private:
	std::list<PlayingInfo> mPlayingList;

	RAudio() {};
	~RAudio() = default;
	RAudio(const RAudio&) {};
	RAudio& operator=(const RAudio&) { return *this; }

	void InitInternal();
	void FinalInternal();
};

