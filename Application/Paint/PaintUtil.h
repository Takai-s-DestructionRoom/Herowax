#pragma once
#include <Float4.h>
#include <Vector3.h>
#include <Vector2.h>
#include <Matrix4.h>
#include <vector>

namespace PaintUtil {
	Vector2 CalcPointTexCoord(std::vector<Vector3> mesh, std::vector<Vector2> texcoords, Vector3 point, Matrix4 vpMatrix);
}