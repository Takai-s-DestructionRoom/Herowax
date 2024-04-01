#include "AABBCollider.h"
#include "SphereCollider.h"

bool AABBCollider::Collide(ICollider* hit, CollisionInfo* outputInfo)
{
    CollisionInfo info;
    if (hit->GetTypeIndentifier() == "SphereCollider") {
        SphereCollider* hitT = static_cast<SphereCollider*>(hit);

        if (ColPrimitive3D::CheckSphereToAABB(hitT->mSphere, mAABB)) {
            if (outputInfo) {
                outputInfo->hitCollider = hit;
            }
            return true;
        }
        return false;
    }

    return false;
}
