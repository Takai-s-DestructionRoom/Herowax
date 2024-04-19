#include "EnemyBehaviorEditor.h"
#include "Util.h"
#include "PathUtil.h"
#include <cassert>
#include "RImGui.h"

using namespace std::filesystem;

std::ofstream EnemyBehaviorEditor::writing_file;
std::string EnemyBehaviorEditor::saveFileName = "";
std::string EnemyBehaviorEditor::loadFileName = "";
BehaviorData EnemyBehaviorEditor::saveBehaviorData;

Vector3 EnemyBehaviorEditor::GetVector3Data(const std::string& str)
{
	Vector3 result;

	std::vector<std::string> strs2 = Util::StringSplit(str, "_");
	result.x = std::stof(strs2[0]);
	result.y = std::stof(strs2[1]);
	result.z = std::stof(strs2[2]);

	return result;
}

std::string EnemyBehaviorEditor::SaveVector3(const Vector3& data)
{
	return std::to_string(data.x) + "_" + std::to_string(data.y) + "_" + std::to_string(data.z);
}

BehaviorData EnemyBehaviorEditor::Load(const std::string& filename)
{
	std::string outputName = "";
	outputName = "./Resources/Data/BehaviorOrder/" + filename + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(
		Util::ConvertStringToWString(outputName));

	BehaviorData result;

	//ファイルストリーム
	std::ifstream file;
	//ファイルを開く
	file.open(path.c_str());
	//失敗したら
	if (file.fail()) {
		result.error = "エラー";
		return result;
	}

	std::string line = "";
	while (getline(file, line)) {
		std::istringstream line_stream(line);
		
		if (Util::ContainString(line, "start")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.start = GetVector3Data(strs[1]);
		}
		if (Util::ContainString(line, "inter")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.inters.push_back(GetVector3Data(strs[1]));
		}
		if (Util::ContainString(line, "end")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.end = GetVector3Data(strs[1]);
		}
	}

	return result;
}

void EnemyBehaviorEditor::Save(const BehaviorData& data, const std::string& saveFileName_)
{
	std::string outputName = "";
	outputName = "./Resources/Data/BehaviorOrder/" + saveFileName_ + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(outputName));

	writing_file.open(path, std::ios::out);

	writing_file << "start:" << SaveVector3(data.start) << std::endl;

	int32_t count = 0;
	for (auto& inter : data.inters)
	{
		count++;
		writing_file << "inter" << std::to_string(count) << ":" << SaveVector3(inter) << std::endl;
	}

	writing_file << "end:" << SaveVector3(data.end) << std::endl;
	writing_file.close();
}

Vector3 GetLerp(Vector3 start, Vector3 end, float timeRate)
{
	return Vector3(Easing::lerp(start.x,end.x, timeRate),
			 Easing::lerp(start.y,end.y, timeRate),
			 Easing::lerp(start.z,end.z, timeRate));
}

Vector3 BehaviorData::GetMoveVec()
{
	oldRate = timer.GetTimeRate();
	timer.Update();

	if (!timer.GetRun()) {
		timer.Start();
		progress++;
		if (progress > inters.size() + 2) {
			progress = 0;
		}
	}

	//最初なら
	if (progress == 0){
		Vector3 now = GetLerp(start, inters[0], timer.GetTimeRate());
		Vector3 old = GetLerp(start, inters[0], oldRate);
		
		return Vector3(now - old);
	}
	//最後なら
	else if (progress == inters.size() + 2) {
		Vector3 now = GetLerp(inters.back(), end, timer.GetTimeRate());
		Vector3 old = GetLerp(inters.back(), end, oldRate);

		return Vector3(now - old);
	}
	//途中なら
	else {
		Vector3 now = GetLerp(inters[progress - 1], inters[progress], timer.GetTimeRate());
		Vector3 old = GetLerp(inters[progress - 1], inters[progress], oldRate);

		return Vector3(now - old);
	}
}
