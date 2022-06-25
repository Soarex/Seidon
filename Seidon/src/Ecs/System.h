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
	public:
		virtual ~System();
		virtual void Init() {};
		virtual void Update(float deltaTime) {};
		virtual void Destroy() {};

	protected:
		bool initialized = false;

		Scene* scene;
		Window* window;
		InputManager* inputManager;
		ResourceManager* resourceManager;
		WorkManager* workManager;
		SceneManager* sceneManager;

	private:
		void SysInit();
		void SysUpdate(float deltaTime);
		void SysDestroy();
	public:
		friend class Scene;

		//TODO: Engine shouldn't know of Editor existence; 
		friend class Project;
	};
}