#include "SpawnOrderData.h"
#include <fstream>
#include <cassert>
#include <PathUtil.h>
#include "Util.h"
#include "RImGui.h"

std::ofstream SpawnDataLoader::writing_file;
std::string SpawnDataLoader::saveFileName;
SpawnOrderData  SpawnDataLoader::saveOrderData;
SpawnOrderOnce SpawnDataLoader::once;

std::string enemyHandles::enemy = "enemy";
std::string enemyHandles::bombsolider = "bombsolider";

SpawnOrderData SpawnDataLoader::Load(const std::string& filename)
{
	std::string outputName = "";
	outputName = "./Resources/Data/SpawnOrder/" + filename + ".txt";
	
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

		//全体の時間を取る
		if (Util::ContainString(line, "全体の時間")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxTime = std::stof(strs[1]);
		}
		//出現タイミングを取る
		if (Util::ContainString(line, "出現タイミング")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.startTiming = std::stof(strs[1]);
		}
		//敵の出現情報なので読み込む
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

void SpawnDataLoader::Save(const SpawnOrderData& saveOrder, const std::string& saveFileName_)
{
	std::string outputName = "";
	outputName = "./Resources/Data/SpawnOrder/" + saveFileName_ + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(outputName));

	writing_file.open(path, std::ios::out);

	writing_file << "全体の時間:" << saveOrder.maxTime << std::endl;
	writing_file << "出現タイミング:" << saveOrder.startTiming << std::endl;
	for (auto& order : saveOrder.orders)
	{
		writing_file << "敵の出現情報" << ":";
		writing_file << order.enemyClassName << ":";
		writing_file << order.spawnTiming << ":";
		writing_file << order.spawnNum << std::endl;
	}
	
	writing_file.close();
}

void SpawnDataLoader::OrderCreateGUI()
{
	//ImGui::SetNextWindowSize({ 400, 400 }, ImGuiCond_FirstUseEver);

	//ImGui::Begin("OrderCreateGUI");

	////------------ファイル読み込み-----------------//
	//static std::string loadfilename;
	//ImGui::PushItemWidth(200);
	//ImGui::InputText("編集するファイル名を入力", &loadfilename);
	//if (ImGui::Button("読み込み")) {
	//	saveOrderData = SpawnDataLoader::Load(loadfilename);
	//	saveFileName = loadfilename;
	//}
	//ImGui::Text("-----------------------");

	////-------------orderファイル生成部分----------------//
	//ImGui::InputText("セーブするファイル名", &saveFileName);
	//ImGui::PopItemWidth();

	////スポナー自体の生存時間設定
	//ImGui::PushItemWidth(100);
	//ImGui::InputFloat("スポナーの生存時間", &saveOrderData.maxTime,1.0f);
	//ImGui::InputFloat("スポナーの出現タイミング", &saveOrderData.startTiming, 1.0f);

	//if (ImGui::Button("セーブ")) {
	//	SpawnDataLoader::Save(saveOrderData, saveFileName);
	//}
	//
	//ImGui::Text("-----------------------");

	////敵のハンドル一覧をプルダウンで表示
	//std::vector<const char*> temp;
	//temp.push_back(enemyHandles::enemy.c_str());
	//temp.push_back(enemyHandles::bombsolider.c_str());
	//static int32_t select = 0;
	//ImGui::Combo("出現させる敵", &select, &temp[0], (int32_t)temp.size());
	//
	////切り替え用の名前に保存
	//once.enemyClassName = temp[select];
	//
	////出現させる敵の情報指定
	//ImGui::InputInt("スポーンさせる数", &once.spawnNum, 1);
	//ImGui::InputFloat("スポーンさせるタイミング",&once.spawnTiming, 1.0f);
	//
	////情報を入れる位置指定
	//static int32_t insertNum = 0;
	//ImGui::InputInt("挿入する位置", &insertNum,1);
	//insertNum = Util::Clamp(insertNum, 0, (int32_t)saveOrderData.orders.size());
	//ImGui::SameLine();
	//ImGui::PopItemWidth();
	//if (ImGui::Button("一番後ろに入れる")) {
	//	insertNum = (int32_t)saveOrderData.orders.size();
	//}

	////出現情報を登録
	//if (ImGui::Button("出現情報を登録")) {
	//	saveOrderData.orders.insert(saveOrderData.orders.begin() + insertNum,once);
	//	once.enemyClassName = "";
	//	once.spawnNum = 0;
	//	once.spawnTiming = 0.f;
	//	insertNum = (int32_t)saveOrderData.orders.size();

	//	SpawnDataLoader::Save(saveOrderData, saveFileName);
	//}

	////データ一覧表示
	//ImGui::Text("セーブ予定のデータ");

	//int32_t i = 0;
	//for (auto itr = saveOrderData.orders.begin(); itr != saveOrderData.orders.end();)
	//{
	//	i++;
	//	ImGui::Text("--------%d番--------",i);
	//	ImGui::Text("出現させる敵:%s", (*itr).enemyClassName.c_str());
	//	ImGui::Text("スポーンさせる数:%d", (*itr).spawnNum);
	//	ImGui::Text("スポーンさせるタイミング:%f", (*itr).spawnTiming);
	//	std::string deleteStr = "このデータを削除" + std::to_string(i);
	//	if (ImGui::Button(deleteStr.c_str()))
	//	{
	//		itr = saveOrderData.orders.erase(itr);
	//		SpawnDataLoader::Save(saveOrderData, saveFileName);
	//	}
	//	else {
	//		itr++;
	//	}
	//}

	//ImGui::Text("--------------------");

	//ImGui::End();
}
