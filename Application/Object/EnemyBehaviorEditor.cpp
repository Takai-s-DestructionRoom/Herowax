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
		if (Util::ContainString(line, "point")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.points.push_back(GetVector3Data(strs[1]));
		}
		/*if (Util::ContainString(line, "start")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.start = GetVector3Data(strs[1]);
		}
		
		if (Util::ContainString(line, "end")) {
			std::vector<std::string> strs = Util::StringSplit(line, ":");
			result.end = GetVector3Data(strs[1]);
		}*/
	}

	return result;
}

void EnemyBehaviorEditor::Save(const BehaviorData& data, const std::string& saveFileName_)
{
	std::string outputName = "";
	outputName = "./Resources/Data/BehaviorOrder/" + saveFileName_ + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(outputName));

	writing_file.open(path, std::ios::out);

	//writing_file << "start:" << SaveVector3(data.start) << std::endl;

	int32_t count = 0;
	for (auto& inter : data.points)
	{
		count++;
		writing_file << "point" << std::to_string(count) << ":" << SaveVector3(inter) << std::endl;
	}

	//writing_file << "end:" << SaveVector3(data.end) << std::endl;
	writing_file.close();
}

Vector3 GetLerp(Vector3 start, Vector3 end, float timeRate)
{
	return Vector3(Easing::lerp(start.x,end.x, timeRate),
			 Easing::lerp(start.y,end.y, timeRate),
			 Easing::lerp(start.z,end.z, timeRate));
}

Vector3 BehaviorData::GetBehavior(Vector3 basis)
{
	//1つしか入ってないならスキップ
	if (points.size() < 2)return Vector3(0,0,0);

	oldRate = timer.GetTimeRate();

	if (!timer.GetRun()) {
		timer.Start();
		progress++;
		if (progress >= points.size()) {
			progress = 0;
		}
	}

	timer.Update();

	Vector3 result = { 0,0,0 };
	
	//終点まで到達したら最初の位置に戻る
	if (progress >= points.size() - 1) {
		result = GetLerp(basis + points[progress], basis + points[0], timer.GetTimeRate());
	}
	else
	{
		result = GetLerp(basis + points[progress], basis + points[progress + 1], timer.GetTimeRate());
	}

	return result;
}

Vector3 BehaviorData::GetMoveDir()
{
	if (progress >= points.size() - 1) {
		return  points[0] - points[progress];
	}
	else
	{
		return points[progress + 1] - points[progress];
	}
}

void BehaviorData::Reset()
{
	progress = -1;
	timer.Reset();
	oldRate = 0.0f;
}
