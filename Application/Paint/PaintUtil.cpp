#include "PaintUtil.h"

Vector2 PaintUtil::CalcPointTexCoord(std::vector<Vector3> meshPotisions, std::vector<Vector2> texcoords, Vector3 point, Matrix4 vpMatrix)
{
	Float4 p1 = meshPotisions[0] * vpMatrix;
	Float4 p2 = meshPotisions[1] * vpMatrix;
	Float4 p3 = meshPotisions[2] * vpMatrix;
	Float4 cp = point * vpMatrix;

	p1 /= p1.w;
	p2 /= p2.w;
	p3 /= p3.w;
	cp /= cp.w;

	float s1 = 0.5f * ((p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x));
	float s2 = 0.5f * ((p3.x - cp.x) * (p1.y - cp.y) - (p3.y - cp.y) * (p1.x - cp.x));
	float s3 = 0.5f * ((p1.x - cp.x) * (p2.y - cp.y) - (p1.y - cp.y) * (p2.x - cp.x));

	float u = s2 / s1;
	float v = s3 / s1;
	float w = 1 / ((1 - u - v) * 1 / p1.w + u * 1 / p2.w + v * 1 / p3.w);
	return w * ((1 - u - v) * texcoords[0] / p1.w + u * texcoords[1] / p2.w + v * texcoords[2] / p3.w);
}
