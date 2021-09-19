#pragma once
#include <Seidon.h>

namespace Seidon
{
	class HierarchyPanel
	{
	private:
		Entity selectedEntity;
		std::vector<std::function<void(Entity&)>> onEntitySelectionCallbacks;

	public:
		HierarchyPanel();

		void Draw();
		void AddSelectionCallback(const std::function<void(Entity&)>& callback);
		void SetSelectedEntity(Entity& entity);

	private:
		void DrawEntityNode(Entity& entity);
	};
}