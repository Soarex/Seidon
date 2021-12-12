#include <EntryPoint.h>
#include <Extensions/Extension.h>

namespace Seidon
{
	class Runtime : public Application
	{
	private:
		Entity selectedEntity;
		Extension e;
	public:
		void Init() override
		{
			window->SetName("Seidon Runtime");
			window->SetSize(1280, 720);
			window->EnableMouseCursor(false);
			window->EnableFullscreen(true);

			e.Bind(L"Assets/scripts.dll");
			e.Init();

			Scene* scene = new Scene();
			scene->LoadText("Assets/Test.sdscene");

			sceneManager->SetActiveScene(scene);
		}

		void Update() override
		{
		}

		void Destroy() override
		{
			e.Destroy();
		}
	};

	Application* CreateApplication()
	{
		return new Runtime();
	}
}