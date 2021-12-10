#include <EntryPoint.h>
#include <Extensions/Extension.h>

#include "HierarchyPanel.h"
#include "InspectorPanel.h"

namespace Seidon
{
	class Runtime : public Application
	{
	private:
		HierarchyPanel p1;
		InspectorPanel p2;

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

			p1.Init();
			p2.Init();

			selectedEntity = { entt::null, nullptr };
			p1.AddSelectionCallback([&](Entity& entity)
				{
					selectedEntity = entity;
				});
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