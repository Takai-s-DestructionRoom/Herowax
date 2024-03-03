#include "TestRenderStage.h"
#include <RDirectX.h>
#include "RenderTarget.h"
#include <algorithm>

void TestRenderStage::Render()
{
	struct sortObj {
		bool operator()(const RenderOrder& a, const RenderOrder& b) const noexcept {
			return a.anchorPoint.z < b.anchorPoint.z;
		}
	};
	std::stable_sort(mOrders.begin(), mOrders.end(), sortObj{});

	AllCall();
}
