#include "Vertex.h"
#include <cassert>

bool VertexPNU::operator==(const VertexPNU& a) const
{
	return pos == a.pos && normal == a.normal && uv == a.uv;
}

void VertexPNU::CalcNormalVec(VertexPNU list[], const uint32_t indexlist[], const uint32_t indexcount)
{
	assert(indexcount % 3 == 0);
	for (uint32_t i = 0; i < indexcount / 3; i++) {
		uint32_t index0 = indexlist[i * 3 + 0];
		uint32_t index1 = indexlist[i * 3 + 1];
		uint32_t index2 = indexlist[i * 3 + 2];

		Vector3 p0 = list[index0].pos;
		Vector3 p1 = list[index1].pos;
		Vector3 p2 = list[index2].pos;

		Vector3 v1 = p1 - p0;
		Vector3 v2 = p2 - p0;

		Vector3 normal = v1.Cross(v2);
		normal.Normalize();

		list[index0].normal = normal;
		list[index1].normal = normal;
		list[index2].normal = normal;
	}
}

void VertexPNU::CalcNormalVec(std::vector<VertexPNU> list, std::vector<uint32_t> indexlist) {
	assert(indexlist.size() % 3 == 0);
	for (size_t i = 0; i < list.size() / 3; i++) {
		size_t index0 = indexlist[i * 3 + 0];
		size_t index1 = indexlist[i * 3 + 1];
		size_t index2 = indexlist[i * 3 + 2];

		Vector3 p0 = list[index0].pos;
		Vector3 p1 = list[index1].pos;
		Vector3 p2 = list[index2].pos;

		Vector3 v1 = p1 - p0;
		Vector3 v2 = p2 - p0;

		Vector3 normal = v1.Cross(v2);
		normal.Normalize();

		list[index0].normal = normal;
		list[index1].normal = normal;
		list[index2].normal = normal;
	}
}
