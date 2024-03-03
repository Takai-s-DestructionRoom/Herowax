#include "Matrix4.h"
#include <stdexcept>
#include "Util.h"
#include "Vector3.h"
#include "Float4.h"

const float EPSILON = 0.000001f;

float Matrix4::Line::operator[](const size_t i) const
{
	if (i >= 4) {
		throw std::out_of_range("4列なのに5列目以降を触るな");
	}
	return m[i];
}

float& Matrix4::Line::operator[](const size_t i)
{
	if (i >= 4) {
		throw std::out_of_range("4列なのに5列目以降を触るな");
	}
	return m[i];
}

Matrix4::Matrix4()
{
	m[0][0] = 1;
	m[1][1] = 1;
	m[2][2] = 1;
	m[3][3] = 1;
}

Matrix4::Matrix4(DirectX::XMMATRIX matrix)
{
	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = 0; j < 4; j++) {
			m[i][j] = matrix.r[i].m128_f32[j];
		}
	}
}

Matrix4::Line Matrix4::operator[](const size_t i) const
{
	if (i >= 4) {
		throw std::out_of_range("4行4列なのに5行目以降を触るな");
	}
	return m[i];
}

Matrix4::Line& Matrix4::operator[](const size_t i)
{
	if (i >= 4) {
		throw std::out_of_range("4行4列なのに5行目以降を触るな");
	}
	return m[i];
}

Matrix4 Matrix4::operator-() const
{
	Matrix4 temp;
	float mat[4][8] = {0};

	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = 0; j < 4; j++) {
			mat[i][j] = m[i][j];
		}
	}

	mat[0][4] = 1;
	mat[1][5] = 1;
	mat[2][6] = 1;
	mat[3][7] = 1;

	for (int32_t n = 0; n < 4; n++) {
		//最大の絶対値を探索する(とりあえず対象成分を最大と仮定しておく)
		float max = abs(mat[n][n]);
		int32_t maxIndex = n;

		for (int32_t i = n + 1; i < 4; i++) {
			if (abs(mat[i][n]) > max) {
				max = abs(mat[i][n]);
				maxIndex = i;
			}
		}

		//最大の絶対値が0だったら逆行列は求められない
		if (abs(mat[maxIndex][n]) <= EPSILON) {
			return temp; //とりあえず単位行列返しちゃう
		}

		//入れ替え
		if (n != maxIndex) {
			for (int32_t i = 0; i < 8; i++) {
				float f = mat[maxIndex][i];
				mat[maxIndex][i] = mat[n][i];
				mat[n][i] = f;
			}
		}

		//掛けたら1になる値を算出
		float mul = 1 / mat[n][n];

		//掛ける
		for (int32_t i = 0; i < 8; i++) {
			mat[n][i] *= mul;
		}

		//他全部0にする
		for (int32_t i = 0; i < 4; i++) {
			if (n == i) {
				continue;
			}

			mul = -mat[i][n];

			for (int32_t j = 0; j < 8; j++) {
				mat[i][j] += mat[n][j] * mul;
			}
		}
	}

	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = 0; j < 4; j++) {
			temp[i][j] = mat[i][j + 4];
		}
	}

	return temp;
}

bool Matrix4::operator==(const Matrix4& a) const
{
	bool ok = true;
	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = 0; j < 4; j++) {
			if (m[i][j] != a[i][j]) {
				ok = false;
			}
		}
	}
	return ok;
}

bool Matrix4::operator!=(const Matrix4& a) const
{
	return !(*this == a);
}

Matrix4 Matrix4::operator+(const Matrix4& a) const
{
	Matrix4 temp(*this);
	temp += a;
	return temp;
}

Matrix4 Matrix4::operator-(const Matrix4& a) const
{
	Matrix4 temp(*this);
	temp -= a;
	return temp;
}

Matrix4 Matrix4::operator*(const Matrix4& a) const
{
	Matrix4 temp(*this);
	temp *= a;
	return temp;
}

Matrix4& Matrix4::operator+=(const Matrix4& a)
{
	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = 0; j < 4; j++) {
			m[i][j] += a[i][j];
		}
	}
	return *this;
}

Matrix4& Matrix4::operator-=(const Matrix4& a)
{
	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = 0; j < 4; j++) {
			m[i][j] -= a[i][j];
		}
	}
	return *this;
}

Matrix4& Matrix4::operator*=(const Matrix4& a)
{
	Matrix4 temp(*this);
	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = 0; j < 4; j++) {
			float f = 0;
			for (int32_t k = 0; k < 4; k++) {
				f += temp.m[i][k] * a[k][j];
			}
			m[i][j] = f;
		}
	}
	return *this;
}

Matrix4& Matrix4::Transpose()
{
	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = i; j < 4; j++) {
			float f = m[i][j];
			m[i][j] = m[j][i];
			m[j][i] = f;
		}
	}

	return *this;
}

Matrix4 Matrix4::GetTranspose() const
{
	Matrix4 temp(*this);
	temp.Transpose();
	return temp;
}

Matrix4 Matrix4::Translation(float x, float y, float z)
{
	Matrix4 mat;
	mat[3][0] = x;
	mat[3][1] = y;
	mat[3][2] = z;
	return mat;
}

Matrix4 Matrix4::Scaling(float x, float y, float z)
{
	Matrix4 mat;
	mat[0][0] = x;
	mat[1][1] = y;
	mat[2][2] = z;
	return mat;
}

Matrix4 Matrix4::RotationX(float radian)
{
	Matrix4 mat;
	mat[1][1] = cosf(radian);
	mat[1][2] = sinf(radian);
	mat[2][1] = -sinf(radian);
	mat[2][2] = cosf(radian);
	return mat;
}

Matrix4 Matrix4::RotationY(float radian)
{
	Matrix4 mat;
	mat[0][0] = cosf(radian);
	mat[0][2] = -sinf(radian);
	mat[2][0] = sinf(radian);
	mat[2][2] = cosf(radian);
	return mat;
}

Matrix4 Matrix4::RotationZ(float radian)
{
	Matrix4 mat;
	mat[0][0] = cosf(radian);
	mat[0][1] = sinf(radian);
	mat[1][0] = -sinf(radian);
	mat[1][1] = cosf(radian);
	return mat;
}

Matrix4 Matrix4::RotationZXY(float radianX, float radianY, float radianZ)
{
	Matrix4 mat;
	mat *= RotationZ(radianZ);
	mat *= RotationX(radianX);
	mat *= RotationY(radianY);
	return mat;
}

Matrix4 Matrix4::View(Vector3 eye, Vector3 target, Vector3 up)
{
	Matrix4 mat;

	Vector3 direction = target - eye;
	direction.Normalize();

	Vector3 xVec = up.Cross(direction);
	xVec.Normalize();
	Vector3 yVec = direction.Cross(xVec);
	yVec.Normalize();

	mat[0][0] = xVec.x;
	mat[0][1] = xVec.y;
	mat[0][2] = xVec.z;
	mat[1][0] = yVec.x;
	mat[1][1] = yVec.y;
	mat[1][2] = yVec.z;
	mat[2][0] = direction.x;
	mat[2][1] = direction.y;
	mat[2][2] = direction.z;
	mat[3][0] = eye.x;
	mat[3][1] = eye.y;
	mat[3][2] = eye.z;

	mat = -mat;
	return mat;
}

Matrix4 Matrix4::OrthoGraphicProjection(float left, float right, float top, float bottom, float nearZ, float farZ)
{
	Matrix4 mat;

	float width = 1.0f / (right - left);
	float height = 1.0f / (top - bottom);
	float range = 1.0f / (farZ - nearZ);

	mat[0][0] = width * 2;
	mat[1][1] = height * 2;
	mat[2][2] = range;

	mat[3][0] = -(left + right) * width;
	mat[3][1] = -(top + bottom) * height;
	mat[3][2] = -range * nearZ;
	mat[3][3] = 1.0f;

	return mat;
}

Matrix4 Matrix4::PerspectiveProjection(float fov, float aspect, float nearZ, float farZ)
{
	Matrix4 mat;

	Matrix4 _a; //アスペクト比を1:1に
	_a[0][0] = 1 / aspect;

	Matrix4 _b; //直方体に
	_b[0][0] = 1 / tanf(fov / 2);
	_b[1][1] = 1 / tanf(fov / 2);

	Matrix4 _c; //原点にくっつけて縮める
	_c[2][2] = farZ * (1 / (farZ - nearZ));
	_c[3][2] = farZ * (-nearZ / (farZ - nearZ));

	mat = _a * _b * _c;
	mat[2][3] = 1;
	mat[3][3] = 0;

	return mat;
}

Matrix4 Matrix4::Viewport(float x, float y, float width, float height, float minDepth, float maxDepth)
{
	Matrix4 result;
	result[0][0] = width / 2.0f;
	result[1][1] = -height / 2.0f;
	result[2][2] = maxDepth - minDepth;
	result[3][0] = x + width / 2.0f;
	result[3][1] = y + height / 2.0f;
	result[3][2] = minDepth;
	return result;
}

Vector3 operator*(const Vector3 vec, const Matrix4 mat)
{
	Vector3 temp = vec;
	temp.x = vec.x * mat[0][0] + vec.y * mat[1][0] + vec.z * mat[2][0];
	temp.y = vec.x * mat[0][1] + vec.y * mat[1][1] + vec.z * mat[2][1];
	temp.z = vec.x * mat[0][2] + vec.y * mat[1][2] + vec.z * mat[2][2];
	return temp;
}

Vector3& operator*=(Vector3& vec, const Matrix4 mat)
{
	Vector3 temp = vec * mat;
	vec = temp;
	return vec;
}

Float4 operator*(const Float4 f, const Matrix4 mat)
{
	Float4 temp = f;
	temp.x = f.x * mat[0][0] + f.y * mat[1][0] + f.z * mat[2][0] + f.w * mat[3][0];
	temp.y = f.x * mat[0][1] + f.y * mat[1][1] + f.z * mat[2][1] + f.w * mat[3][1];
	temp.z = f.x * mat[0][2] + f.y * mat[1][2] + f.z * mat[2][2] + f.w * mat[3][2];
	temp.w = f.x * mat[0][3] + f.y * mat[1][3] + f.z * mat[2][3] + f.w * mat[3][3];
	return temp;
}

Float4& operator*=(Float4& f, const Matrix4 mat)
{
	Float4 temp = f * mat;
	f = temp;
	return f;
}
