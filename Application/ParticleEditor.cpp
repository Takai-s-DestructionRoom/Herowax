#include "ParticleEditor.h"
#include "Util.h"
#include "PathUtil.h"
#include <cassert>
#include "RImGui.h"

std::ofstream ParticleEditor::writing_file;
std::string ParticleEditor::saveFileName = "";
std::string ParticleEditor::loadFileName = "";
SimplePData ParticleEditor::savePData;
bool ParticleEditor::isAlwaysUpdate = false;

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
	ImGui::SetNextWindowSize({ 500, 400 }, ImGuiCond_FirstUseEver);

	ImGui::Begin("ParticleCreateGUI");
	
	ImGui::PushItemWidth(200);
	ImGui::Checkbox("常に更新する", &isAlwaysUpdate);
	ImGui::InputText("読み込むファイル名", &loadFileName);
	if (ImGui::Button("ロード")) {
		savePData = LoadSimple(loadFileName);
		saveFileName = loadFileName;
	}
	//エラーメッセージが入っているなら表示
	if (savePData.error != "") {
		ImGui::Text(savePData.error.c_str());
	}
	ImGui::Text("テクスチャは/Particle/の中に入れてください");
	ImGui::InputText("使用するテクスチャ", &savePData.tex);
	ImGui::ColorEdit4("パーティクルの色", &savePData.color.r);
	ImGui::Text("------------------------------------------");
	ImGui::InputInt("生成数", &savePData.addNum,1);
	ImGui::InputFloat("パーティクルの生存時間", &savePData.life);
	ImGui::Text("------------------------------------------");
	ImGui::InputFloat3("大きさ(倍率?)", &savePData.emitScale.x);
	ImGui::InputFloat("最小の大きさ(ランダムのmin)", &savePData.minScale);
	ImGui::InputFloat("最大の大きさ(ランダムのmax)", &savePData.endScale);
	ImGui::InputFloat3("最小の方向(ランダムのmin)", &savePData.minVelo.x);
	ImGui::InputFloat3("最大の方向(ランダムのmax)", &savePData.maxVelo.x);
	ImGui::InputFloat3("最小の回転(ランダムのmin)", &savePData.minRot.x);
	ImGui::InputFloat3("最大の回転(ランダムのmax)", &savePData.maxRot.x);
	ImGui::Text("------------------------------------------");
	ImGui::InputFloat("光る時間", &savePData.growingTimer, 1.0f);
	ImGui::InputFloat("加速度", &savePData.accelPower, 1.0f);
	ImGui::Text("------------------------------------------");
	ImGui::Checkbox("重力を適用するか", &savePData.isGravity);
	ImGui::Checkbox("Y軸ビルボード化するか", &savePData.isBillboard);
	ImGui::Text("------------------------------------------");
	ImGui::InputText("セーブするファイル名", &saveFileName);
	ImGui::PopItemWidth();

	if (ImGui::Button("セーブ") || isAlwaysUpdate) {
		SaveSimple(savePData, saveFileName);
	}
	ImGui::End();
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

		//boolども
		if (Util::ContainString(line, "isgravity")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.isGravity = false;
			if (Util::ContainString(strs[1],"true")) {
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
			
			TextureManager::Load("./Resources/Particle/" + strs[1] + ".png",strs[1]);

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
	
	std::string tempGrab = "false";
	if (saveData.isGravity)tempGrab = "true";
	writing_file << "isgravity:" << tempGrab << std::endl;
	
	std::string tempbill = "false";
	if (saveData.isBillboard)tempbill = "true";
	writing_file << "isbillboard:" << tempbill << std::endl;

	writing_file << "emitscale:" << SaveVector3(saveData.emitScale) << std::endl;
	writing_file << "minvelo:" <<  SaveVector3(saveData.minVelo) << std::endl;
	writing_file << "maxvelo:" <<  SaveVector3(saveData.maxVelo) << std::endl;
	writing_file << "maxvelo:" <<  SaveVector3(saveData.maxVelo) << std::endl;
	writing_file << "minrot:" << SaveVector3(saveData.minRot) << std::endl;
	writing_file << "maxrot:" << SaveVector3(saveData.maxRot) << std::endl;

	writing_file << "color:" << SaveColor(saveData.color) << std::endl;
	writing_file << "tex:" << saveData.tex << std::endl;

	writing_file.close();
}