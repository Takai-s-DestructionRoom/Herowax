/*
* @file LightGroup.h
* @brief 3D空間の光源を管理するクラス
*/

#pragma once
#include "Color.h"
#include "RConstBuffer.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class LightGroup
{
public:
	static LightGroup* sNowLight;
	static constexpr int32_t DIRECTIONAL_LIGHT_NUM = 8;
	static constexpr int32_t POINT_LIGHT_NUM = 8;
	static constexpr int32_t SPOT_LIGHT_NUM = 8;

	struct LightGroupBuffer {
		Vector3 ambientColor;
		float pad;
		DirectionalLight::LightBuffer directionalLights[DIRECTIONAL_LIGHT_NUM];
		PointLight::LightBuffer pointLights[POINT_LIGHT_NUM];
		SpotLight::LightBuffer spotLights[SPOT_LIGHT_NUM];
	};

	RConstBuffer<LightGroupBuffer> mBuffer;

	LightGroup() {
		SetDefault();
	}

	//デフォルト値に設定
	void SetDefault();

	//更新処理
	void Update();

	//環境光の色を設定する
	void SetAmbientColor(Vector3 color);

	//現在の環境光の色を得る
	Vector3 GetAmbientColor();

	//平行光源のアクティブ状態を設定する
	void SetDirectionalLightActive(int32_t index, bool active);

	//平行光源のアクティブ状態を得る
	bool GetDirectionalLightActive(int32_t index);

	//平行光源の方向を設定する
	void SetDirectionalLightVec(int32_t index, Vector3 vec);

	//平行光源の方向を得る
	Vector3 GetDirectionalLightVec(int32_t index);

	//平行光源の色を設定する
	void SetDirectionalLightColor(int32_t index, Color color);

	//平行光源の色を得る
	Color GetDirectionalLightColor(int32_t index);

	//点光源のアクティブ状態を設定する
	void SetPointLightActive(int32_t index, bool active);

	//点光源のアクティブ状態を得る
	bool GetPointLightActive(int32_t index);

	//点光源の座標を設定する
	void SetPointLightPos(int32_t index, Vector3 pos);

	//点光源の座標を得る
	Vector3 GetPointLightPos(int32_t index);

	//点光源の色を設定する
	void SetPointLightColor(int32_t index, Color color);

	//点光源の色を得る
	Color GetPointLightColor(int32_t index);

	//点光源の減衰係数を設定する
	void SetPointLightAtten(int32_t index, Vector3 atten);

	//点光源の減衰係数を得る
	Vector3 GetPointLightAtten(int32_t index);

	//スポットライトのアクティブ状態を設定する
	void SetSpotLightActive(int32_t index, bool active);

	//スポットライトのアクティブ状態を得る
	bool GetSpotLightActive(int32_t index);

	//スポットライトの座標を設定する
	void SetSpotLightPos(int32_t index, Vector3 pos);

	//スポットライトの座標を得る
	Vector3 GetSpotLightPos(int32_t index);

	//スポットライトの向きを設定する
	void SetSpotLightDirection(int32_t index, Vector3 dir);

	//スポットライトの座標を得る
	Vector3 GetSpotLightDirection(int32_t index);

	//スポットライトの色を設定する
	void SetSpotLightColor(int32_t index, Color color);

	//スポットライトの色を得る
	Color GetSpotLightColor(int32_t index);

	//スポットライトの減衰係数を設定する
	void SetSpotLightAtten(int32_t index, Vector3 atten);

	//スポットライトの減衰係数を得る
	Vector3 GetSpotLightAtten(int32_t index);

	//スポットライトの減衰角度を設定する
	void SetSpotLightFactorAngle(int32_t index, Vector2 factorAngle);

	//スポットライトの減衰角度を得る
	Vector2 GetSpotLightFactorAngle(int32_t index);

private:
	//環境光の色
	Vector3 mAmbientColor = { 1, 1, 1 };

	//平行光源配列
	DirectionalLight mDirectionalLights[DIRECTIONAL_LIGHT_NUM];

	//点光源配列
	PointLight mPointLights[POINT_LIGHT_NUM];

	//スポットライト配列
	SpotLight mSpotLights[SPOT_LIGHT_NUM];

	//更新フラグ
	bool mChangeFlag = false;
};
