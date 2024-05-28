#include "WaxWall.h"
#include "WaxManager.h"
#include "RImGui.h"

void WaxWall::Init()
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere"));
	obj.mTransform.scale = { 2,2,2 };
	baseScale = { 10,10,10 };

	colliderSize = 10;

	isAlive = false;

	obj.mTuneMaterial.mColor = Color::kWaxColor;
	obj.mTuneMaterial.mColor.a = { 1.0f };
}

void WaxWall::Update()
{
	//パリィ中の場合はパリィが終わった後に終了を呼ぶ
	if (endFlag && parryTimer.GetEnd()) {
		End();
		endFlag = false;
	}

	parryTimer.Update();
	
	if (parryTimer.GetNowEnd()) {
		leakOutTimer.Start();
	}

	leakOutTimer.Update();

	if (!isAlive)return;

	//入りの挙動(外側から濃くなって、プレイヤーの周りに発生する)
	obj.mTransform.scale = baseScale;
	if (parryTimer.GetStarted()) {
		float barriarScale = Easing::OutQuad(1.0f, 0.0f, parryTimer.GetTimeRate());
		obj.mTransform.scale += { barriarScale, barriarScale, barriarScale };
	}
	obj.mTuneMaterial.mColor.a = Easing::OutQuad(0.5f, 0.6f,parryTimer.GetTimeRate());

	UpdateCollider();

	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);


	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 600, 250 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("WaxWall");
		ImGui::ColorEdit4("色", &obj.mTuneMaterial.mColor.r);
		ImGui::End();
	}
}

void WaxWall::Draw()
{
	if (!isAlive)return;

	BrightDraw();

	isDrawCollider = true;
	DrawCollider();
}

bool WaxWall::GetParry()
{
	return parryTimer.GetRun();
}

bool WaxWall::GetLeakOutMode()
{
	return parryTimer.GetEnd();
}

bool WaxWall::ContinueCheck(int32_t remainWax)
{
	//漏れるタイマーが終了して
	if (leakOutTimer.GetEnd()) {
		//ロウの消費ができるなら継続
		if(remainWax >= CONTINUE_CHECK_WAXNUM){
			leakOutTimer.Start();
			return true;
		}
		//出来ないなら終了
		else {
			End();
		}
	}
	return false;
}

bool WaxWall::StartCheck(int32_t remainWax)
{
	return remainWax >= START_CHECK_WAXNUM;
}

void WaxWall::Start()
{
	parryTimer.Start();
	isAlive = true;
	endFlag = false;
}

void WaxWall::End()
{
	if (parryTimer.GetEnd()) {
		isAlive = false;
		leakOutTimer.Reset();
		parryTimer.Reset();
	}
	else {
		endFlag = true;
	}
}
