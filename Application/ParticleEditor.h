#pragma once
#include "Vector3.h"
#include "Color.h"
#include <stdint.h>
#include "Texture.h"
#include <fstream>
#include <string>

struct SimplePData
{
	//Vector3 emitPos; //posは使うときに指定したいので省略
	Vector3 emitScale = {};
	int32_t addNum = 0;
	float life = 0;
	Color color = {1,1,1,1};
	TextureHandle tex = "";
	float minScale = 0;
	float maxScale = 0;
	Vector3 minVelo = {};
	Vector3 maxVelo = {};
	float accelPower = 0.f;
	Vector3 minRot = {}; 
	Vector3 maxRot = {};
	float growingTimer = 0.f;
	float endScale = 0.f;
	bool isGravity = false;
	bool isBillboard = false;

	std::string error = "";
};

class ParticleEditor
{
public:
	//オーダーを作るImgui
	static void OrderCreateGUI();
	
	//読み込み
	static SimplePData LoadSimple(const std::string& filename);

	//書き出し
	static void SaveSimple(const SimplePData& saveData, const std::string& saveFileName_);

private:
	//読み込みで使うデータたち
	static std::ofstream writing_file;
	static std::string saveFileName;
	static std::string loadFileName;
	static SimplePData savePData;

	static bool isAlwaysUpdate;
};