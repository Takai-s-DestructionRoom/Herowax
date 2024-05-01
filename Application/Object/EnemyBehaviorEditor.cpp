#include "EnemyBehaviorEditor.h"
#include "Util.h"
#include "PathUtil.h"
#include <cassert>
#include "RImGui.h"
#include "ColPrimitive3D.h"

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

std::vector<std::string> EnemyBehaviorEditor::LoadFileNames()
{
	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString("./Resources/Data/BehaviorOrder/"));
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

Vector3 BehaviorData::GetBehavior(Vector3 basis, Vector3 nowPos)
{
	//1つしか入ってないならスキップ
	if (points.size() < 2)return Vector3(0,0,0);
	
	Vector3 result = { 0,0,0 };
	
	//終点まで到達したら最初の位置に戻る
	if (progress >= points.size() - 1) {
		result = (basis + points[0]) - (basis + points[progress]);
		
		ColPrimitive3D::Sphere sphere1;
		sphere1.pos = basis + points[0];
		sphere1.pos.y = 0;
		sphere1.r = 3;
		ColPrimitive3D::Sphere sphere2;
		sphere2.pos = nowPos;
		sphere2.pos.y = 0;
		sphere2.r = 3;
		//終点まで到達したら
		if (ColPrimitive3D::CheckSphereToSphere(sphere1, sphere2)) {
			//次に進める
			hitCheck = true;
		}
	}
	else
	{
		result = (basis + points[progress + 1]) - (basis + points[progress]);
	
		ColPrimitive3D::Sphere sphere1;
		sphere1.pos = basis + points[progress + 1];
		sphere1.pos.y = 0;
		sphere1.r = 3;
		ColPrimitive3D::Sphere sphere2;
		sphere2.pos = nowPos;
		sphere2.pos.y = 0;
		sphere2.r = 3;
		//終点まで到達したら
		if (ColPrimitive3D::CheckSphereToSphere(sphere1, sphere2)) {
			//次に進める
			hitCheck = true;
		}
	}

	if (hitCheck) {
		progress++;
		if (progress >= points.size()) {
			progress = 0;
		}

		hitCheck = false;
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
	progress = 0;
}
