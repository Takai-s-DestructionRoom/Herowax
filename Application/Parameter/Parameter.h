#pragma once
#include <string>
#include <map>
#include <fstream>
#include "Color.h"
#include "Vector3.h"

class Parameter
{
public:
	//よくわかんなかったらWaxManagerで使ってるのでそれ見てください。

	/// <summary>
	/// ImGui::Beginとほぼ同じ使用感
	/// Saveを呼び出す前に呼び出して、ファイルを生成したり呼び出したりする
	/// </summary>
	/// <param name="filename">保存するファイル名</param>
	static void Begin(std::string filename);

	/// <summary>
	/// BeginとEndの間で呼び出す
	/// 保存したいデータを入れる。基本to_stringで変換して入れるのを想定
	/// :区切りでデータを保存しているので、handleに":"を使うとバグります
	/// </summary>
	/// <param name="handle">ファイルの中で手前に付く名前。ImGuiのラベルと同じ感じで使ってください。</param>
	/// <param name="data">保存したいデータ</param>
	static void Save(std::string handle, std::string data);

	/// <summary>
	/// BeginとEndの間で呼び出す
	/// 保存したいデータを入れる。
	/// :区切りでデータを保存しているので、handleに":"を使うとバグります
	/// <param name="handle">ファイルの中で手前に付く名前。ImGuiのラベルと同じ感じで使ってください。</param>
	/// <param name="data">保存したいデータ<</param>
	static void Save(const std::string& handle, float data);

	/// <summary>
	/// BeginとEndの間で呼び出す
	/// 保存したいデータを入れる。
	/// :区切りでデータを保存しているので、handleに":"を使うとバグります
	/// <param name="handle">ファイルの中で手前に付く名前。ImGuiのラベルと同じ感じで使ってください。</param>
	/// <param name="data">保存したいデータ<</param>
	static void Save(const std::string& handle, int32_t data);
	
	/// <summary>
	/// ImGui::Endとほぼ同じ使用感
	/// 開いているファイルを閉じる
	/// </summary>
	static void End();

	/// <summary>
	/// 指定したファイルを開き、中の情報を返す
	/// Saveで指定したhandleと紐づいているので、それでいい感じに使って
	/// </summary>
	/// <param name="filename">開きたいファイル名</param>
	/// <returns></returns>
	static std::map<std::string, std::string> Extract(const std::string& filename);

	/// <summary>
	/// Extractで取得したデータから特定のハンドルのデータを取り出す関数
	/// もしデータが存在しない場合は引数のデータを入れる
	/// </summary>
	/// <param name="extractData">extractで取り出したデータ</param>
	/// <param name="handle">取り出したいデータのハンドル(saveの時に入れたhandleを入れる)</param>
	/// <param name="defaultData">データが存在しない場合に返す値</param>
	/// <returns></returns>
	static float GetParam(std::map<std::string, std::string>& extractData, const std::string& handle,float defaultData);

	static std::string GetParamStr(std::map<std::string, std::string>& extractData, const std::string& handle, std::string defaultData);
	
	static Vector3 GetVector3Data(std::map<std::string, std::string>& extractData, const std::string& handle, Vector3 defaultData);
	static Color GetColorData(std::map<std::string, std::string>& extractData, const std::string& handle, Color defaultData);

	static void SaveVector3(const std::string& handle, Vector3 data);
	static void SaveColor(const std::string& handle, Color data);

private:
	static std::ofstream writing_file;
};