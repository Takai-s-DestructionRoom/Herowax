#pragma once
#include "IScene.h"
#include "PhysicsCircle.h"
#include "Util.h"
#include <vector>

class RayMarchTestScene : public IScene
{
public:
	~RayMarchTestScene() {}

	void Init();
	void Update();
	void Draw();
	void Finalize();

public:
	std::vector<PhysicsCircle> circles;
	//PhysicsCircle circle = {Vector2(Util::WIN_WIDTH / 2,Util::WIN_HEIGHT / 2),3.f};
	Vector2 downPos;
	ColPrimitive3D::Ray ground;
};

