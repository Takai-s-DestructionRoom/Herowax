#pragma once
#include "ModelObj.h"
#include <vector>

class BreakableObj
{
public:
	BreakableObj();
	void Init();
	void Update();
	void Draw();

public:
	std::vector<ModelObj> objs;	//描画する分割されたほう
	std::list<Model> models;	//描画する分割されたほう

	Model root;	//モデル本体(描画はしない)
};

