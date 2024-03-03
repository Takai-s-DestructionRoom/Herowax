#include "Quaternion.h"
#include <math.h>

Quaternion::Quaternion()
	: x(0), y(0), z(0), w(1) {}

Quaternion::Quaternion(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w) {}

bool Quaternion::operator==(const Quaternion& rq) const
{
	return Dot(rq) > 0.99999f;
}

Quaternion Quaternion::operator+(const Quaternion& rq) const
{
	return Quaternion(x + rq.x, y + rq.y, z + rq.z, w + rq.w);
}

Quaternion Quaternion::operator-(const Quaternion& rq) const
{
	return Quaternion(x - rq.x, y - rq.y, z - rq.z, w - rq.w);
}

Quaternion Quaternion::operator*(const float& f) const
{
	return Quaternion(x * f, y * f, z * f, w * f);
}

Quaternion Quaternion::operator/(const float& f) const
{
	return Quaternion(x / f, y / f, z / f, w / f);
}

Quaternion Quaternion::operator+=(const Quaternion& rq)
{
	Quaternion temp = *this;
	temp = temp + rq;
	*this = temp;
	return *this;
}

Quaternion Quaternion::operator-=(const Quaternion& rq)
{
	Quaternion temp = *this;
	temp = temp - rq;
	*this = temp;
	return *this;
}

Quaternion& Quaternion::operator*=(const float& f)
{
	Quaternion temp = *this;
	temp = temp * f;
	*this = temp;
	return *this;
}

Quaternion& Quaternion::operator/=(const float& f)
{
	Quaternion temp = *this;
	temp = temp / f;
	*this = temp;
	return *this;
}

Quaternion Quaternion::operator-() const
{
	return Quaternion(-x, -y, -z, -w);
}

Quaternion Quaternion::operator*(const Quaternion& rq) const
{
	return Quaternion(
		y * rq.z - z * rq.y + x * rq.w + rq.x * w,
		z * rq.x - x * rq.z + y * rq.w + rq.y * w,
		x * rq.y - y * rq.x + z * rq.w + rq.z * w,
		-x * rq.x - y * rq.y - z * rq.z + w * rq.w
	);
}

Quaternion& Quaternion::operator*=(const Quaternion& rq)
{
	Quaternion temp = *this;
	temp = rq * temp;
	*this = temp;
	return *this;
}

Quaternion& Quaternion::Inverse()
{
	*this = GetConjugate() / (x * x + y * y + z * z + w * w);
	return *this;
}

Quaternion Quaternion::GetInverse() const
{
	Quaternion temp = *this;
	temp.Inverse();
	return temp;
}

Quaternion& Quaternion::Conjugate()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

Quaternion Quaternion::GetConjugate() const
{
	Quaternion temp = *this;
	temp.x = -temp.x;
	temp.y = -temp.y;
	temp.z = -temp.z;
	return temp;
}

Quaternion& Quaternion::Normalize()
{
	float norm = sqrtf(x * x + y * y + z * z + w * w);
	x /= norm;
	y /= norm;
	z /= norm;
	w /= norm;
	return *this;
}

float Quaternion::GetNorm() const
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

float Quaternion::Dot(const Quaternion& q) const
{
	return x * q.x + y * q.y + z * q.z + w * q.w;
}

Vector3 Quaternion::ToEuler() const
{
	float sinx = 2 * y * z + 2 * x * w;
	bool islocked = fabsf(sinx) > 0.99999f;
	return Vector3(
		asinf(sinx),
		islocked ? 0 : atan2f(-(2 * x * z - 2 * y * w), 2 * w * w + 2 * z * z - 1),
		islocked ? atan2f(2 * x * y + 2 * z * w, 2 * w * w + 2 * x * x - 1)
			: atan2f(-(2 * x * y - 2 * z * w), 2 * w * w + 2 * y * y - 1)
	);
}

Matrix4 Quaternion::ToRotationMatrix() const
{
	Matrix4 mat;
	float xy2 = x * y * 2;
	float xz2 = x * z * 2;
	float xw2 = x * w * 2;
	float yz2 = y * z * 2;
	float yw2 = y * w * 2;
	float zw2 = z * w * 2;
	float ww2 = w * w * 2;
	mat[0][0] = ww2 + 2 * x * x - 1;
	mat[0][1] = xy2 + zw2;
	mat[0][2] = xz2 - yw2;
	mat[1][0] = xy2 - zw2;
	mat[1][1] = ww2 + 2 * y * y - 1;
	mat[1][2] = yz2 + xw2;
	mat[2][0] = xz2 + yw2;
	mat[2][1] = yz2 - xw2;
	mat[2][2] = ww2 + 2 * z * z - 1;
	return mat;
}

Quaternion Quaternion::Euler(const float& x, const float& y, const float& z)
{
	return Quaternion::Euler(Vector3(x, y, z));
}

Quaternion Quaternion::Euler(const Vector3& rot)
{
	Quaternion x = AngleAxis({ 1, 0, 0 }, rot.x);
	Quaternion y = AngleAxis({ 0, 1, 0 }, rot.y);
	Quaternion z = AngleAxis({ 0, 0, 1 }, rot.z);

	Quaternion q = y * x * z;

	return q;

	//float cosx = cosf(0.5f * rot.x);
	//float sinx = sinf(0.5f * rot.x);
	//float cosy = cosf(0.5f * rot.y);
	//float siny = sinf(0.5f * rot.y);
	//float cosz = cosf(0.5f * rot.z);
	//float sinz = sinf(0.5f * rot.z);
	//return Quaternion(
	//	-cosx * siny * sinz + sinx * cosy * cosz,
	//	cosx * siny * cosz + sinx * cosy * sinz,
	//	sinx * siny * cosz + cosx * cosy * sinz,
	//	-sinx * siny * sinz + cosx * cosy * cosz
	//);
}

Quaternion Quaternion::FromRotationMatrix(const Matrix4& mat)
{
	float elem[4];
	elem[0] =  mat[0][0] - mat[1][1] - mat[2][2] + 1;
	elem[1] = -mat[0][0] + mat[1][1] - mat[2][2] + 1;
	elem[2] = -mat[0][0] - mat[1][1] + mat[2][2] + 1;
	elem[3] =  mat[0][0] + mat[1][1] + mat[2][2] + 1;

	int32_t biggestIndex = 0;
	for (int32_t i = 1; i < 4; i++) {
		if (elem[i] > elem[biggestIndex]) {
			biggestIndex = i;
		}
	}

	if (elem[biggestIndex] < 0.0f) {
		//行列がおかしい
		return Quaternion(0, 0, 0, 0);
	}

	float v = sqrtf(elem[biggestIndex]) * 0.5f;
	float mult = 0.25f / v;
	switch (biggestIndex) {
	case 0:
		return Quaternion(
			v,
			(mat[0][1] + mat[1][0]) * mult,
			(mat[2][0] + mat[0][2]) * mult,
			(mat[0][2] - mat[2][1]) * mult
		);
	case 1:
		return Quaternion(
			(mat[0][1] + mat[1][0]) * mult,
			v,
			(mat[1][2] + mat[2][1]) * mult,
			(mat[2][0] - mat[0][2]) * mult
		);
	case 2:
		return Quaternion(
			(mat[2][0] + mat[0][2]) * mult,
			(mat[1][2] + mat[2][1]) * mult,
			v,
			(mat[0][1] - mat[1][0]) * mult
		);
	case 3:
		return Quaternion(
			(mat[1][2] - mat[2][1]) * mult,
			(mat[2][0] - mat[0][2]) * mult,
			(mat[0][1] - mat[1][0]) * mult,
			v
		);
	}

	return Quaternion(0, 0, 0, 0);
}

Quaternion Quaternion::AngleAxis(const Vector3& vec, const float& angle)
{
	Quaternion q(
		vec.x * sinf(angle / 2.0f),
		vec.y * sinf(angle / 2.0f),
		vec.z * sinf(angle / 2.0f),
		cosf(angle / 2.0f)
	);
	q.Normalize();
	return q;
}

Quaternion Quaternion::LookAt(const Vector3& vec, const Vector3& up)
{
	Matrix4 mat = -Matrix4::View({ 0, 0, 0 }, vec.GetNormalize(), up);
	return Quaternion::FromRotationMatrix(mat);
}

Quaternion Quaternion::FromToRotation(const Quaternion& from, const Quaternion& to) {
	return from * to.GetInverse();
}

Quaternion Quaternion::Lerp(const Quaternion& qA, const Quaternion& qB, const float& t)
{
	if (qA == qB) return qA;

	Quaternion result;

	if (qA.Dot(qB) < 0) {
		result = (1 - t) * qA + t * -qB;
		result.Normalize();
		return result;
	}

	result = (1 - t) * qA + t * qB;
	result.Normalize();
	return result;
}

Quaternion Quaternion::Slerp(const Quaternion& qA, const Quaternion& qB, const float& t)
{
	if (qA == qB) return qA;

	Quaternion result;
	float angle = acosf(qA.Dot(qB));

	if (qA.Dot(qB) < 0) {
		result = (sinf(1 - t) * angle / sinf(angle)) * qA + (sinf(t) * angle / sinf(angle)) * -qB;
		result.Normalize();
		return result;
	}

	result = result = (sinf(1 - t) * angle / sinf(angle)) * qA + (sinf(t) * angle / sinf(angle)) * qB;
	result.Normalize();
	return result;
}

Quaternion operator*(const float& f, const Quaternion& q)
{
	return q * f;
}

Quaternion& operator*=(const float& f, Quaternion& q)
{
	q *= f;
	return q;
}

Vector3 operator*(const Vector3& v, const Quaternion& q)
{
	Quaternion vec(v.x, v.y, v.z, 0);
	Quaternion result = q * vec * q.GetConjugate();
	return Vector3(result.x, result.y, result.z);
}

Vector3& operator*=(Vector3& v, const Quaternion& q)
{
	Vector3 temp = v;
	temp = temp * q;
	v = temp;
	return v;
}
