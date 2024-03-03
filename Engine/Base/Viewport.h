/*
* @file Viewport.h
* @brief ビューポート。
*/

#pragma once
struct Viewport
{
	float width;
	float height;
	float topleftX;
	float topleftY;
	float minDepth;
	float maxDepth;

	bool operator==(const Viewport& a) const {
		return width == a.width && height == a.height
			&& topleftX == a.topleftX && topleftY == a.topleftY
			&& minDepth == a.minDepth && maxDepth == a.maxDepth;
	}

	bool operator!=(const Viewport& a) const {
		return !operator==(a);
	}
};
