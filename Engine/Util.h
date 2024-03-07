/*
* @file Util.h
* @brief べんり！
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <string>
#include <vector>
#include <list>
#include <stdexcept>
#include <random>
#include <chrono>
#include <windows.h>
#include "Vector3.h"
#pragma warning(pop)

namespace Util {
	extern bool debugBool;
    extern int32_t debugInt;
	constexpr float PI = 3.1415926535897932384626f;

    const int32_t WIN_WIDTH = 1280;
    const int32_t WIN_HEIGHT = 720;

    extern std::chrono::high_resolution_clock::time_point memTimePoint;
    extern double memElapsedTime;

	//DegreeからRadianへ変換する
	float AngleToRadian(float angle);

	//RadianからDegreeへ変換する
	float RadianToAngle(float radian);

    /*
    printfみたいに
    フォーマット指定子を含む文字列(X = %dみたいなの)と
    引数を好きなだけ入れると
    その結果の文字列を返す
    例:
    StringFormat("X = %d, Y = %d", 100, 500); -> "X = 100, Y = 500"
*/
    template<typename ... Args>
    std::string StringFormat(const std::string& format, Args&& ... args) {
        /* フォーマット後の文字数を算出 */
        int32_t str_len = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args) ...);

        /* フォーマット失敗 */
        if (str_len < 0)
        {
            throw std::runtime_error("String Formatting Error");
        }

        /* バッファサイズを算出(文字列長 + null文字サイズ) */
        size_t bufferSize = str_len + sizeof(char);

        /* バッファサイズ分メモリ確保 */
        std::vector<char> buffer;
        buffer.resize(bufferSize);

        /* 文字列のフォーマット */
        std::snprintf(&buffer[0], bufferSize, format.c_str(), args ...);

        /* 文字列をstd::string型に変換して出力 */
        return std::string(&buffer[0], &buffer[0] + str_len);
    }

    /*
        Javaでいうinstanceofとほぼ一緒！
        instanceof<int>(a)みたいな書き方でどうぞ
    */
    template <typename check, typename NANIYATU>
    inline bool instanceof(const NANIYATU& a)
    {
        return dynamic_cast<const check*>(&a) != 0;
    }

    /*
        std::stringにsplitがないので
        自前でsplitできるやつ
    */
    std::vector<std::string> StringSplit(std::string s, std::string separator);

    //指定したintがminからmaxの間に収まっていればそのまま、
    //収まっていなければ収めて返す
    int32_t Clamp(int32_t i, int32_t min, int32_t max);

    //指定したfloatがminからmaxの間に収まっていればそのまま、
    //収まっていなければ収めて返す
    float Clamp(float f, float min, float max);

    //指定したdoubleがminからmaxの間に収まっていればそのまま、
    //収まっていなければ収めて返す
    double Clamp(double d, double min, double max);

    //std::vectorにsearchが含まれているか調べる
    template <class T>
    bool Contains(std::vector<T> vector, T search) {
        for (auto itr = vector.begin(); itr != vector.end(); itr++) {
            if (*itr == search) {
                return true;
            }
        }
        return false;
    }

    //std::listにsearchが含まれているか調べる
    template <class T>
    bool Contains(std::list<T> list, T search) {
        auto itr = std::find(list.begin(), list.end(), search);
        return itr != list.end();
    }

    //std::vectorにsearchがいるならそのインデックスを得る
    //ないなら-1
    template <class T>
    int32_t IndexOf(std::vector<T> vector, T search) {
        auto itr = std::find(vector.begin(), vector.end(), search);
        if (itr == vector.end()) {
            return -1;
        }
        return static_cast<int32_t>(std::distance(vector.begin(), itr));
    }

    //stringをwstringに変換する（MultiByteToWideChar)
    std::wstring ConvertStringToWString(std::string str, uint32_t codePage = CP_UTF8);

    //wstringをstringに変換する（WideCharToMultiByte)
    std::string ConvertWStringToString(std::wstring wstr, uint32_t codePage = CP_UTF8);
    
    //秒を00:00.00形式のstringで表します
    std::string GetTimeString(float sec);

    //乱数生成(一様分布)
    int32_t GetRand(int32_t min, int32_t max);

    //乱数生成(一様分布)
    float GetRand(float min, float max);

    //乱数生成(一様分布)
    double GetRand(double min, double max);

    /// <summary>
    /// a~bの範囲内で、どの割合にpが位置しているかを返す
    /// e.x. a=2,b=6,p=4 なら return 0.5f;
    /// </summary>
    /// <returns>割合</returns>
    float GetRatio(float a, float b, float p);

    //処理時間計測開始
    void CalcElapsedTimeStart();
    //処理時間計測出力
    void CalcElapsedTimeEnd(std::string name, bool consecutive = true);

    //デバッグ出力(デバッグビルドでのみ処理)
    void DebugLog(std::string log);

    //デバッグ出力(デバッグビルドでのみ処理)
    void DebugLog(std::wstring log);

    //デバッグ出力(debugBoolが立っている時処理)
    void DebugLogC(std::string log);

    //デバッグ出力(debugBoolが立っている時処理)
    void DebugLogC(std::wstring log);

    /// <summary>
    /// Spline挙動を作る
    /// </summary>
    /// <param name="points"></param>
    /// <param name="t"></param>
    /// <returns></returns>
    Vector3 Spline(const std::vector<Vector3>& points, float t);
}
