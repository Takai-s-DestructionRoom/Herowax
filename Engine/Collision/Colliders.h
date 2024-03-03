/*
* @file Colliders.h
* @brief コライダー管理クラス
*/

#pragma once
#include "ICollider.h"
#include <mutex>
#include <map>
#include <memory>
#include <list>
#include <Windows.h>

template <typename ColliderType>
class Collider;

// 各種コライダー一括管理クラス

class Colliders final {
	template <typename ColliderType>
	friend class Collider;
public:
	template <typename ColliderType, typename ...Args>
	static Collider<ColliderType> Create(Args ...args) {
		std::lock_guard<std::recursive_mutex> lock(sInstance.mMutex);

		sInstance.mColList.emplace_back(std::make_unique<ColliderType>(args...));
		return Collider<ColliderType>(static_cast<ColliderType*>(sInstance.mColList.back().get()));
	}

	static void Update();

	//全コライダーをアクティブにする
	static void AllActivate();
	//全コライダーを非アクティブにする
	static void AllInactivate();

	//Collidersクラスが管理する各コライダーのリストへのアクセス用クラス
	//外部からはこのクラスを介してのみアクセスできる
	class ColListAccessor {
		friend class Colliders;
	public:
		std::unique_lock<std::recursive_mutex> mLock;
		std::list<std::unique_ptr<ICollider>>* mList;

		ColListAccessor() : mLock(sInstance.mMutex), mList(&sInstance.mColList) {}
	};

private:
	static Colliders sInstance;

	Colliders() {};
	~Colliders() {}
	Colliders(const Colliders& a) = delete;
	Colliders& operator=(const Colliders&) = delete;
	Colliders(const Colliders&& a) = delete;
	Colliders& operator=(const Colliders&&) = delete;

	std::recursive_mutex mMutex;
	std::list<std::unique_ptr<ICollider>> mColList;
};

/*
	Collidersが管理する各種コライダー実体を参照する、
	スマートポインタもどきなクラス。
	"Collider<SphereCollider>"みたいな感じで使う。
	内部カウントを持ち、このクラスの生成で+1, 破棄で-1して、
	0になったらコライダー実体も破棄する。
	(Collidersインスタンスより寿命の長いColliderインスタンスがいると危ないのは内緒)
	(一応大丈夫なはずだけど)
*/

template <typename ColliderType>
class Collider final {
	friend class Colliders;
public:
	//コンストラクタ
	Collider() {}

private: //Collidersを介さないと新規生成はできないように(コピーとかはええで)
	ColliderType* mData = nullptr;

	//コンストラクタ
	Collider(ColliderType* data) : mData(data) {
		AddCount(data);
	}

public:
	//デストラクタ
	~Collider() {
		if (mData) {
			SubtractCount(mData);
			if (GetCount(mData) == 0) {
				//全参照消滅、削除する処理をここに
				Colliders::sInstance.mColList.remove_if(
					[&](std::unique_ptr<ICollider>& o) {
						return o.get() == mData;
					});
			}
		}
	}
	//コピーコンストラクタ
	Collider(const Collider& o) {
		if (this != &o) {
			if (mData) {
				SubtractCount(mData);
				if (GetCount(mData) == 0) {
					//全参照消滅、削除する処理をここに
					Colliders::sInstance.mColList.remove_if(
						[&](std::unique_ptr<ICollider>& o) {
							return o.get() == mData;
						});
				}
			}
		}
		mData = o.mData;
		AddCount(mData);
	}
	//コピー代入演算子
	Collider& operator=(const Collider& o) {
		if (this != &o) {
			if (mData) {
				SubtractCount(mData);
				if (GetCount(mData) == 0) {
					//全参照消滅、削除する処理をここに
					Colliders::sInstance.mColList.remove_if(
						[&](std::unique_ptr<ICollider>& o) {
							return o.get() == mData;
						});
				}
			}
			mData = o.mData;
			AddCount(mData);
		}
		return *this;
	}
	//ムーブコンストラクタ
	Collider(Collider&& o) noexcept {
		if (this != &o) {
			if (mData) {
				SubtractCount(mData);
				if (GetCount(mData) == 0) {
					//全参照消滅、削除する処理をここに
					Colliders::sInstance.mColList.remove_if(
						[&](std::unique_ptr<ICollider>& o) {
							return o.get() == mData;
						});
				}
			}
			mData = o.mData;
			o.mData = nullptr;
		}
	}
	//ムーブ代入演算子
	Collider& operator=(Collider&& o) noexcept {
		if (this != &o) {
			if (mData) {
				SubtractCount(mData);
				if (GetCount(mData) == 0) {
					//全参照消滅、削除する処理をここに
					Colliders::sInstance.mColList.remove_if(
						[&](std::unique_ptr<ICollider>& o) {
							return o.get() == mData;
						});
				}
			}
			mData = o.mData;
			o.mData = nullptr;
		}
		return *this;
	}

	operator bool() const {
		return mData != nullptr;
	}

	ColliderType* operator->() {
		return mData;
	}

private:
	static std::recursive_mutex mMutex;
	static std::map<void*, size_t> mCountMap;

	static void AddCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(mMutex);
		mCountMap[p]++;
	}

	static void SubtractCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(mMutex);
		mCountMap[p]--;

		if (mCountMap[p] == 0) {
			mCountMap.erase(p);
		}
	}

	static size_t GetCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(mMutex);
		auto itr = mCountMap.find(p);
		if (itr == mCountMap.end()) {
			return 0;
		}
		return mCountMap[p];
	}
};

template<typename ColliderType>
std::map<void*, size_t> Collider<ColliderType>::mCountMap = std::map<void*, size_t>();

template<typename ColliderType>
std::recursive_mutex Collider<ColliderType>::mMutex;
