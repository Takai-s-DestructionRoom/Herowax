#include "WaxVisual.h"
#include "WaxManager.h"
#include "Ground.h"
#include "TimeManager.h"

void WaxVisual::Init()
{
	//最初のずらし
	//ここの出現位置は外側で指定して、塗られたら染み出す感じにしたい
	//雫が落ちる感じ
	
	//メタボールの挙動的に、集まったら勝手に大きくなるから、
	//大きさは固定で、自分と同じ位置にある程度たくさん集まったら(当たり判定)
	//何個か落ちる(先に判定を行ったやつが落下する、コライダーが移動することで次のものと当たらないように)

	initInter.x = Util::GetRand(-5.0f,5.0f);
	initInter.y = -5.f;
	initInter.z = Util::GetRand(-5.0f, 5.0f);

	obj.mTransform.position = initInter;
	collider.r = 0.11f;

	ground = Ground::GetInstance()->GetTransform();
}

void WaxVisual::Update()
{
	moveVec = { 0,0,0 };

	if (power >= THRESHOLD_POWER) {
		gravity += gravityAccel;
		moveVec.y -= gravity * power * TimeManager::deltaTime;
	}

	moveVec.y *= 0.99f;

	//ローカルで座標を下げる
	obj.mTransform.position += moveVec;
	
	//ワールド座標をコライダーに保存
	if (obj.GetParent()) {
		collider.pos = obj.GetParent()->mTransform.position + obj.mTransform.position;
		
		//当たってるからなんか色々する
	}
	else{
		collider.pos += moveVec;
	}

	if (ground.position.y > collider.pos.y)
	{
		collider.pos.y = ground.position.y;
		gravity = 0.0f;

		isAlive = false;
	}
}

void WaxVisual::Draw()
{

}

void WaxVisual::TransferBuffer()
{
	if (obj.GetParent()) {

	}
	WaxManager::GetInstance()->slimeWax.spheres.emplace_back();
	WaxManager::GetInstance()->slimeWax.spheres.back().collider.pos = collider.pos;
	WaxManager::GetInstance()->slimeWax.spheres.back().collider.r = collider.r;
}

void WaxVisual::SetGround(Transform ground_)
{
	ground = ground_;
}
