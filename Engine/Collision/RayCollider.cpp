#include "RayCollider.h"
#include "SphereCollider.h"
#include "PolygonCollider.h"

void RayCollider::Update()
{
    mFlagSkip = false;
    if (!mOptPierce) {
        mMemDis = FLT_MAX;
        mNearest = nullptr;
        Colliders::ColListAccessor access;
        for (std::unique_ptr<ICollider>& ptr : *access.mList) {
            CollisionInfo info;
            if (Collide(ptr.get(), &info)) {
                if (info.hasDistance && info.hasInter) {
                    if (info.distance < mMemDis) {
                        mMemDis = info.distance;
                        mMemInfo = info;
                        mNearest = ptr.get();
                    }
                }
            }
        }
        mFlagSkip = true;
    }
}

bool RayCollider::Collide(ICollider* hit, CollisionInfo* outputInfo)
{
    if (mFlagSkip) {
        if (outputInfo) *outputInfo = mMemInfo;
        return hit == mNearest;
    }

    CollisionInfo info;
    if (hit->GetTypeIndentifier() == "SphereCollider") {
        SphereCollider* hitT = static_cast<SphereCollider*>(hit);

        if (ColPrimitive3D::CheckRayToSphere(mRay, hitT->mSphere, &info.distance, &info.inter)) {
            info.hitCollider = hit;
            info.hasDistance = true;
            info.hasInter = true;
            if (outputInfo) {
                *outputInfo = info;
            }
            return true;
        }
        return false;
    }

    if (hit->GetTypeIndentifier() == "PolygonCollider") {
        PolygonCollider* hitT = static_cast<PolygonCollider*>(hit);

        if (ColPrimitive3D::CheckRayToTriangle(mRay, hitT->mPolygon, &info.distance, &info.inter)) {
            info.hitCollider = hit;
            info.hasDistance = true;
            info.hasInter = true;
            if (outputInfo) {
                *outputInfo = info;
            }
            return true;
        }
        return false;
    }

    return false;
}
