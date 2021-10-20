#pragma once

namespace Seidon
{
	class Scene;
	class Window;
	class ResourceManager;
	class InputManager;
	class ResourceManager;
	class WorkManager;
	class SceneManager;

	class System
	{
	protected:
		Scene* scene;
		Window* window;
		InputManager* inputManager;
		ResourceManager* resourceManager;
		WorkManager* workManager;
		SceneManager* sceneManager;

	public:
		virtual void Init() {};
		virtual void Update(float deltaTime) {};
		virtual void Destroy() {};

	private:
		void SysInit();

	public:
		friend class Scene;
	};
}