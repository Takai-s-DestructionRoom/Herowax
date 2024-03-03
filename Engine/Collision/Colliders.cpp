#include "Colliders.h"
#include <Util.h>
#include <vector>

Colliders Colliders::sInstance;

void Colliders::Update()
{
    std::lock_guard<std::recursive_mutex> lock(sInstance.mMutex);

    //二重判定防止用なのです
    std::map <ICollider*, std::vector<ICollider*>> hitted;

    //2回回すの嫌だから何かいい方法考えたい所
    for (std::unique_ptr<ICollider>& ptr : sInstance.mColList) {
        ptr->mHasCollision = false;
    }

    //総当たりチェック！！！！！
    for (std::unique_ptr<ICollider>& ptr : sInstance.mColList) {
        if (!ptr->mIsActive) continue;
        ptr->Update();
        for (std::unique_ptr<ICollider>& ptr2 : sInstance.mColList) {
            if (ptr == ptr2) continue;
            if (!ptr2->mIsActive) continue;
            if (Util::IndexOf(hitted[ptr.get()], ptr2.get()) != -1) continue;

            CollisionInfo info;
            if (ptr->Collide(ptr2.get(), &info)) {
                hitted[ptr.get()].push_back(ptr2.get());
                hitted[ptr2.get()].push_back(ptr.get());

                ptr->mHasCollision = true;
                if(ptr->mFuncOnCollision) ptr->mFuncOnCollision(info);

                ptr2->mHasCollision = true;
                if (ptr2->mFuncOnCollision) {
                    CollisionInfo copy = info;
                    copy.hitCollider = ptr2.get();
                    ptr2->mFuncOnCollision(copy);
                }
            }
        }
    }
}

void Colliders::AllActivate()
{
    std::lock_guard<std::recursive_mutex> lock(sInstance.mMutex);
    for (std::unique_ptr<ICollider>& ptr : sInstance.mColList) {
        ptr->mIsActive = true;
    }
}

void Colliders::AllInactivate()
{
    std::lock_guard<std::recursive_mutex> lock(sInstance.mMutex);
    for (std::unique_ptr<ICollider>& ptr : sInstance.mColList) {
        ptr->mIsActive = false;
    }
}
