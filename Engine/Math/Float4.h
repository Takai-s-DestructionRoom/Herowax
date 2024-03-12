/*
* @file Float4.h
* @brief float4つをまとめて一つで扱うクラス
*/

#pragma once
//仲良し相手宣言
class Vector2;
class Vector3;

class Float4
{
public:
	float x;
	float y;
	float z;
	float w;

	Float4() : x(0), y(0), z(0), w(0) {}
	Float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	//z,w成分を捨ててVector2へキャスト
	operator Vector2() const;

	//w成分を捨ててVector3へキャスト
	operator Vector3() const;

	Float4 operator*(const float& f) const;
	Float4& operator*=(const float& f);

	Float4 operator/(const float& f) const;
	Float4& operator/=(const float& f);
};
