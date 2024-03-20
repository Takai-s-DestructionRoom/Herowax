#include "SpawnOrderData.h"
#include <fstream>
#include <cassert>
#include <PathUtil.h>
#include "Util.h"

SpawnOrderData SpawnOrderData::Load(const std::string& filename)
{
	std::string outputName = "";
	outputName = "./Data/SpawnOrder/" + filename + ".txt";
	
	std::filesystem::path path = PathUtil::ConvertAbsolute(
		Util::ConvertStringToWString(outputName));

	//ファイルストリーム
	std::ifstream file;
	//ファイルを開く
	file.open(path.c_str());
	if (file.fail()) {
		assert(0);
	}

	SpawnOrderData result;

	std::string line = "";
	while (getline(file, line)) {
		std::istringstream line_stream(line);

		//全体の時間が明記された行の時間を取る
		if (Util::ContainString(line, "全体の時間")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxTime = std::stof(strs[1]);
		}
		//それ以外なら敵の出現情報なので読み込む
		if(Util::ContainString(line, "敵の出現情報")){
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.orders.emplace_back();
			result.orders.back().enemyClassName = strs[1];
			result.orders.back().spawnTiming = std::stof(strs[2]);
			result.orders.back().spawnNum = (int32_t)std::stoi(strs[3]);
		}
	}

	return result;
}
