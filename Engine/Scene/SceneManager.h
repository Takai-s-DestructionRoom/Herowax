/*
* @file SceneManager.h
* @brief 各シーンとトランジションの制御, 非同期読み込みなど管理するクラス
*/

#pragma once
#include <memory>
#include <future>
#include <vector>
#include "IScene.h"
#include "ISceneTransition.h"
#include "Sprite.h"

class SceneManager
{
public:
	static SceneManager* GetInstance();
	static IScene* GetNowScene();

	static bool IsSceneChanging();
	static bool CheckGameEndFlag();

	static void Finalize();
	static void Update();
	static void Draw();

private:
	struct SceneChange {
		std::shared_ptr<std::future<std::shared_ptr<IScene>>> futureMakeScene = nullptr;
		std::shared_ptr<std::future<std::shared_ptr<IScene>>> futureMakeScene2 = nullptr;
		std::shared_ptr<std::future<bool>> futureSwapScene = nullptr;
		std::shared_ptr<std::future<bool>> futureTrashScene = nullptr;
		std::shared_ptr<ISceneTransition> transition = nullptr;
		std::shared_ptr<IScene> scene = nullptr;
		int32_t increment = 0;
	};

	std::mutex mMutex;
	std::shared_ptr<IScene> mNowScene = nullptr;
	std::shared_ptr<IScene> mChangeScene = nullptr;
	std::vector<SceneChange> mRunningSceneChanges;
	bool mFlagGameEnd = false;

	float mLoadingTimer = 0;
	float mLoadingAngle = 0;
	float mLoadingAlpha = 0;
	Sprite mLoadingMark;

	SceneManager();
	~SceneManager();
	SceneManager(const SceneManager& a) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

public:
	template<class SceneClassName, class SceneTransitionClassName = ISceneTransition>
	static void Change() {
		SceneManager* instance = GetInstance();

		SceneChange sc{};
		sc.futureMakeScene = std::make_shared<std::future<std::shared_ptr<IScene>>>(std::async(std::launch::deferred, [] {
			std::shared_ptr<IScene> newScene = std::make_shared<SceneClassName>();
			return newScene;
		}));

		sc.transition = std::make_shared<SceneTransitionClassName>();
		instance->mRunningSceneChanges.push_back(sc);
	}

	template<typename SceneClassName>
	static void Set() {
		SceneManager* instance = GetInstance();
		instance->mRunningSceneChanges.clear();
		std::unique_ptr<IScene> newScene = std::make_unique<SceneClassName>();
		instance->mNowScene = std::move(newScene);
		instance->mNowScene->Init();
	}
};
