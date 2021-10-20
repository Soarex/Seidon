#include "System.h"
#include "../Core/Application.h"

namespace Seidon
{
	void System::SysInit()
	{
		Application* app = Application::Get();
		inputManager = app->GetInputManager();
		resourceManager = app->GetResourceManager();
		sceneManager = app->GetSceneManager();
		workManager = app->GetWorkManager();
		window = app->GetWindow();

		Init();
	}
}