/*
* @file Matrix4.h
* @brief 4x4の行列クラス
*/

#pragma once

//XMMATRIX参照するためだけにインクルード
//あまりにも不要だったら削除対象かも
#pragma warning(push, 0)
#include <DirectXMath.h>
#pragma warning(pop)

//掛ける相手の宣言だけしておく
//実際の計算はcppに置いてあるので本物の相手はそっちがincludeする
class Vector3;
class Float4;

class Matrix4
{
private:
	class Line {
		float m[4] = {};

	public:
		float operator[](const size_t i) const;
		float& operator[](const size_t i);
	};
	Line m[4] = {};

public:
	//単位行列
	Matrix4();

	//念のためXMMATRIXからの変換
	Matrix4(DirectX::XMMATRIX matrix);

	//添え字
	Line operator[](const size_t i) const;
	Line& operator[](const size_t i);

	//逆行列
	Matrix4 operator-() const;

	//比較
	bool operator==(const Matrix4& a) const;
	bool operator!=(const Matrix4& a) const;

	//加算
	Matrix4 operator+(const Matrix4& a) const;
	//減算
	Matrix4 operator-(const Matrix4& a) const;
	//乗算
	Matrix4 operator*(const Matrix4& a) const;

	//加算
	Matrix4& operator+=(const Matrix4& a);
	//減算
	Matrix4& operator-=(const Matrix4& a);
	//乗算
	Matrix4& operator*=(const Matrix4& a);

	//転置行列(このインスタンスを書き換える
	Matrix4& Transpose();

	//転置行列を取得
	//（このインスタンスは書き換えずに別のインスタンスとして取得する）
	Matrix4 GetTranspose() const;

	//平行移動行列らくらく生成
	static Matrix4 Translation(float x, float y, float z);
	static Matrix4 Translation(Vector3 pos);

	//スケーリング行列らくらく生成
	static Matrix4 Scaling(float x, float y, float z);

	//X軸回転行列らくらく生成
	static Matrix4 RotationX(float radian);
	//Y軸回転行列らくらく生成
	static Matrix4 RotationY(float radian);
	//Z軸回転行列らくらく生成
	static Matrix4 RotationZ(float radian);

	//ZXY(Roll,Pitch,Yaw)回転行列らくらく生成
	static Matrix4 RotationZXY(float radianX, float radianY, float radianZ);
	static Matrix4 RotationZXY(Vector3 rot);

	//ビュー変換行列楽々生成
	static Matrix4 View(Vector3 eye, Vector3 target, Vector3 up);

	//平行投影変換行列らくらく生成
	static Matrix4 OrthoGraphicProjection(float left, float right, float top, float bottom, float nearZ, float farZ);
	//透視投影変換行列らくらく生成
	static Matrix4 PerspectiveProjection(float fov, float aspect, float nearZ, float farZ);

	//ビューポート行列
	static Matrix4 Viewport(float x, float y, float width, float height, float minDepth, float maxDepth);
};

Vector3 operator*(const Vector3 vec, const Matrix4 mat);
Vector3& operator*=(Vector3& vec, const Matrix4 mat);

Float4 operator*(const Float4 f, const Matrix4 mat);
Float4& operator*=(Float4& f, const Matrix4 mat);
