#include "Float4.h"
#include "Vector2.h"
#include "Vector3.h"

Float4::operator Vector2() const
{
	return Vector2(x, y);
}

Float4::operator Vector3() const
{
	return Vector3(x, y, z);
}

Float4 Float4::operator/(const float& f) const
{
	Float4 temp = *this;
	temp.x /= f;
	temp.y /= f;
	temp.z /= f;
	temp.w /= f;
	return temp;
}

Float4& Float4::operator/=(const float& f)
{
	Float4 temp = *this / f;
	*this = temp;
	return *this;
}
