#pragma once
#include "Scene.h"

namespace Seidon
{
	class SceneManager
	{
	private:
		static Scene* activeScene;

	public:
		static Scene* GetActiveScene();
		static void SetActiveScene(Scene* scene);
		static void ChangeActiveScene(Scene* scene);

		static void UpdateActiveScene(float deltaTime);
	};
}