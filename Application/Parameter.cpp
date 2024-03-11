#include "Parameter.h"
#include "PathUtil.h"
#include "Util.h"
#include <cassert>

using namespace std;

std::ofstream Parameter::writing_file;

void Parameter::Save(std::string handle, std::string data)
{
	//ハンドル側に":"を入れるなほげが
	if (Util::ContainString(handle,":") )
	{
		assert(0);
	}
	writing_file << handle << ":" << data << std::endl;
}

void Parameter::Begin(std::string filename)
{
	std::string outputName = "";
	outputName = "./Resources/Parameter/" + filename + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(outputName));

	writing_file.open(path, std::ios::out);
}

void Parameter::Save(std::string handle, float data)
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

std::map<std::string, std::string> Parameter::Extract(std::string filename)
{
	std::string outputName = "";
	outputName = "./Resources/Parameter/" + filename + ".txt";

	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(outputName));

	std::ifstream file;
	file.open(path.c_str());
	if (file.fail()) {
		assert(0);
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
