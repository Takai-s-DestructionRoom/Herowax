#pragma once
#include <string>
#include <map>
#include <fstream>

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
	/// 保存したいデータを入れる。基本to_stringで変換して入れるのを想定
	/// :区切りでデータを保存しているので、handleに":"を使うとバグります
	/// floatだけよく入れるので用意
	/// <param name="handle">ファイルの中で手前に付く名前。ImGuiのラベルと同じ感じで使ってください。</param>
	/// <param name="data">保存したいデータ<</param>
	static void Save(std::string handle, float data);

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
	static std::map<std::string, std::string> Extract(std::string filename);

private:
	static std::ofstream writing_file;
};

