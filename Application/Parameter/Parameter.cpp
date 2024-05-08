#include "Parameter.h"
#include "PathUtil.h"
#include "Util.h"
#include <cassert>

using namespace std;

std::ofstream Parameter::writing_file;

void Parameter::Begin(std::string filename)
{
	std::string outputName = "";
	outputName = "./Resources/Parameter/" + filename + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(outputName));

	writing_file.open(path, std::ios::out);
}

void Parameter::Save(std::string handle, std::string data)
{
	//ハンドル側に":"を入れるなほげが
	if (Util::ContainString(handle, ":"))
	{
		assert(0);
	}
	writing_file << handle << ":" << data << std::endl;
}

void Parameter::Save(const std::string& handle, float data)
{
	//ハンドル側に":"を入れるなほげが
	if (Util::ContainString(handle, ":"))
	{
		assert(0 && "ハンドル側に:を入れるなほげが");
	}
	writing_file << handle << ":" << data << std::endl;;
}

void Parameter::Save(const std::string& handle, int32_t data)
{	
	//ハンドル側に":"を入れるなほげが
	if (Util::ContainString(handle, ":"))
	{
		assert(0 && "ハンドル側に:を入れるなほげが");
	}
	writing_file << handle << ":" << data << std::endl;;
}

void Parameter::End()
{
	writing_file.close();
}

std::map<std::string, std::string> Parameter::Extract(const std::string& filename)
{
	std::string outputName = "";
	outputName = "./Resources/Parameter/" + filename + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(outputName));

	std::ifstream file;
	file.open(path.c_str());
	
	//ファイルが見つからないなら生成する
	if (file.fail()) {
		Begin(filename);
		Save("__TEST__GGEZ__", 1.f);
		End();
	}

	std::map<std::string, std::string> result;

	string line = "";
	while (getline(file, line)) {
		istringstream line_stream(line);

		string key;
		std::vector<std::string> strs = Util::StringSplit(line, ":");
		result[strs[0]] = strs[1];
	}

	return result;
}

float Parameter::GetParam(std::map<std::string, std::string>& extractData, 
	const std::string& handle,float defaultData)
{
	//ここでファイルの中に指定した名前のハンドルがあるかをチェックする
	//ない場合デフォルトデータを返す
	if (extractData.find(handle) == std::end(extractData))
	{
		return defaultData;
	}

	//ある場合その要素を返す
	return std::stof(extractData[handle]);
}

Vector3 Parameter::GetVector3Data(std::map<std::string, std::string>& extractData, 
	const std::string& handle, Vector3 defaultData)
{
	//ここでファイルの中に指定した名前のハンドルがあるかをチェックする
	//ない場合デフォルトデータを返す
	if (extractData.find(handle) == std::end(extractData))
	{
		return defaultData;
	}

	std::vector<std::string> temp = Util::StringSplit(extractData[handle], "_");

	Vector3 returnData = {
		std::stof(temp[0]),
		std::stof(temp[1]),
		std::stof(temp[2])
	};

	//ある場合その要素を返す
	return returnData;
}

Color Parameter::GetColorData(std::map<std::string, std::string>& extractData, const std::string& handle, Color defaultData)
{
	//ここでファイルの中に指定した名前のハンドルがあるかをチェックする
	//ない場合デフォルトデータを返す
	if (extractData.find(handle) == std::end(extractData))
	{
		return defaultData;
	}

	std::vector<std::string> temp = Util::StringSplit(extractData[handle], "_");

	Color returnData = {
		std::stof(temp[0]),
		std::stof(temp[1]),
		std::stof(temp[2]),
		std::stof(temp[3])
	};

	//ある場合その要素を返す
	return returnData;
}

void Parameter::SaveVector3(const std::string& handle, Vector3 data)
{
	//ハンドル側に":"を入れるなほげが
	if (Util::ContainString(handle, ":"))
	{
		assert(0 && "ハンドル側に:を入れるなほげが");
	}
	writing_file << handle << ":";
	writing_file << data.x << "_";
	writing_file << data.y << "_";
	writing_file << data.z << std::endl;
}

void Parameter::SaveColor(const std::string& handle, Color data)
{
	//ハンドル側に":"を入れるなほげが
	if (Util::ContainString(handle, ":"))
	{
		assert(0 && "ハンドル側に:を入れるなほげが");
	}
	writing_file << handle << ":";
	writing_file << data.r << "_";
	writing_file << data.g << "_";
	writing_file << data.b << "_";
	writing_file << data.a << std::endl;
}
