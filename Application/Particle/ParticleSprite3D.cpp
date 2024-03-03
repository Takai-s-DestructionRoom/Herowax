#include "ParticleSprite3D.h"
#include "TimeManager.h"
#include "Util.h"
#include <Camera.h>
#include <Renderer.h>
#include <SimpleDrawer.h>

using namespace Util;

ParticleSprite3D::ParticleSprite3D(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, float angle, float rotSpeed, Vector3 inVelocity, Vector3 stayVelocity, Vector3 outVelocity, Vector2 inSize, Vector2 staySize, Vector2 outSize, float inTime, float stayTime, float outTime, bool backDraw)
	: inVelocity(inVelocity), stayVelocity(stayVelocity), outVelocity(outVelocity),
	  inSize(inSize), staySize(staySize), outSize(outSize),
	  inTime(inTime), stayTime(stayTime), outTime(outTime),
	  backTrans(backDraw)
{
	this->image.SetTexture(texHandle);
	this->image.SetTexRect(
		static_cast<float>(texRect.left),
		static_cast<float>(texRect.top),
		static_cast<float>(texRect.right - texRect.left),
		static_cast<float>(texRect.bottom - texRect.top));
	this->image.SetAnchor(texAnchor);
	this->image.mBlendMode = Image3D::BlendMode::OverwriteAdd;
	this->pos = pos;
	this->color = color;
	this->radian = Util::AngleToRadian(angle);
	this->rotSpeed = Util::AngleToRadian(rotSpeed);
	this->maxTime = inTime + stayTime + outTime;
}

void ParticleSprite3D::Update()
{
	timer += TimeManager::deltaTime;
	if (timer >= maxTime) {
		isDeleted = true;
		return;
	}

	float inBorder = inTime;
	float outBorder = maxTime - outTime;

	image.mMaterial.mColor = color;

	if (timer < inBorder) {
		float t = timer / inTime;
		float fx = 1 - powf(1 - t, 3);

		velocity = inVelocity * (1 - fx) + stayVelocity * fx;
		size = inSize * (1 - fx) + staySize * fx;
	}
	else if (timer >= outBorder) {
		float t = (timer - inTime - stayTime) / outTime;
		float fx = 1 - powf(1 - t, 3);

		velocity = stayVelocity * (1 - fx) + outVelocity * fx;
		size = staySize * (1 - fx) + outSize * fx;
		image.mMaterial.mColor.a = color.a * (1 - fx);
	}
	else {
		velocity = stayVelocity;
		size = staySize;
	}

	pos += velocity * TimeManager::deltaTime;
	radian += rotSpeed * TimeManager::deltaTime;
}

void ParticleSprite3D::Draw()
{
	image.mTransform.position = pos;
	image.mTransform.rotation.z = radian;
	image.mTransform.scale = { size.x, size.y, 1 };
	image.mTransform.UpdateMatrix();
	
	image.TransferBuffer(Camera::sNowCamera->mViewProjection);
	if(backTrans) image.Draw("BackTransparent");
	else image.Draw("Transparent");
}

void ParticleSprite3D::Spawn(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, float angle, float rotSpeed, Vector3 inVelocity, Vector3 stayVelocity, Vector3 outVelocity, Vector2 inSize, Vector2 staySize, Vector2 outSize, float inTime, float stayTime, float outTime, bool backDraw) {
	manageParticleObjList.emplace_back(std::make_unique<ParticleSprite3D>(pos, texHandle, texRect, texAnchor, color, angle, rotSpeed, inVelocity, stayVelocity, outVelocity, inSize, staySize, outSize, inTime, stayTime, outTime, backDraw));
}
