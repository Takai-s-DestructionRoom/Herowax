#include "Util.h"
#include <cassert>

using namespace std;

bool Util::debugBool = false;
int32_t Util::debugInt = 0;

std::chrono::high_resolution_clock::time_point Util::memTimePoint;
double Util::memElapsedTime = 0;

float Util::AngleToRadian(float angle)
{
	return angle * (PI / 180);
}

float Util::RadianToAngle(float radian)
{
	return radian * (180 / PI);
}

vector<string> Util::StringSplit(string s, string separator)
{
	vector<string> result;

	//区切り文字の長さが0なら区切るわけがないのでそのまま返す
	if (separator.length() == 0) {
		result.push_back(s);
		return result;
	}

	auto offset = string::size_type(0);
	while (1) {
		//区切り文字が出現する位置を取得
		auto find = s.find(separator, offset);

		//文字列の終点まで行ったら
		if (find == string::npos) {
			//残り全部加えておしまい
			result.push_back(s.substr(offset));
			break;
		}

		//そうでないなら区切って加える
		result.push_back(s.substr(offset, find - offset));

		//処理済みの所までイテレータを進める
		offset = find + separator.length();
	}

	return result;
}

int32_t Util::Clamp(int32_t i, int32_t min, int32_t max) {
	if (i < min) { return min; }
	if (i > max) { return max; }
	return i;
}

float Util::Clamp(float f, float min, float max) {
	if (f < min) { return min; }
	if (f > max) { return max; }
	return f;
}

double Util::Clamp(double d, double min, double max) {
	if (d < min) { return min; }
	if (d > max) { return max; }
	return d;
}

wstring Util::ConvertStringToWString(string str, uint32_t codePage) {
	//必要なwchar_t配列長を得る
	int32_t _arraySize = MultiByteToWideChar(codePage, 0, str.c_str()
		, -1, (wchar_t*)NULL, 0);

	//配列を用意する
	std::vector<wchar_t> wArray;
	wArray.resize(max(1, _arraySize));

	//変換してwchar_tの配列にぶち込む
	MultiByteToWideChar(codePage, 0, str.c_str(), -1, &wArray[0], _arraySize);

	//wstringにする
	std::wstring wStr(&wArray[0], &wArray.back());

	//おしまい
	return wStr;
}

std::string Util::ConvertWStringToString(std::wstring wstr, uint32_t codePage)
{
	//必要なchar配列長を得る
	int32_t _arraySize = WideCharToMultiByte(codePage, 0, wstr.c_str()
		, -1, (char*)NULL, 0, NULL, NULL);

	//配列を用意する
	std::vector<char> sArray;
	sArray.resize(max(1, _arraySize));

	//変換してchar_tの配列にぶち込む
	WideCharToMultiByte(codePage, 0, wstr.c_str(), -1, &sArray[0], _arraySize, NULL, NULL);

	//stringにする
	std::string str(&sArray[0], &sArray.back());

	//おしまい
	return str;
}

std::string Util::GetTimeString(float sec)
{
	int32_t hour = static_cast<int32_t>(floorf(sec / 3600));
	int32_t minute = static_cast<int32_t>(floorf((sec - 3600 * hour) / 60));
	float second = sec - 60 * minute - 3600 * hour;

	if (hour > 0) return Util::StringFormat("%d:%d:%.2f", hour, minute, second);
	return Util::StringFormat("%d:%05.2f", minute, second);
}

int32_t Util::GetRand(int32_t min, int32_t max)
{
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_int_distribution<> distr(min, max);
	return distr(eng);
}

float Util::GetRand(float min, float max)
{
	if (min >= max) {
		float min_ = max;
		min = max;
		max = min_;
	};
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> distr(min, max);
	return distr(eng);
}

double Util::GetRand(double min, double max)
{
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<double> distr(min, max);
	return distr(eng);
}

float Util::GetRatio(float a, float b, float p)
{
	if (a > b) {
		float temp = b;
		b = a;
		a = temp;
	}

	float size = b - a;
	if (size == 0) {
		return 1.0f;
	}
	return (p - a) / size;
}

void Util::CalcElapsedTimeStart() {
	memTimePoint = std::chrono::high_resolution_clock::now();
	memElapsedTime = 0;
}

void Util::CalcElapsedTimeEnd(std::string name, bool consecutive) {
	auto nowTimePoint = std::chrono::high_resolution_clock::now();
	double elapsedMicro = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(nowTimePoint - memTimePoint).count());
	if (consecutive) elapsedMicro -= memElapsedTime;
	double elapsedMili = elapsedMicro / 1000;
	
	OutputDebugStringA((name + ": " + StringFormat("%.3f", elapsedMili) + "ms / " + StringFormat("%.0f", elapsedMicro) + "us\n").c_str());
	memElapsedTime += elapsedMicro;
}

void Util::DebugLog(std::string log) {
	DebugLog(ConvertStringToWString(log));
}

void Util::DebugLog(std::wstring log)
{
#ifdef _DEBUG
	OutputDebugStringW((log + L"\n").c_str());
#endif
}

void Util::DebugLogC(std::string log)
{
	DebugLogC(ConvertStringToWString(log));
}

void Util::DebugLogC(std::wstring log)
{
	if (debugBool) {
		OutputDebugStringW((log + L"\n").c_str());
	}
}

Vector3 Util::GetRandVector3(const Vector3& value, float min, float max, const Vector3& factor)
{
	Vector3 temp = value;
	float posrand = GetRand(min, max);
	temp.x += posrand * factor.x;
	posrand = GetRand(min, max);
	temp.y += posrand * factor.y;
	posrand = GetRand(min, max);
	temp.z += posrand * factor.z;
	return temp;
}

Vector3 Util::Spline(const std::vector<Vector3>& points, float t)
{
	//ひとつしか入っていないならそのまま返す
	if (points.size() <= 1) { return Vector3(points.front()); }
	t = Util::Clamp(t, 0.f, 1.f);

	float perSegment = 1.f / (points.size() - 1);
	int32_t currentIndex = (int32_t)(t / perSegment);

	if (t == 1.0f)
	{
		currentIndex -= 1;
	}

	t = t - currentIndex * perSegment;
	t = t / perSegment;

	currentIndex -= 1;

	const Vector3& p0 = points.at(currentIndex < 0 ? 0 : currentIndex);
	const Vector3& p1 = points.at(currentIndex + 1);
	const Vector3& p2 = points.at(currentIndex + 2);
	const Vector3& p3 = points.at(currentIndex + 3 >= points.size() ? points.size() - 1 : currentIndex + 3);

	return (p1 * 2 + (-p0 + p2) * t +
		(p0 * 2 - p1 * 5 + p2 * 4 - p3) * t * t +
		(-p0 + p1 * 3 - p2 * 3 + p3) * t * t * t) * 0.5f;
}

bool Util::ContainString(const std::string& str, const std::string& check)
{
	return str.find(check) != std::string::npos;
}
