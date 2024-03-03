/*
* @file PathUtil.h
* @brief ファイルパスを扱う機能群
*/

#pragma once
#include <filesystem>

namespace PathUtil
{
	void SetDebugPath(std::filesystem::path path);

	//メインパスを取得します
	//exeを叩いた場合は実行ファイルの格納ディレクトリパス
	//VSからデバッガ実行の場合はソリューションパス
	std::filesystem::path GetMainPath();

	//実行ファイルパス(.exe含む)を取得します
	std::filesystem::path GetModuleFilePath();

	//パスを絶対パスに変換します
	//既に絶対パスなら何もしません
	//指定したなら、その際baseパスを基準として相対位置を解決します
	//指定がなければメインパスを基準として相対位置を解決します
	std::filesystem::path ConvertAbsolute(std::filesystem::path path, std::filesystem::path base = "");

	//パスAを基準とした相対パスにパスBを変換します
	std::filesystem::path ConvertRelativeFromTo(std::filesystem::path a, std::filesystem::path b);

	//メインパスを基準とした相対パスにパスを変換します
	std::filesystem::path ConvertRelativeFromMainPath(std::filesystem::path path);

	//パスからファイル名を削除します
	//path::remove_filename()がカスなので追加処理だ
	std::filesystem::path RemoveFileName(std::filesystem::path path);
};

