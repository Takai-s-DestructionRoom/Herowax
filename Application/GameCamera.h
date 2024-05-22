#pragma once
#include "Camera.h"
#include "ModelObj.h"
#include "Easing.h"

class GameCamera
{
public:
	void Init();
	void Update();
	
	//カメラシェイク
	void Shake(float time,float power);

	void SetTarget(ModelObj* target_);

	static GameCamera* GetInstance();

public:
	float cameraDist = -20.f;			//注視点からカメラの距離
	float mmCameraDist = -250.f;		//注視点からカメラの距離
	Camera camera = Camera();			//通常カメラ

private:
	ModelObj* target;

	Vector2 cameraAngle{};				//カメラアングル
	Vector2 cameraSpeed{};				//カメラの移動速度
	Vector2 mAngle = { 0, 0 };

	Vector2 inverse = {1,-1};
	std::string invStr = "";

	float cameraUpOffset = 6.5f;

	bool changeFPS = false;

	bool changeCamera = false;

	Vector3 shake{};
	Easing::EaseTimer shakeTimer;
	float shakePower;
};

