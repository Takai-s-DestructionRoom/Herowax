/*
* @file Quaternion.h
* @brief クォータニオンの諸々クラス
*/

#pragma once
#include "Vector3.h"
#include "Matrix4.h"

class Quaternion {
public:
	float x;
	float y;
	float z;
	float w;

	//単位回転のクォータニオン
	//これは「回転していない」のと同じである
	Quaternion();

	//各値を指定してクォータニオンのインスタンスを作る
	//通常、このコンストラクタを各値を理解していない者が呼ぶべきではない
	Quaternion(float x, float y, float z, float w);

	//クォータニオン同士が互いに一致するか比較する
	//これは二つの内積が1に近いかをテストする
	bool operator==(const Quaternion& rq) const;

	//クォータニオンの和
	Quaternion operator+(const Quaternion& rq) const;

	//クォータニオンの差
	Quaternion operator-(const Quaternion& rq) const;

	//スカラー倍
	Quaternion operator*(const float& f) const;

	//スカラー分の1倍
	Quaternion operator/(const float& f) const;

	//クォータニオンの和
	Quaternion operator+=(const Quaternion& rq);

	//クォータニオンの差
	Quaternion operator-=(const Quaternion& rq);

	//スカラー倍
	Quaternion& operator*=(const float& f);

	//スカラー分の1倍
	Quaternion& operator/=(const float& f);

	//クォータニオンの全ての値の符号を反転させる
	//回転軸と回転量全てが逆になるため、回転後の結果としては同じ回転を表す
	//逆回転が欲しいならConjugate関数
	//逆元が欲しいならInverse関数
	Quaternion operator-() const;

	//回転を合成する
	//非可換性であり、a * bとb * aは同じ回転ではない
	//a * bはbの回転の後にaの回転を行うのと同じ、注意
	Quaternion operator*(const Quaternion& rq) const;

	//回転を合成する
	//非可換性であり、a * bとb * aは同じ回転ではない
	//a * bはbの回転の後にaの回転を行うのと同じ、注意
	//この関数はa = b * a;をするのと同じ
	Quaternion& operator*=(const Quaternion& rq);

	//このクォータニオンの逆元を得る
	//元のインスタンスを書き換える
	Quaternion& Inverse();

	//このクォータニオンの逆元を得る
	//元のインスタンスは書き換えず、別のインスタンスとして得る
	Quaternion GetInverse() const;

	//このクォータニオンの逆回転(共役)を得る
	//元のインスタンスを書き換える
	Quaternion& Conjugate();

	//このクォータニオンの逆回転(共役)を得る
	//元のインスタンスは書き換えず、別のインスタンスとして得る
	Quaternion GetConjugate() const;

	//このクォータニオンを正規化する
	//普通に使う限りは、基本的に呼ぶ必要はない
	Quaternion& Normalize();

	//ノルムを得る
	//普通使わない
	float GetNorm() const;

	float Dot(const Quaternion& q) const;

	//オイラー角に変換する
	Vector3 ToEuler() const;

	//回転行列に変換する
	Matrix4 ToRotationMatrix() const;

	//以下便利関数？

	/// <summary>
	/// オイラー角で表された回転をクォータニオンで表す
	/// </summary>
	/// <param name="x">x軸周りの回転</param>
	/// <param name="y">y軸周りの回転</param>
	/// <param name="z">z軸周りの回転</param>
	/// <returns>回転クォータニオン</returns>
	static Quaternion Euler(const float& x, const float& y, const float& z);

	/// <summary>
	/// オイラー角で表された回転をクォータニオンで表す
	/// </summary>
	/// <param name="rot">オイラー角</param>
	/// <returns>回転クォータニオン</returns>
	static Quaternion Euler(const Vector3& rot);

	/// <summary>
	/// 回転行列からクォータニオンを生成する
	/// </summary>
	/// <param name="mat">回転行列</param>
	/// <returns>回転クォータニオン</returns>
	static Quaternion FromRotationMatrix(const Matrix4& mat);

	/// <summary>
	/// 任意の回転軸で指定した角度だけ回転するクォータニオンを生成する
	/// </summary>
	/// <param name="vec">回転軸</param>
	/// <param name="angle">角度(Radian)</param>
	/// <returns>回転クォータニオン</returns>
	static Quaternion AngleAxis(const Vector3& vec, const float& angle);

	/// <summary>
	/// 指定した方向を向く回転クォータニオンを生成する
	/// </summary>
	/// <param name="vec">向かせたい向き</param>
	/// <param name="vec">上方向ベクトル</param>
	/// <returns>回転クォータニオン</returns>
	static Quaternion LookAt(const Vector3& vec, const Vector3& up = {0, 1, 0});

	/// <summary>
	/// fromからtoへの回転クォータニオンを生成する
	/// </summary>
	/// <param name="from">回転前の姿勢クォータニオン</param>
	/// <param name="to">回転後の姿勢クォータニオン</param>
	/// <returns>回転クォータニオン</returns>
	static Quaternion FromToRotation(const Quaternion& from, const Quaternion& to);

	/// <summary>
	/// 二つのクォータニオンを線形補間する
	/// t=0でqA,t=1でqBに一致する
	/// </summary>
	/// <param name="qA">始点クォータニオン</param>
	/// <param name="qB">終点クォータニオン</param>
	/// <param name="t">時間(割合)</param>
	/// <returns>補間されたクォータニオン(正規化済み)</returns>
	static Quaternion Lerp(const Quaternion& qA, const Quaternion& qB, const float& t);

	/// <summary>
	/// 二つのクォータニオンを球面線形補間する
	/// t=0でqA,t=1でqBに一致する
	/// </summary>
	/// <param name="qA">始点クォータニオン</param>
	/// <param name="qB">終点クォータニオン</param>
	/// <param name="t">時間(割合)</param>
	/// <returns>補間されたクォータニオン(正規化済み)</returns>
	static Quaternion Slerp(const Quaternion& qA, const Quaternion& qB, const float& t);
};

//スカラー倍
Quaternion operator*(const float& f, const Quaternion& q);

//スカラー倍
Quaternion& operator*=(const float& f, Quaternion& q);

//ベクトルをクォータニオンで回す
Vector3 operator*(const Vector3& v, const Quaternion& q);

//ベクトルをクォータニオンで回す
Vector3& operator*=(Vector3& v, const Quaternion& q);
