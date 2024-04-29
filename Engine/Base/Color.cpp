#include "Color.h"

const Color Color::kRed(1.f, 0, 0, 1.f);
const Color Color::kGreen(0, 1.0f, 0, 1.f);
const Color Color::kBlue(0, 0, 1.0f, 1.f);
const Color Color::kYellow(1.0f, 1.0f, 0, 1.f);
const Color Color::kPink(1.0f, 0, 1.0f, 1.f);
const Color Color::kLightblue(0, 1.0f, 1.0f, 1.f);
const Color Color::kWhite(1.0f, 1.0f, 1.0f, 1.f);
const Color Color::kBlack(0, 0, 0, 1.f);

const Color Color::kFireOutside(1.f, 0.25f, 0.05f, 1.f);
const Color Color::kFireInside(0.9f, 0.8f, 0.05f, 1.f);
const Color Color::kWaxColor(0.8f, 0.6f, 0.35f, 1.f);

Color::Color(int32_t hexcolor)
{
	int32_t r_, g_, b_;
	a = 1;
	r_ = hexcolor & 0xff0000;
	r_ = r_ >> 16;
	g_ = hexcolor & 0xff00;
	g_ = g_ >> 8;
	b_ = hexcolor & 0xff;

	r = r_ / 255.0f;
	g = g_ / 255.0f;
	b = b_ / 255.0f;
}

Color Color::operator+(const float num) const
{
	return { r + num, g + num, b + num };
}

Color Color::operator-(const float num) const
{
	return { r - num, g - num, b - num };
}

Color Color::operator*(const float num) const
{
	return { r * num, g * num, b * num };
}

Color Color::operator/(const float num) const
{
	return { r / num, g / num, b / num };
}

Color Color::operator=(const Color& col)
{
	r = col.r, g = col.g, b = col.b, a = col.a;
	return *this;
}

bool Color::operator==(const Color& o) const
{
	return r == o.r && g == o.g && b == o.b && a == o.a;
}

Color Color::convertFromHSVA(int32_t h, int32_t s, int32_t v, int32_t a)
{
	Color c;

	while (h < 0) { h += 360; }
	while (h > 360) { h -= 360; }
	if (s < 0) { s = 0; }
	if (s > 100) { s = 100; }
	if (v < 0) { v = 0; }
	if (v > 100) { v = 100; }

	s = (int32_t)(255 * s / 100.0f);
	v = (int32_t)(255 * v / 100.0f);

	int32_t MAX = v;
	int32_t MIN = (int32_t)(MAX - ((s / 255.0f) * MAX));

	if (h >= 0 && h <= 60) {
		return Color(MAX / 255.0f, ((h / 60.0f) * (MAX - MIN) + MIN) / 255.0f, MIN / 255.0f, a / 255.0f);
	}
	else if (h >= 61 && h <= 120) {
		return Color((((120 - h) / 60.0f) * (MAX - MIN) + MIN) / 255.0f, MAX / 255.0f, MIN / 255.0f, a / 255.0f);
	}
	else if (h >= 121 && h <= 180) {
		return Color(MIN / 255.0f, MAX / 255.0f, (((h - 120) / 60.0f) * (MAX - MIN) + MIN) / 255.0f, a / 255.0f);
	}
	else if (h >= 181 && h <= 240) {
		return Color(MIN / 255.0f, (((240 - h) / 60.0f) * (MAX - MIN) + MIN) / 255.0f, MAX / 255.0f, a / 255.0f);
	}
	else if (h >= 241 && h <= 300) {
		return Color((((h - 240) / 60.0f) * (MAX - MIN) + MIN) / 255.0f, MIN / 255.0f, MAX / 255.0f, a / 255.0f);
	}
	else if (h >= 301 && h <= 360) {
		return Color(MAX / 255.0f, MIN / 255.0f, (((360 - h) / 60.0f) * (MAX - MIN) + MIN) / 255.0f, a / 255.0f);
	}

	return Color();
}
