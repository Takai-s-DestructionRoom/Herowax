#include "Vector2.h"
#include <math.h>
#include "Vector3.h"
#include "Float4.h"

const float EPSILON = 0.000001f;

Vector2::Vector2() : x(0), y(0) {}

Vector2::Vector2(int32_t x, int32_t y) :
	x(static_cast<float>(x)),
	y(static_cast<float>(y)) {}

Vector2::Vector2(float x, float y) : x(x), y(y) {}

Vector2::~Vector2() {}

bool Vector2::operator==(const Vector2& a) const {
	return x == a.x && y == a.y;
}

bool Vector2::operator!=(const Vector2& a) const {
	return x != a.x || y != a.y;
}

Vector2 Vector2::operator+(const Vector2& a) const {
	return Vector2(x + a.x, y + a.y);
}

Vector2 Vector2::operator-(const Vector2& a) const {
	return Vector2(x - a.x, y - a.y);
}

Vector2 Vector2::operator-() const {
	return Vector2(-x, -y);
}

Vector2 Vector2::operator*(const float a) const {
	return Vector2(x * a, y * a);
}

Vector2 operator*(const float a, const Vector2& b) {
	return Vector2(b.x * a, b.y * a);
}

Vector2 Vector2::operator/(const float a) const {
	return Vector2(x / a, y / a);
}

Vector2& Vector2::operator+=(const Vector2& a) {
	x += a.x;
	y += a.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& a) {
	x -= a.x;
	y -= a.y;
	return *this;
}

Vector2& Vector2::operator*=(const float a) {
	x *= a;
	y *= a;
	return *this;
}

Vector2& Vector2::operator/=(const float a) {
	x /= a;
	y /= a;
	return *this;
}

float Vector2::Dot(const Vector2& a) const {
	return x * a.x + y * a.y;
}

float Vector2::Cross(const Vector2& a) const {
	return x * a.y - a.x * y;
}

float Vector2::Length() const {
	return sqrtf(LengthSq());
}

float Vector2::LengthSq() const {
	return x * x + y * y;
}

Vector2& Vector2::Normalize() {
	float length = Length();
	if (length != 0) {
		x = x / length;
		y = y / length;
	}
	else {
		x = 0;
		y = 0;
	}
	return *this;
}

Vector2 Vector2::GetNormalize() const {
	float length = Length();
	float dx = 0;
	float dy = 0;
	if (length != 0) {
		dx = x / length;
		dy = y / length;
	}
	return Vector2(dx, dy);
}

Vector2::operator Vector3() const
{
	return Vector3(x, y, 0);
}

Vector2::operator Float4() const
{
	return Float4(x, y, 0, 1);
}
