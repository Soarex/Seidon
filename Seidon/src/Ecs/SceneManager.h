#pragma once
#include "Scene.h"

namespace Seidon
{
	class SceneManager
	{
	private:
		Scene* activeScene;

	public:
		Scene* GetActiveScene();
		void SetActiveScene(Scene* scene);
		void ChangeActiveScene(Scene* scene);

		void UpdateActiveScene(float deltaTime);
	};
}