#include "SceneManager.h"

namespace Seidon
{
	Scene* SceneManager::activeScene;

	Scene* SceneManager::GetActiveScene()
	{
		return activeScene;
	}

	void SceneManager::SetActiveScene(Scene* scene)
	{
		activeScene = scene;
		activeScene->Init();
	}

	void SceneManager::ChangeActiveScene(Scene* scene)
	{
		if (activeScene)
		{
			activeScene->Destroy();
			delete activeScene;
		}

		activeScene = scene;
		activeScene->Init();
	}

	void SceneManager::UpdateActiveScene(float deltaTime)
	{
		if(activeScene)
			activeScene->Update(deltaTime);
	}
}