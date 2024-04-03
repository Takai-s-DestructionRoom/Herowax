#include "SceneManager.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include "Util.h"
#include "RWindow.h"
#include "TimeManager.h"

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

IScene* SceneManager::GetNowScene() {
	return GetInstance()->mNowScene.get();
}

bool SceneManager::IsSceneChanging() {
	return !GetInstance()->mRunningSceneChanges.empty();
}

bool SceneManager::CheckGameEndFlag()
{
	return GetInstance()->mFlagGameEnd;
}

void SceneManager::Finalize()
{
	GetInstance()->mFlagGameEnd = true;
	GetInstance()->mRunningSceneChanges.clear();
}

SceneManager::SceneManager() {
	mLoadingMark = Sprite(TextureManager::Load("Resources/Internals/loadingMark.png", "LoadingMark"));
}

SceneManager::~SceneManager()
{
	mChangeScene = nullptr;

	if (mNowScene != nullptr) {
		mNowScene->Finalize();
		mNowScene = nullptr;
	}
}

void SceneManager::Update() {
	SceneManager* instance = GetInstance();

	bool cancel = false;

	if (!instance->mRunningSceneChanges.empty()) {
		SceneChange& sc = *instance->mRunningSceneChanges.begin();

		if (sc.increment == 0) {
			sc.transition->Close();
			sc.increment++;
		}
		sc.transition->Update();

		if (sc.increment == 1
			&& sc.transition->IsClosed()
			&& sc.futureMakeScene->valid()) {
			sc.futureMakeScene2 = std::make_shared<std::future<std::shared_ptr<IScene>>>(std::async(std::launch::async, [&sc] {
				return sc.futureMakeScene->get();
			}));
			sc.increment++;
		}

		if (sc.increment == 2
				&& sc.futureMakeScene2->valid()
				&& sc.futureMakeScene2->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			sc.scene = sc.futureMakeScene2->get();
			sc.futureSwapScene = std::make_shared<std::future<bool>>(std::async(std::launch::async, [&sc] {
				std::lock_guard<std::recursive_mutex> lockSR(SRBufferAllocator::sMutex);
				SceneManager* instance = GetInstance();
				std::lock_guard<std::mutex> lock(instance->mMutex);
				std::shared_ptr<IScene> scene = std::move(sc.scene);
				std::swap(instance->mChangeScene, scene);
				return true;
			}));
			sc.increment++;
		}

		if (sc.increment == 3
			&& sc.futureSwapScene->valid()
			&& sc.futureSwapScene->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			std::lock_guard<std::mutex> lock(instance->mMutex);
			
			instance->mNowScene->Finalize();
			std::swap(instance->mNowScene, instance->mChangeScene);
			instance->mNowScene->Init();
			sc.futureTrashScene = std::make_shared<std::future<bool>>(std::async(std::launch::async, [&sc] {
				auto trash = std::make_shared<IScene>();
				SceneManager* instance = GetInstance();
				std::lock_guard<std::mutex> lock(instance->mMutex);
				std::swap(instance->mChangeScene, trash);
				return true;
			}));
			sc.increment++;
		}

		if (sc.increment == 4
			&& sc.futureTrashScene->valid()
			&& sc.futureTrashScene->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			instance->mLoadingTimer = 0;
			sc.transition->Open();
			sc.increment++;
		}

		if (sc.increment == 5
			&& sc.transition->IsOpened()) {
			instance->mRunningSceneChanges.erase(instance->mRunningSceneChanges.begin());
		}

		//ローディングマーク
		if ((sc.increment >= 1 && sc.increment <= 4) && sc.transition->IsClosed()) {
			instance->mLoadingTimer += TimeManager::deltaTime;
			if (instance->mLoadingTimer >= 0.5f) {
				float t = min(1, instance->mLoadingTimer - 0.5f / 0.5f);
				float fx = 1 - powf(1 - t, 5);

				instance->mLoadingAngle += Util::AngleToRadian(5);
				instance->mLoadingMark.mTransform.position = { RWindow::GetWidth() - 45.0f, RWindow::GetHeight() - 45.0f, 1145141919.0f };
				instance->mLoadingMark.mTransform.rotation = { 0, 0, instance->mLoadingAngle };
				instance->mLoadingMark.mTransform.UpdateMatrix();
				instance->mLoadingMark.mMaterial.mColor.a = 1.0f * fx;
				instance->mLoadingAlpha = 1.0f * fx;
			}
			else {
				instance->mLoadingAngle = 0;
				instance->mLoadingMark.mTransform.position = { RWindow::GetWidth() - 45.0f, RWindow::GetHeight() - 45.0f, 1145141919.0f };
				instance->mLoadingMark.mTransform.rotation = { 0, 0, 0 };
				instance->mLoadingMark.mTransform.UpdateMatrix();
				instance->mLoadingMark.mMaterial.mColor.a = 0;
			}
		}
		if (sc.increment == 5) {
			if (instance->mLoadingAngle != 0) {
				instance->mLoadingTimer += TimeManager::deltaTime;
				float t = min(1, instance->mLoadingTimer / 0.5f);
				float fx = 1 - powf(1 - t, 5);

				instance->mLoadingAngle += Util::AngleToRadian(5);
				instance->mLoadingMark.mTransform.position = { RWindow::GetWidth() - 45.0f, RWindow::GetHeight() - 45.0f, 1145141919.0f };
				instance->mLoadingMark.mTransform.rotation = { 0, 0, instance->mLoadingAngle };
				instance->mLoadingMark.mTransform.UpdateMatrix();
				instance->mLoadingMark.mMaterial.mColor.a = instance->mLoadingAlpha * (1 - fx);
			}
		}

		if (sc.increment >= 2 && sc.increment <= 4) cancel = true;
	}
	else {
		instance->mLoadingTimer = 0;
		instance->mLoadingAngle = 0;
		instance->mLoadingAlpha = 0;
	}

	if (!cancel && instance->mNowScene != nullptr) {
		instance->mNowScene->Update();
	}
}

void SceneManager::Draw() {
	SceneManager* instance = GetInstance();
	
	bool cancel = false;

	if (!instance->mRunningSceneChanges.empty()) {
		SceneChange& sc = *instance->mRunningSceneChanges.begin();

		if (sc.increment >= 2 && sc.increment <= 4) cancel = true;
		sc.transition->Draw();

		if (instance->mLoadingAlpha != 0 && ((sc.increment == 1 && sc.transition->IsClosed()) || (sc.increment >= 2 && sc.increment <= 4))) {
			instance->mLoadingMark.TransferBuffer();
			instance->mLoadingMark.Draw();
		}
	}

	if (!cancel && instance->mNowScene != nullptr) {
		instance->mNowScene->Draw();
	}
}
