#pragma once
#include "IScene.h"
#include "Util.h"
#include <vector>
#include "ModelObj.h"
#include "DebugCamera.h"
#include "LightGroup.h"
#include "BillboardImage.h"
#include "ColPrimitive3D.h"
#include "Float4.h"
#include "Easing.h"

static const int MAX_SPHERE_COUNT = 256;

struct SlimeBuffer
{
	float slimeValue;
	int32_t sphereNum;
	int32_t rayMatchNum = 16;
	float clipValue = 0.001f;

	Float4 spheres[MAX_SPHERE_COUNT];
};

//テスト用にランダムに動く球
class RandomSphere
{
public:
	ModelObj obj;
	Easing::EaseTimer timer = 3.0f;
	Vector3 moveVec = { 0,0,0 };

public:
	void Init();
	void Update();
	void Draw();
};

class RayMarchTestScene : public IScene
{
public:
	~RayMarchTestScene() {}

	void Init();
	void Update();
	void Draw();
	void Finalize();

	GraphicsPipeline SlimeShaderPipeLine();

public:

private:
	ModelObj plane;	//レイマーチングが見えるようになるオブジェクト
	//こういう形式じゃなくて、どこからでもレイマーチングオブジェクトが見えるようにしたい

	std::vector<RandomSphere> spheres;
	int32_t sphereNum = 64;

	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	LightGroup light;

	SRConstBuffer<SlimeBuffer> slimeBuff;

	bool isPlaneDraw = true;
	bool isSphereMeshDraw = true;
};

