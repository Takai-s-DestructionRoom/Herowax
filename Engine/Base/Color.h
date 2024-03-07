/*
* @file Color.h
* @brief RGBで定義する色。0xffffffなどのuintからこれに変換したりもできる
*/

#pragma once
#include <cstdint>

struct RGBA {
	int32_t r;
	int32_t g;
	int32_t b;
	int32_t a;
};

struct HSVA {
	int32_t h;
	int32_t s;
	int32_t v;
	int32_t a;
};

class Color
{
public:
	float r = 1;
	float g = 1;
	float b = 1;
	float a = 1;

	Color() {}
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
	Color(int32_t hexcolor);

	//すぐ呼べる用の色
	const static Color kRed;
	const static Color kGreen;
	const static Color kBlue;
	const static Color kYellow;
	const static Color kPink;
	const static Color kLightblue;
	const static Color kWhite;
	const static Color kBlack;

	bool operator==(const Color& o) const;

	static Color convertFromHSVA(HSVA hsva) {
		return convertFromHSVA(hsva.h, hsva.s, hsva.v, hsva.a);
	}
	static Color convertFromHSVA(int32_t h, int32_t s, int32_t v, int32_t a);
};

