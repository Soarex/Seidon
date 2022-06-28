#include <EntryPoint.h>
#include <Extensions/Extension.h>

namespace Seidon
{
	class Runtime : public Application
	{
	private:
		Entity selectedEntity;
	public:
		void Init() override
		{
			window->SetName("Seidon Runtime");
			window->SetSize(1280, 720);
			window->EnableMouseCursor(false);
			window->EnableFullscreen(true);

			std::ifstream in("Assets/ResourceRegistry.sdreg", std::ios::in | std::ios::binary);
			resourceManager->Load(in);

			Scene* scene = new Scene();

			std::ifstream in2("Assets/Test.sdscene", std::ios::in | std::ios::binary);
			scene->Load(in2);

			sceneManager->SetActiveScene(scene);
		}

		void Update() override
		{
		}

		void Destroy() override
		{
		}
	};

	Application* CreateApplication()
	{
		return new Runtime();
	}
}