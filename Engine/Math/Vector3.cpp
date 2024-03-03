#include "Vector3.h"
#include <math.h>
#include "Vector2.h"
#include "Float4.h"

const float EPSILON = 0.000001f;
const Vector3 Vector3::ZERO = Vector3();

Vector3::Vector3() : x(0), y(0), z(0) {}

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

Vector3::~Vector3() {}

bool Vector3::operator==(const Vector3& a) const {
	return x == a.x && y == a.y && z == a.z;
}

bool Vector3::operator!=(const Vector3& a) const {
	return x != a.x || y != a.y || z == a.z;
}

bool Vector3::operator<(const Vector3& a) const
{
	if (x < a.x) return true;
	if (x == a.x) {
		if (y < a.y) return true;
		if (y == a.y) {
			if (z < a.z) return true;
		}
	}
	return false;
}

Vector3 Vector3::operator+(const Vector3& a) const {
	return Vector3(x + a.x, y + a.y, z + a.z);
}

Vector3 Vector3::operator-(const Vector3& a) const {
	return Vector3(x - a.x, y - a.y, z - a.z);
}

Vector3 Vector3::operator-() const {
	return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator*(const float a) const {
	return Vector3(x * a, y * a, z * a);
}

Vector3 operator*(const float a, const Vector3& b) {
	return Vector3(b.x * a, b.y * a, b.z * a);
}

Vector3 Vector3::operator/(const float a) const {
	return Vector3(x / a, y / a, z / a);
}

Vector3& Vector3::operator+=(const Vector3& a) {
	x += a.x;
	y += a.y;
	z += a.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& a) {
	x -= a.x;
	y -= a.y;
	z -= a.z;
	return *this;
}

Vector3& Vector3::operator*=(const Vector3& a)
{
	x *= a.x;
	y *= a.y;
	z *= a.z;
	return *this;
}

Vector3& Vector3::operator*=(const float a) {
	x *= a;
	y *= a;
	z *= a;
	return *this;
}

Vector3& Vector3::operator/=(const float a) {
	x /= a;
	y /= a;
	z /= a;
	return *this;
}

float Vector3::Dot(const Vector3& a) const {
	return x * a.x + y * a.y + z * a.z;
}

Vector3 Vector3::Cross(const Vector3& a) const {
	float nx = y * a.z - z * a.y;
	float ny = z * a.x - x * a.z;
	float nz = x * a.y - y * a.x;
	return Vector3(nx, ny, nz);
}

float Vector3::Length() const {
	return sqrtf(LengthSq());
}

float Vector3::LengthSq() const {
	return x * x + y * y + z * z;
}

Vector3& Vector3::Normalize() {
	float length = Length();
	if (length != 0) {
		x = x / length;
		y = y / length;
		z = z / length;
	}
	else {
		x = 0;
		y = 0;
		z = 0;
	}
	return *this;
}

Vector3 Vector3::GetNormalize() const {
	float length = Length();
	float dx = 0;
	float dy = 0;
	float dz = 0;
	if (length != 0) {
		dx = x / length;
		dy = y / length;
		dz = z / length;
	}
	return Vector3(dx, dy, dz);
}

Vector3::operator Vector2() const
{
	return Vector2(x, y);
}

Vector3::operator Float4() const
{
	return Float4(x, y, z, 1);
}

const Vector3 lerp(const Vector3& start, const Vector3& end, const float t)
{
	return start + (end - start) * t;
}
