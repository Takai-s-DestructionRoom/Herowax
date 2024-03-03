#include "LightGroup.h"

LightGroup* LightGroup::sNowLight = nullptr;

void LightGroup::SetDefault()
{
	mChangeFlag = true;

	mAmbientColor = { 1, 1, 1 };
	
	mDirectionalLights[0].mIsActive = true;
	mDirectionalLights[0].mLightVec = { 1, -1, 1 };
	mDirectionalLights[0].mColor = { 1, 1, 1, 1 };

	for (int32_t i = 1; i < DIRECTIONAL_LIGHT_NUM; i++) {
		mDirectionalLights[i].mIsActive = false;
	}

	for (int32_t i = 0; i < POINT_LIGHT_NUM; i++) {
		mPointLights[i].mIsActive = false;
	}

	for (int32_t i = 0; i < SPOT_LIGHT_NUM; i++) {
		mSpotLights[i].mIsActive = false;
	}
}

void LightGroup::Update()
{
	if (mChangeFlag) {
		mChangeFlag = false;

		mBuffer.mConstMap->ambientColor = mAmbientColor;

		for (int32_t i = 0; i < DIRECTIONAL_LIGHT_NUM; i++) {
			if (mDirectionalLights[i].mIsActive) {
				mDirectionalLights[i].TransferBuffer(mBuffer.mConstMap->directionalLights[i]);
			}
			else {
				mBuffer.mConstMap->directionalLights[i].active = false;
			}
		}

		for (int32_t i = 0; i < POINT_LIGHT_NUM; i++) {
			if (mPointLights[i].mIsActive) {
				mPointLights[i].TransferBuffer(mBuffer.mConstMap->pointLights[i]);
			}
			else {
				mBuffer.mConstMap->pointLights[i].active = false;
			}
		}

		for (int32_t i = 0; i < SPOT_LIGHT_NUM; i++) {
			if (mSpotLights[i].mIsActive) {
				mSpotLights[i].TransferBuffer(mBuffer.mConstMap->spotLights[i]);
			}
			else {
				mBuffer.mConstMap->spotLights[i].active = false;
			}
		}
	}
}

void LightGroup::SetAmbientColor(Vector3 color)
{
	mAmbientColor = color;
}

Vector3 LightGroup::GetAmbientColor()
{
	return mAmbientColor;
}

void LightGroup::SetDirectionalLightActive(int32_t index, bool active)
{
	assert(0 <= index && index < DIRECTIONAL_LIGHT_NUM);
	mDirectionalLights[index].mIsActive = active;
	mChangeFlag = true;
}

bool LightGroup::GetDirectionalLightActive(int32_t index)
{
	assert(0 <= index && index < DIRECTIONAL_LIGHT_NUM);
	return mDirectionalLights[index].mIsActive;
}

void LightGroup::SetDirectionalLightVec(int32_t index, Vector3 vec)
{
	assert(0 <= index && index < DIRECTIONAL_LIGHT_NUM);
	mDirectionalLights[index].mLightVec = vec;
	mChangeFlag = true;
}

Vector3 LightGroup::GetDirectionalLightVec(int32_t index)
{
	assert(0 <= index && index < DIRECTIONAL_LIGHT_NUM);
	return mDirectionalLights[index].mLightVec;
}

void LightGroup::SetDirectionalLightColor(int32_t index, Color color)
{
	assert(0 <= index && index < DIRECTIONAL_LIGHT_NUM);
	mDirectionalLights[index].mColor = color;
	mChangeFlag = true;
}

Color LightGroup::GetDirectionalLightColor(int32_t index)
{
	assert(0 <= index && index < DIRECTIONAL_LIGHT_NUM);
	return mDirectionalLights[index].mColor;
}

void LightGroup::SetPointLightActive(int32_t index, bool active)
{
	assert(0 <= index && index < POINT_LIGHT_NUM);
	mPointLights[index].mIsActive = active;
	mChangeFlag = true;
}

bool LightGroup::GetPointLightActive(int32_t index)
{
	assert(0 <= index && index < POINT_LIGHT_NUM);
	return mPointLights[index].mIsActive;
}

void LightGroup::SetPointLightPos(int32_t index, Vector3 pos)
{
	assert(0 <= index && index < POINT_LIGHT_NUM);
	mPointLights[index].mPos = pos;
	mChangeFlag = true;
}

Vector3 LightGroup::GetPointLightPos(int32_t index)
{
	assert(0 <= index && index < POINT_LIGHT_NUM);
	return mPointLights[index].mPos;
}

void LightGroup::SetPointLightColor(int32_t index, Color color)
{
	assert(0 <= index && index < POINT_LIGHT_NUM);
	mPointLights[index].mColor = color;
	mChangeFlag = true;
}

Color LightGroup::GetPointLightColor(int32_t index)
{
	assert(0 <= index && index < POINT_LIGHT_NUM);
	return mPointLights[index].mColor;
}

void LightGroup::SetPointLightAtten(int32_t index, Vector3 atten)
{
	assert(0 <= index && index < POINT_LIGHT_NUM);
	mPointLights[index].mAtten = atten;
	mChangeFlag = true;
}

Vector3 LightGroup::GetPointLightAtten(int32_t index)
{
	assert(0 <= index && index < POINT_LIGHT_NUM);
	return mPointLights[index].mAtten;
}

void LightGroup::SetSpotLightActive(int32_t index, bool active)
{
	assert(0 <= index && index < SPOT_LIGHT_NUM);
	mSpotLights[index].mIsActive = active;
	mChangeFlag = true;
}

bool LightGroup::GetSpotLightActive(int32_t index)
{
	assert(0 <= index && index < SPOT_LIGHT_NUM);
	return mSpotLights[index].mIsActive;
}

void LightGroup::SetSpotLightPos(int32_t index, Vector3 pos)
{
	assert(0 <= index && index < SPOT_LIGHT_NUM);
	mSpotLights[index].mPos = pos;
	mChangeFlag = true;
}

Vector3 LightGroup::GetSpotLightPos(int32_t index)
{
	assert(0 <= index && index < SPOT_LIGHT_NUM);
	return mSpotLights[index].mPos;
}

void LightGroup::SetSpotLightDirection(int32_t index, Vector3 dir)
{
	assert(0 <= index && index < SPOT_LIGHT_NUM);
	mSpotLights[index].mDirection = dir;
	mChangeFlag = true;
}

Vector3 LightGroup::GetSpotLightDirection(int32_t index)
{
	assert(0 <= index && index < SPOT_LIGHT_NUM);
	return mSpotLights[index].mDirection;
}

void LightGroup::SetSpotLightColor(int32_t index, Color color)
{
	assert(0 <= index && index < SPOT_LIGHT_NUM);
	mSpotLights[index].mColor = color;
	mChangeFlag = true;
}

Color LightGroup::GetSpotLightColor(int32_t index)
{
	assert(0 <= index && index < SPOT_LIGHT_NUM);
	return mSpotLights[index].mColor;
}

void LightGroup::SetSpotLightAtten(int32_t index, Vector3 atten)
{
	assert(0 <= index && index < SPOT_LIGHT_NUM);
	mSpotLights[index].mAtten = atten;
	mChangeFlag = true;
}

Vector3 LightGroup::GetSpotLightAtten(int32_t index)
{
	assert(0 <= index && index < SPOT_LIGHT_NUM);
	return mSpotLights[index].mAtten;
}

void LightGroup::SetSpotLightFactorAngle(int32_t index, Vector2 factorAngle)
{
	assert(0 <= index && index < SPOT_LIGHT_NUM);
	mSpotLights[index].mFactorAngle = factorAngle;
	mChangeFlag = true;
}

Vector2 LightGroup::GetSpotLightFactorAngle(int32_t index)
{
	assert(0 <= index && index < SPOT_LIGHT_NUM);
	return mSpotLights[index].mFactorAngle;
}
