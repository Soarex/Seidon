#include "System.h"
#include "../Core/Application.h"
#include "../Debug/Debug.h"

namespace Seidon
{
	System::~System()
	{

	}

	void System::SysInit()
	{
		SD_ASSERT(!initialized, "System already initialized");

		Application* app = Application::Get();
		inputManager = app->GetInputManager();
		resourceManager = app->GetResourceManager();
		sceneManager = app->GetSceneManager();
		workManager = app->GetWorkManager();
		window = app->GetWindow();

		Init();

		initialized = true;
	}

	void System::SysUpdate(float deltaTime)
	{
		SD_ASSERT(initialized, "System not initialized");

		Update(deltaTime);
	}

	void System::SysDestroy()
	{
		SD_ASSERT(initialized, "System not initialized");

		Destroy();
		initialized = false;
	}
}