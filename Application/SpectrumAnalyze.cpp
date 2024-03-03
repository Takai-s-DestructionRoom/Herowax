#include "SpectrumAnalyze.h"
#include <Util.h>
#include <RAudio.h>

std::vector<std::complex<float>> FFT(const std::vector<float>& data)
{
	//分からんすぎたのでいったんコピペ借り

	//虚数単位
	const std::complex<float>Imaginary(0, 1);
	//累乗を求める
	unsigned int exponent = int(std::ceil(std::log2(data.size())));

	//2の累乗の数
	unsigned int num = int(std::pow(2.0, exponent));

	//計算用バッファ
	std::vector<std::complex<float>>buf(num, 0);

	//ビット反転
	std::vector<unsigned int>index(buf.size());
	for (size_t i = 0; i < index.size(); ++i)
	{
		unsigned int tmp = static_cast<uint32_t>(i);
		for (unsigned int n = 0; n < exponent; ++n)
		{
			index[i] <<= 1;
			index[i] |= (tmp >> n) & 0x0001;
		}
		//渡されたデータの並びを変える
		if (index[i] < data.size())
		{
			buf[i] = data[index[i]];
		}
		else
		{
			//データ数を超えたら0埋め
			buf[i] = 0.0;
		}
	}

	//バタフライ演算
	for (unsigned int stage = 1; stage <= exponent; ++stage)
	{
		for (unsigned int i = 0; i < std::powf(2.0f, static_cast<float>(exponent - stage)); ++i)
		{
			for (unsigned int n = 0; n < std::powf(2.0f, static_cast<float>(stage - 1)); ++n)
			{
				std::complex<float> corre1 = std::exp(-Imaginary * 2.0f * std::acosf(-1.0f) * float(n) / std::powf(2.0f, static_cast<float>(stage)));
				std::complex<float> corre2 = std::exp(-Imaginary * 2.0f * std::acosf(-1.0f) * float(n + std::pow(2.0f, static_cast<float>(stage - 1))) / std::powf(2.0f, static_cast<float>(stage)));

				unsigned int No1 = int(i * std::powf(2.0f, static_cast<float>(stage)) + n);
				unsigned int No2 = int(No1 + std::powf(2.0f, static_cast<float>(stage - 1)));

				std::complex<float> tmp1 = buf[No1];
				std::complex<float> tmp2 = buf[No2];

				buf[No1] = tmp1 + (tmp2 * corre1);
				buf[No2] = tmp1 + (tmp2 * corre2);
			}
		}
	}
	return buf;
}

float Window(int i, int N) {
	//return 0.5f - 0.5f * cosf(2 * Util::PI * i / (N - 1));
	return 0.42f - 0.5f * cosf((2 * Util::PI * i) / (N - 1)) + 0.08f * cosf((4 * Util::PI * i) / (N - 1));
	//return 0.54f - 0.46f * cosf(2 * Util::PI * i / (N-1));
}

std::vector<float> SpectrumAnalyze(AudioData* data, int32_t currentSamplePos, int32_t sampleNum) {
	WaveAudio* audio = static_cast<WaveAudio*>(data);

	std::vector<float> freqData;

	for (int i = 0; i < sampleNum; i++) {
		int32_t sample = currentSamplePos + i;
		int32_t block = audio->wfex.nBlockAlign * sample;
		BYTE* nowByte = nullptr;

		float freq = 0;

		if (block >= 0 && block < audio->buffer.size()) {
			nowByte = &audio->buffer[block];

			if (audio->wfex.wFormatTag == WAVE_FORMAT_PCM) {
				int16_t* tfreq = reinterpret_cast<int16_t*>(nowByte);
				freq = *tfreq / static_cast<float>(SHORT_MAX);
			}

			if (audio->wfex.wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
				float* tfreq = reinterpret_cast<float*>(nowByte);
				freq = *tfreq;
			}
		}

		//freqData.push_back(freq);
		freqData.push_back(freq * Window(i, sampleNum));
	}

	std::vector<std::complex<float>> fftData = FFT(freqData);

	std::vector<float> spectrum;

	for (int32_t i = 0; i < sampleNum / 2; i++) {
		float real = fftData[i].real() / (sampleNum / 2.0f);
		float imag = fftData[i].imag() / (sampleNum / 2.0f);
		float magnitude = sqrtf(real * real + imag * imag);
		//float freq = i * static_cast<float>(audio->wfex.nSamplesPerSec) / sampleNum;
		spectrum.push_back(magnitude);
	}

	return spectrum;
}