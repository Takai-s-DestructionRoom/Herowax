/*
* @file Vector3.h
* @brief 3次元ベクトル
*/

#pragma once

//仲良し相手宣言
class Vector2;
class Float4;

class Vector3
{
public:
	float x;
	float y;
	float z;

	//ゼロベクトル
	Vector3();
	const static Vector3 ZERO;
	const static Vector3 ONE;

	//任意の成分ベクトル
	Vector3(float x, float y, float z);

	~Vector3();

	//比較
	bool operator==(const Vector3& a) const;
	bool operator!=(const Vector3& a) const;

	//狭義の弱順序(ベクトルの長さを見たりするわけじゃないので普通使わない）
	bool operator<(const Vector3& a) const;

	//ベクトルの足し算
	Vector3 operator+(const Vector3& a) const;
	//ベクトルの引き算
	Vector3 operator-(const Vector3& a) const;
	//逆ベクトル
	Vector3 operator-() const;
	//定数倍
	Vector3 operator*(const float a) const;
	//定数分の1倍
	Vector3 operator/(const float a) const;

	//ベクトルの足し算
	Vector3& operator+=(const Vector3& a);
	//ベクトルの引き算
	Vector3& operator-=(const Vector3& a);
	//ベクトルのかけ算
	Vector3& operator*=(const Vector3& a);
	//定数倍
	Vector3& operator*=(const float a);
	//定数分の1倍
	Vector3& operator/=(const float a);

	//内積
	float Dot(const Vector3& a) const;
	//外積
	Vector3 Cross(const Vector3& a) const;

	//ベクトルの長さ（√を使うので重め）
	float Length() const;
	//ベクトルのべき乗長さ（√を使わないので軽め）
	float LengthSq() const;

	//正規化（このインスタンスを書き換える）
	Vector3& Normalize();
	//正規化ベクトルを取得
	//（このインスタンスは書き換えずに別のインスタンスとして取得する）
	Vector3 GetNormalize() const;

	//z成分を捨ててVector2へキャスト
	operator Vector2() const;

	//w成分を1としてFloat4へキャスト
	operator Float4() const;
};

//左辺にfloatを置いたときに*, /が出来ないのでそれ対策

//定数倍
Vector3 operator*(const float a, const Vector3& b);

//線形補間(1次関数補間)
const Vector3 lerp(const Vector3& start, const Vector3& end, const float t);
