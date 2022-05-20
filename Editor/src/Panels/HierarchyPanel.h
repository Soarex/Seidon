#pragma once
#include <Seidon.h>
#include "../SelectedItem.h"

namespace Seidon
{
	class HierarchyPanel
	{
	private:
		SelectedItem& selectedItem;

		std::vector<std::function<void(Entity&)>> onEntitySelectionCallbacks;

	public:
		HierarchyPanel(SelectedItem& selectedItem) : selectedItem(selectedItem) {}

		void Init();
		void Draw();
		void AddSelectionCallback(const std::function<void(Entity&)>& callback);
		void SetSelectedEntity(Entity& entity);

	private:
		void DrawEntityNode(Entity& entity);
		void DrawBoneNode(Armature& armature, std::vector<glm::mat4>& boneTransforms, int index);
	};
}