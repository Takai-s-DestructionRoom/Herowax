#include "ParticleEditor.h"
#include "Util.h"
#include "PathUtil.h"
#include <cassert>
#include "RImGui.h"

using namespace std::filesystem;

std::ofstream ParticleEditor::writing_file;
std::string ParticleEditor::saveFileName = "";
std::string ParticleEditor::loadFileName = "";
SimplePData ParticleEditor::saveSimplePData;
RingPData ParticleEditor::saveRingPData;
HomingPData ParticleEditor::saveHomingPData;
DirectionalPData ParticleEditor::saveDirectionalPData;
bool ParticleEditor::isAlwaysUpdate = false;
std::vector<std::string> ParticleEditor::file_names;

Vector3 GetVector3Data(const std::string& str)
{
	Vector3 result;

	std::vector<std::string> strs2 = Util::StringSplit(str, "_");
	result.x = std::stof(strs2[0]);
	result.y = std::stof(strs2[1]);
	result.z = std::stof(strs2[2]);

	return result;
}

Color GetColorData(const std::string& str)
{
	Color result;

	std::vector<std::string> strs2 = Util::StringSplit(str, "_");
	result.r = std::stof(strs2[0]);
	result.g = std::stof(strs2[1]);
	result.b = std::stof(strs2[2]);
	result.a = std::stof(strs2[3]);

	return result;
}

std::string SaveVector3(const Vector3& data)
{
	return std::to_string(data.x) + "_" + std::to_string(data.y) + "_" + std::to_string(data.z);
}

std::string SaveColor(const Color& data)
{
	return std::to_string(data.r) + "_" + std::to_string(data.g) + "_" + std::to_string(data.b) + "_" + std::to_string(data.a);
}

void ParticleEditor::OrderCreateGUI()
{
	if (RImGui::showImGui)
	{

		ImGui::SetNextWindowSize({ 500, 400 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("ParticleCreateGUI");

		ImGui::PushItemWidth(200);
		ImGui::Checkbox("常に更新する", &isAlwaysUpdate);

		if (!file_names.empty())
		{
			//ハンドルの一覧をプルダウンで表示
			std::vector<const char*> temp;
			for (size_t i = 0; i < file_names.size(); i++)
			{
				temp.push_back(file_names[i].c_str());
			}
			static int32_t select = 0;
			ImGui::Combo("読み込むファイル名", &select, &temp[0], (int32_t)file_names.size());
			loadFileName = file_names[select];
		}

		if (ImGui::Button("ロード")) {
			if (Util::ContainString(loadFileName, "_ring"))
			{
				saveRingPData = LoadRing(loadFileName);
			}
			else if (Util::ContainString(loadFileName, "_homing"))
			{
				saveHomingPData = LoadHoming(loadFileName);
			}
			//ファイル名に以下略
			else if (Util::ContainString(loadFileName, "_directional")) 
			{
				saveDirectionalPData = LoadDirectional(loadFileName);
			}
			else
			{
				saveSimplePData = LoadSimple(loadFileName);
			}
			saveFileName = loadFileName;
		}
		//エラーメッセージが入っているなら表示
		if (saveSimplePData.error != "") {
			ImGui::Text(saveSimplePData.error.c_str());
		}

		if (ImGui::TreeNode("シンプルパーティクル"))
		{
			ImGui::Text("テクスチャは/Particle/の中に入れてください");
			ImGui::InputText("使用するテクスチャ", &saveSimplePData.tex);
			ImGui::ColorEdit4("パーティクルの色", &saveSimplePData.color.r);
			ImGui::Text("------------------------------------------");
			ImGui::InputInt("生成数", &saveSimplePData.addNum, 1);
			ImGui::InputFloat("パーティクルの生存時間", &saveSimplePData.life);
			ImGui::Text("------------------------------------------");
			ImGui::InputFloat3("エミッターサイズ", &saveSimplePData.emitScale.x);
			ImGui::InputFloat("生成しない範囲", &saveSimplePData.rejectRadius);
			ImGui::InputFloat("最小の大きさ(ランダムのmin)", &saveSimplePData.minScale);
			ImGui::InputFloat("最大の大きさ(ランダムのmax)", &saveSimplePData.maxScale);
			ImGui::InputFloat3("最小の方向(ランダムのmin)", &saveSimplePData.minVelo.x);
			ImGui::InputFloat3("最大の方向(ランダムのmax)", &saveSimplePData.maxVelo.x);
			ImGui::InputFloat3("最小の回転(ランダムのmin)", &saveSimplePData.minRot.x);
			ImGui::InputFloat3("最大の回転(ランダムのmax)", &saveSimplePData.maxRot.x);
			ImGui::InputFloat("最終スケール", &saveSimplePData.endScale);
			ImGui::Text("------------------------------------------");
			ImGui::InputFloat("大きくなるまでの時間", &saveSimplePData.growingTimer, 0.05f);
			ImGui::InputFloat("加速度", &saveSimplePData.accelPower, 0.05f);
			ImGui::Text("------------------------------------------");
			ImGui::Checkbox("重力を適用するか", &saveSimplePData.isGravity);
			ImGui::Checkbox("Y軸ビルボード化するか", &saveSimplePData.isBillboard);
			ImGui::Text("------------------------------------------");
			ImGui::InputText("セーブするファイル名", &saveFileName);

			if (ImGui::Button("セーブ") || isAlwaysUpdate) {
				SaveSimple(saveSimplePData, saveFileName);
			}

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("円形パーティクル"))
		{
			ImGui::Text("テクスチャは/Particle/の中に入れてください");
			ImGui::InputText("使用するテクスチャ", &saveRingPData.tex);
			ImGui::ColorEdit4("パーティクルの色", &saveRingPData.color.r);
			ImGui::Text("------------------------------------------");
			ImGui::InputInt("生成数", &saveRingPData.addNum, 1);
			ImGui::InputFloat("パーティクルの生存時間", &saveRingPData.life);
			ImGui::Text("------------------------------------------");
			ImGui::InputFloat("開始半径", &saveRingPData.startRadius);
			ImGui::InputFloat("終了半径", &saveRingPData.endRadius);
			ImGui::InputFloat("最小の大きさ(ランダムのmin)", &saveRingPData.minScale);
			ImGui::InputFloat("最大の大きさ(ランダムのmax)", &saveRingPData.maxScale);
			ImGui::InputFloat("最小のY方向(ランダムのmin)", &saveRingPData.minVeloY);
			ImGui::InputFloat("最大のY方向(ランダムのmax)", &saveRingPData.maxVeloY);
			ImGui::InputFloat3("最小の回転(ランダムのmin)", &saveRingPData.minRot.x);
			ImGui::InputFloat3("最大の回転(ランダムのmax)", &saveRingPData.maxRot.x);
			ImGui::InputFloat("最終スケール", &saveRingPData.endScale);
			ImGui::Text("------------------------------------------");
			ImGui::InputFloat("大きくなるまでの時間", &saveRingPData.growingTimer, 0.05f);
			ImGui::Text("------------------------------------------");
			ImGui::Checkbox("重力を適用するか", &saveRingPData.isGravity);
			ImGui::Checkbox("Y軸ビルボード化するか", &saveRingPData.isBillboard);
			ImGui::Text("------------------------------------------");
			ImGui::InputText("セーブするファイル名", &saveFileName);

			if (ImGui::Button("セーブ") || isAlwaysUpdate) {
				if (Util::ContainString(saveFileName, "_ring"))
				{
					SaveRing(saveRingPData, saveFileName);
				}
				else
				{
					SaveRing(saveRingPData, saveFileName + "_ring");
				}

				//セーブを押したらディレクトリ内を参照して文字列更新
				file_names = LoadFileNames();
			}

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("ホーミングパーティクル"))
		{
			ImGui::Text("テクスチャは/Particle/の中に入れてください");
			ImGui::InputText("使用するテクスチャ", &saveHomingPData.tex);
			ImGui::ColorEdit4("パーティクルの色", &saveHomingPData.color.r);
			ImGui::Text("------------------------------------------");
			ImGui::InputInt("生成数", &saveHomingPData.addNum, 1);
			ImGui::InputFloat("パーティクルの生存時間", &saveHomingPData.life);
			ImGui::Text("------------------------------------------");
			ImGui::InputFloat3("エミッターサイズ", &saveHomingPData.emitScale.x);
			ImGui::InputFloat("生成しない範囲", &saveHomingPData.rejectRadius);
			ImGui::InputFloat("最小の大きさ(ランダムのmin)", &saveHomingPData.minScale);
			ImGui::InputFloat("最大の大きさ(ランダムのmax)", &saveHomingPData.maxScale);
			ImGui::InputFloat3("最小の方向(ランダムのmin)", &saveHomingPData.minVelo.x);
			ImGui::InputFloat3("最大の方向(ランダムのmax)", &saveHomingPData.maxVelo.x);
			ImGui::InputFloat3("ホーミングする座標(参考用)", &saveHomingPData.targetPos.x);
			ImGui::InputFloat3("最小の回転(ランダムのmin)", &saveHomingPData.minRot.x);
			ImGui::InputFloat3("最大の回転(ランダムのmax)", &saveHomingPData.maxRot.x);
			ImGui::InputFloat("最終スケール", &saveHomingPData.endScale);
			ImGui::Text("------------------------------------------");
			ImGui::InputFloat("大きくなるまでの時間", &saveHomingPData.growingTimer, 0.05f);
			ImGui::InputFloat("加速度", &saveHomingPData.accelPower, 0.05f);
			ImGui::Text("------------------------------------------");
			ImGui::Checkbox("重力を適用するか", &saveHomingPData.isGravity);
			ImGui::Checkbox("Y軸ビルボード化するか", &saveHomingPData.isBillboard);
			ImGui::Checkbox("エミッターの中心に引き寄せられるか", &saveHomingPData.isTargetEmitter);
			ImGui::Text("------------------------------------------");
			ImGui::InputText("セーブするファイル名", &saveFileName);

			if (ImGui::Button("セーブ") || isAlwaysUpdate) {
				if (Util::ContainString(saveFileName, "_homing"))
				{
					SaveHoming(saveHomingPData, saveFileName);
				}
				else
				{
					SaveHoming(saveHomingPData, saveFileName + "_homing");
				}

				SaveHoming(saveHomingPData, saveFileName);
			}

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Directionalパーティクル"))
		{
			ImGui::Text("テクスチャは/Particle/の中に入れてください");
			ImGui::InputText("使用するテクスチャ", &saveDirectionalPData.tex);
			ImGui::ColorEdit4("パーティクルの色", &saveDirectionalPData.color.r);
			ImGui::Text("------------------------------------------");
			ImGui::InputInt("生成数", &saveDirectionalPData.addNum, 1);
			ImGui::InputFloat("パーティクルの生存時間", &saveDirectionalPData.life);
			ImGui::Text("------------------------------------------");
			ImGui::InputFloat3("エミッターサイズ", &saveDirectionalPData.emitScale.x);
			ImGui::InputFloat("生成しない範囲", &saveDirectionalPData.rejectRadius);
			ImGui::InputFloat("最小の大きさ(ランダムのmin)", &saveDirectionalPData.minScale);
			ImGui::InputFloat("最大の大きさ(ランダムのmax)", &saveDirectionalPData.maxScale);
			ImGui::InputFloat3("最小の方向(ランダムのmin)", &saveDirectionalPData.minVelo.x);
			ImGui::InputFloat3("最大の方向(ランダムのmax)", &saveDirectionalPData.maxVelo.x);
			ImGui::InputFloat("最終スケール", &saveDirectionalPData.endScale);
			ImGui::Text("------------------------------------------");
			ImGui::InputFloat("大きくなるまでの時間", &saveDirectionalPData.growingTimer, 0.05f);
			ImGui::InputFloat("加速度", &saveDirectionalPData.accelPower, 0.05f);
			ImGui::Text("------------------------------------------");
			ImGui::Checkbox("重力を適用するか", &saveDirectionalPData.isGravity);
			ImGui::Checkbox("Y軸ビルボード化するか", &saveDirectionalPData.isBillboard);
			ImGui::Text("------------------------------------------");
			ImGui::InputText("セーブするファイル名", &saveFileName);

			if (ImGui::Button("セーブ") || isAlwaysUpdate) {
				if (Util::ContainString(saveFileName, "_directional"))
				{
					SaveDirectional(saveDirectionalPData, saveFileName + "_directional");
				}
				else
				{
					SaveDirectional(saveDirectionalPData, saveFileName + "_directional");
				}

				SaveDirectional(saveDirectionalPData, saveFileName);
			}

			ImGui::TreePop();
		}
		ImGui::PopItemWidth();
		ImGui::End();
	}
}

void ParticleEditor::Init()
{
	file_names = LoadFileNames();
}

std::vector<std::string> ParticleEditor::LoadFileNames()
{
	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString("./Resources/Data/ParticleOrder/"));
	directory_iterator iter(path), end;
	std::error_code err;

	std::vector<std::string> temp_file_names;
	
	for (; iter != end && !err; iter.increment(err)) {
		const directory_entry entry = *iter;

		temp_file_names.push_back(entry.path().filename().string());
		std::vector<std::string> temp2 = Util::StringSplit(temp_file_names.back(), ".");
		temp_file_names.back() = temp2[0];
	}

	return temp_file_names;
}

SimplePData ParticleEditor::LoadSimple(const std::string& filename)
{
	std::string outputName = "";
	outputName = "./Resources/Data/ParticleOrder/" + filename + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(
		Util::ConvertStringToWString(outputName));

	SimplePData result;

	//ファイルストリーム
	std::ifstream file;
	//ファイルを開く
	file.open(path.c_str());
	if (file.fail()) {
		result.error = "error_ファイルの展開に失敗しました";
		return result;
	}

	std::string line = "";
	while (getline(file, line)) {
		std::istringstream line_stream(line);

		//intとかfloatとか
		if (Util::ContainString(line, "addNum")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.addNum = std::stoi(strs[1]);
		}
		if (Util::ContainString(line, "life")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.life = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "minscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.minScale = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "maxscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxScale = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "accelpower")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.accelPower = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "growingTimer")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.growingTimer = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "endscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.endScale = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "rejectradius")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.rejectRadius = std::stof(strs[1]);
		}

		//boolども
		if (Util::ContainString(line, "isgravity")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.isGravity = false;
			if (Util::ContainString(strs[1], "true")) {
				result.isGravity = true;
			}
		}
		if (Util::ContainString(line, "isbillboard")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.isBillboard = false;
			if (Util::ContainString(strs[1], "true")) {
				result.isBillboard = true;
			}
		}

		//Vector3ども
		if (Util::ContainString(line, "emitscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.emitScale = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "minvelo")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.minVelo = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "maxvelo")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxVelo = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "minrot")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.minRot = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "maxrot")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxRot = GetVector3Data(strs[1]);
		}

		//色とテクスチャ
		if (Util::ContainString(line, "color")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.color = GetColorData(strs[1]);
		}
		if (Util::ContainString(line, "tex")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");

			TextureManager::Load("./Resources/Particle/" + strs[1] + ".png", strs[1]);

			result.tex = strs[1].c_str();
		}
	}

	return result;
}

RingPData ParticleEditor::LoadRing(const std::string& filename)
{
	std::string outputName = "";
	outputName = "./Resources/Data/ParticleOrder/" + filename + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(
		Util::ConvertStringToWString(outputName));

	RingPData result;

	//ファイルストリーム
	std::ifstream file;
	//ファイルを開く
	file.open(path.c_str());
	if (file.fail()) {
		result.error = "error_ファイルの展開に失敗しました";
		return result;
	}

	std::string line = "";
	while (getline(file, line)) {
		std::istringstream line_stream(line);

		//intとかfloatとか
		if (Util::ContainString(line, "addNum")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.addNum = std::stoi(strs[1]);
		}
		if (Util::ContainString(line, "life")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.life = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "startradius")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.startRadius = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "endradius")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.endRadius = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "minscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.minScale = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "maxscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxScale = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "minveloY")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.minVeloY = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "maxveloY")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxVeloY = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "growingTimer")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.growingTimer = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "endscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.endScale = std::stof(strs[1]);
		}

		//boolども
		if (Util::ContainString(line, "isgravity")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.isGravity = false;
			if (Util::ContainString(strs[1], "true")) {
				result.isGravity = true;
			}
		}
		if (Util::ContainString(line, "isbillboard")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.isBillboard = false;
			if (Util::ContainString(strs[1], "true")) {
				result.isBillboard = true;
			}
		}

		//Vector3ども
		if (Util::ContainString(line, "minrot")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.minRot = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "maxrot")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxRot = GetVector3Data(strs[1]);
		}

		//色とテクスチャ
		if (Util::ContainString(line, "color")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.color = GetColorData(strs[1]);
		}
		if (Util::ContainString(line, "tex")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");

			TextureManager::Load("./Resources/Particle/" + strs[1] + ".png", strs[1]);

			result.tex = strs[1].c_str();
		}
	}

	return result;
}

HomingPData ParticleEditor::LoadHoming(const std::string& filename)
{
	std::string outputName = "";
	outputName = "./Resources/Data/ParticleOrder/" + filename + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(
		Util::ConvertStringToWString(outputName));

	HomingPData result;

	//ファイルストリーム
	std::ifstream file;
	//ファイルを開く
	file.open(path.c_str());
	if (file.fail()) {
		result.error = "error_ファイルの展開に失敗しました";
		return result;
	}

	std::string line = "";
	while (getline(file, line)) {
		std::istringstream line_stream(line);

		//intとかfloatとか
		if (Util::ContainString(line, "addNum")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.addNum = std::stoi(strs[1]);
		}
		if (Util::ContainString(line, "life")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.life = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "minscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.minScale = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "maxscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxScale = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "accelpower")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.accelPower = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "growingTimer")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.growingTimer = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "endscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.endScale = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "rejectradius")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.rejectRadius = std::stof(strs[1]);
		}

		//boolども
		if (Util::ContainString(line, "isgravity")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.isGravity = false;
			if (Util::ContainString(strs[1], "true")) {
				result.isGravity = true;
			}
		}
		if (Util::ContainString(line, "isbillboard")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.isBillboard = false;
			if (Util::ContainString(strs[1], "true")) {
				result.isBillboard = true;
			}
		}
		if (Util::ContainString(line, "istargetemitter")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.isTargetEmitter = false;
			if (Util::ContainString(strs[1], "true")) {
				result.isTargetEmitter = true;
			}
		}

		//Vector3ども
		if (Util::ContainString(line, "emitscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.emitScale = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "minvelo")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.minVelo = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "maxvelo")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxVelo = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "targetpos")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.targetPos = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "minrot")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.minRot = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "maxrot")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxRot = GetVector3Data(strs[1]);
		}

		//色とテクスチャ
		if (Util::ContainString(line, "color")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.color = GetColorData(strs[1]);
		}
		if (Util::ContainString(line, "tex")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");

			TextureManager::Load("./Resources/Particle/" + strs[1] + ".png", strs[1]);

			result.tex = strs[1].c_str();
		}
	}

	return result;
}

DirectionalPData ParticleEditor::LoadDirectional(const std::string& filename)
{
	std::string outputName = "";
	outputName = "./Resources/Data/ParticleOrder/" + filename + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(
		Util::ConvertStringToWString(outputName));

	DirectionalPData result;

	//ファイルストリーム
	std::ifstream file;
	//ファイルを開く
	file.open(path.c_str());
	if (file.fail()) {
		result.error = "error_ファイルの展開に失敗しました";
		return result;
	}

	std::string line = "";
	while (getline(file, line)) {
		std::istringstream line_stream(line);

		//intとかfloatとか
		if (Util::ContainString(line, "addNum")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.addNum = std::stoi(strs[1]);
		}
		if (Util::ContainString(line, "life")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.life = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "minscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.minScale = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "maxscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxScale = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "accelpower")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.accelPower = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "growingTimer")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.growingTimer = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "endscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.endScale = std::stof(strs[1]);
		}
		if (Util::ContainString(line, "rejectradius")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.rejectRadius = std::stof(strs[1]);
		}

		//boolども
		if (Util::ContainString(line, "isgravity")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.isGravity = false;
			if (Util::ContainString(strs[1], "true")) {
				result.isGravity = true;
			}
		}
		if (Util::ContainString(line, "isbillboard")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.isBillboard = false;
			if (Util::ContainString(strs[1], "true")) {
				result.isBillboard = true;
			}
		}

		//Vector3ども
		if (Util::ContainString(line, "emitscale")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.emitScale = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "minvelo")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.minVelo = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "maxvelo")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.maxVelo = GetVector3Data(strs[1]);
		}
		//色とテクスチャ
		if (Util::ContainString(line, "color")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.color = GetColorData(strs[1]);
		}
		if (Util::ContainString(line, "tex")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");

			TextureManager::Load("./Resources/Particle/" + strs[1] + ".png", strs[1]);

			result.tex = strs[1].c_str();
		}
	}

	return result;
}

void ParticleEditor::SaveSimple(const SimplePData& saveData, const std::string& saveFileName_)
{
	std::string outputName = "";
	outputName = "./Resources/Data/ParticleOrder/" + saveFileName_ + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(outputName));

	writing_file.open(path, std::ios::out);

	writing_file << "addNum:" << saveData.addNum << std::endl;
	writing_file << "life:" << saveData.life << std::endl;
	writing_file << "minscale:" << saveData.minScale << std::endl;
	writing_file << "maxscale:" << saveData.maxScale << std::endl;
	writing_file << "accelpower:" << saveData.accelPower << std::endl;
	writing_file << "growingTimer:" << saveData.growingTimer << std::endl;
	writing_file << "endscale:" << saveData.endScale << std::endl;
	writing_file << "rejectradius:" << saveData.rejectRadius << std::endl;

	std::string tempGrab = "false";
	if (saveData.isGravity)tempGrab = "true";
	writing_file << "isgravity:" << tempGrab << std::endl;

	std::string tempbill = "false";
	if (saveData.isBillboard)tempbill = "true";
	writing_file << "isbillboard:" << tempbill << std::endl;

	writing_file << "emitscale:" << SaveVector3(saveData.emitScale) << std::endl;
	writing_file << "minvelo:" << SaveVector3(saveData.minVelo) << std::endl;
	writing_file << "maxvelo:" << SaveVector3(saveData.maxVelo) << std::endl;
	writing_file << "minrot:" << SaveVector3(saveData.minRot) << std::endl;
	writing_file << "maxrot:" << SaveVector3(saveData.maxRot) << std::endl;

	writing_file << "color:" << SaveColor(saveData.color) << std::endl;
	writing_file << "tex:" << saveData.tex << std::endl;

	writing_file.close();
}

void ParticleEditor::SaveRing(const RingPData& saveData, const std::string& saveFileName_)
{
	std::string outputName = "";
	outputName = "./Resources/Data/ParticleOrder/" + saveFileName_ + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(outputName));

	writing_file.open(path, std::ios::out);

	writing_file << "addNum:" << saveData.addNum << std::endl;
	writing_file << "life:" << saveData.life << std::endl;
	writing_file << "startradius:" << saveData.startRadius << std::endl;
	writing_file << "endradius:" << saveData.endRadius << std::endl;
	writing_file << "minscale:" << saveData.minScale << std::endl;
	writing_file << "maxscale:" << saveData.maxScale << std::endl;
	writing_file << "minveloY:" << saveData.minVeloY << std::endl;
	writing_file << "maxveloY:" << saveData.maxVeloY << std::endl;
	writing_file << "growingTimer:" << saveData.growingTimer << std::endl;
	writing_file << "endscale:" << saveData.endScale << std::endl;

	std::string tempGrab = "false";
	if (saveData.isGravity)tempGrab = "true";
	writing_file << "isgravity:" << tempGrab << std::endl;

	std::string tempbill = "false";
	if (saveData.isBillboard)tempbill = "true";
	writing_file << "isbillboard:" << tempbill << std::endl;

	writing_file << "minrot:" << SaveVector3(saveData.minRot) << std::endl;
	writing_file << "maxrot:" << SaveVector3(saveData.maxRot) << std::endl;

	writing_file << "color:" << SaveColor(saveData.color) << std::endl;
	writing_file << "tex:" << saveData.tex << std::endl;

	writing_file.close();
}

void ParticleEditor::SaveHoming(const HomingPData& saveData, const std::string& saveFileName_)
{
	std::string outputName = "";
	outputName = "./Resources/Data/ParticleOrder/" + saveFileName_ + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(outputName));

	writing_file.open(path, std::ios::out);

	writing_file << "addNum:" << saveData.addNum << std::endl;
	writing_file << "life:" << saveData.life << std::endl;
	writing_file << "minscale:" << saveData.minScale << std::endl;
	writing_file << "maxscale:" << saveData.maxScale << std::endl;
	writing_file << "accelpower:" << saveData.accelPower << std::endl;
	writing_file << "growingTimer:" << saveData.growingTimer << std::endl;
	writing_file << "endscale:" << saveData.endScale << std::endl;
	writing_file << "rejectradius:" << saveData.rejectRadius << std::endl;

	std::string tempGrab = "false";
	if (saveData.isGravity)tempGrab = "true";
	writing_file << "isgravity:" << tempGrab << std::endl;

	std::string tempbill = "false";
	if (saveData.isBillboard)tempbill = "true";
	writing_file << "isbillboard:" << tempbill << std::endl;

	std::string tempTarget = "false";
	if (saveData.isTargetEmitter)tempTarget = "true";
	writing_file << "istargetemitter:" << tempTarget << std::endl;

	writing_file << "emitscale:" << SaveVector3(saveData.emitScale) << std::endl;
	writing_file << "minvelo:" << SaveVector3(saveData.minVelo) << std::endl;
	writing_file << "maxvelo:" << SaveVector3(saveData.maxVelo) << std::endl;
	writing_file << "targetpos:" << SaveVector3(saveData.targetPos) << std::endl;
	writing_file << "minrot:" << SaveVector3(saveData.minRot) << std::endl;
	writing_file << "maxrot:" << SaveVector3(saveData.maxRot) << std::endl;

	writing_file << "color:" << SaveColor(saveData.color) << std::endl;
	writing_file << "tex:" << saveData.tex << std::endl;

	writing_file.close();
}

void ParticleEditor::SaveDirectional(const DirectionalPData& saveData, const std::string& saveFileName_)
{
	std::string outputName = "";
	outputName = "./Resources/Data/ParticleOrder/" + saveFileName_ + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(outputName));

	writing_file.open(path, std::ios::out);

	writing_file << "addNum:" << saveData.addNum << std::endl;
	writing_file << "life:" << saveData.life << std::endl;
	writing_file << "minscale:" << saveData.minScale << std::endl;
	writing_file << "maxscale:" << saveData.maxScale << std::endl;
	writing_file << "accelpower:" << saveData.accelPower << std::endl;
	writing_file << "growingTimer:" << saveData.growingTimer << std::endl;
	writing_file << "endscale:" << saveData.endScale << std::endl;
	writing_file << "rejectradius:" << saveData.rejectRadius << std::endl;

	std::string tempGrab = "false";
	if (saveData.isGravity)tempGrab = "true";
	writing_file << "isgravity:" << tempGrab << std::endl;

	std::string tempbill = "false";
	if (saveData.isBillboard)tempbill = "true";
	writing_file << "isbillboard:" << tempbill << std::endl;

	writing_file << "emitscale:" << SaveVector3(saveData.emitScale) << std::endl;
	writing_file << "minvelo:" << SaveVector3(saveData.minVelo) << std::endl;
	writing_file << "maxvelo:" << SaveVector3(saveData.maxVelo) << std::endl;
	
	writing_file << "color:" << SaveColor(saveData.color) << std::endl;
	writing_file << "tex:" << saveData.tex << std::endl;

	writing_file.close();
}
