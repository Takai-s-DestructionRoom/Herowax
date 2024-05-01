#pragma once
#include <fstream>
#include "Vector3.h"
#include <string>
#include <vector>
#include "Easing.h"

struct BehaviorData
{
public:
	std::vector<Vector3> points;	//点を全て保存
	
	std::string error = "";

	//行動を取得(イージングで座標を返す)
	Vector3 GetBehavior(Vector3 basis,Vector3 nowPos);
	//進行方向を取得
	Vector3 GetMoveDir();

	//タイマーと移動順をリセット
	void Reset();

private:
	int32_t progress = 0;
	bool hitCheck = false;
};

class EnemyBehaviorEditor
{
public:
	static BehaviorData Load(const std::string& filename);
	static void Save(const BehaviorData& data, const std::string& saveFileName_);
	static std::vector<std::string> EnemyBehaviorEditor::LoadFileNames();

private:
	static Vector3 GetVector3Data(const std::string& str);

	static std::string SaveVector3(const Vector3& data);

private:
	static std::ofstream writing_file;
	static std::string saveFileName;
	static std::string loadFileName;
	static BehaviorData saveBehaviorData;
};

