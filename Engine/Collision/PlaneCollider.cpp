#include "PlaneCollider.h"
#include "SphereCollider.h"

bool PlaneCollider::Collide(ICollider* hit, CollisionInfo* outputInfo)
{
    if (hit->GetTypeIndentifier() == "SphereCollider") {
        SphereCollider* hitT = static_cast<SphereCollider*>(hit);

        if (ColPrimitive3D::CheckSphereToPlane(hitT->mSphere,mPlane)) {
            if (outputInfo) {
                outputInfo->hitCollider = hit;
            }
            return true;
        }
        return false;
    }

    return false;
}
