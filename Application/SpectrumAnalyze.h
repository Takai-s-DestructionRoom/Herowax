#pragma once
#include <vector>
#include <complex>
#include <RAudio.h>

std::vector<std::complex<float>> FFT(const std::vector<float>& data);
std::vector<float> SpectrumAnalyze(AudioData* data, int32_t currentSamplePos, int32_t sampleNum);