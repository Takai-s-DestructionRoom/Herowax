#include "Shader.h"
#include "Util.h"
#include "RDirectX.h"
#include "PathUtil.h"

Shader::Shader(std::string filepath, std::string entrypoint, std::string target)
{
	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(filepath));
	HRESULT result = D3DCompileFromFile(
		path.c_str(), //ファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), //エントリ名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //デバッグ用
		0,
		&mShaderBlob, &mErrorBlob);
	mSucceeded = SUCCEEDED(result);
}

void Shader::Register(std::string id, Shader shader)
{
	ShaderRegister::GetInstance()->mShaderRegister[id] = shader;
}

Shader Shader::GetOrCreate(std::string id, std::string filepath, std::string entrypoint, std::string target)
{
	Shader res = GetRegistered(id);
	if (!res.mSucceeded) {
		res = Shader(filepath, entrypoint, target);
		Register(id, res);
	}
	return res;
}

Shader Shader::GetRegistered(std::string id)
{
	std::unordered_map<std::string, Shader>& map = ShaderRegister::GetInstance()->mShaderRegister;
	if (map.find(id) == map.end()) {
		return Shader();
	}
	return map[id];
}
