#include "PolygonCollider.h"
#include "SphereCollider.h"

bool PolygonCollider::Collide(ICollider* hit, CollisionInfo* outputInfo)
{
    CollisionInfo info;

    if (hit->GetTypeIndentifier() == "SphereCollider") {
        SphereCollider* hitT = static_cast<SphereCollider*>(hit);

        if (ColPrimitive3D::CheckSphereToTriangle(hitT->mSphere, mPolygon, &info.closestPos)) {
            info.hitCollider = hit;
            info.hasClosestPos = true;
            if (outputInfo) {
                *outputInfo = info;
            }
            return true;
        }
        return false;
    }

    return false;
}
