#pragma once
#include <Seidon.h>
#include "../SelectedItem.h"

namespace Seidon
{
	class HierarchyPanel
	{
	public:
		HierarchyPanel(SelectedItem& selectedItem) : selectedItem(selectedItem) {}

		void Init();
		void Draw();

	private:
		SelectedItem& selectedItem;
	
	private:
		void DrawEntityNode(Entity& entity);
		void DrawBoneNode(Entity& owningEntity, Armature& armature, std::vector<glm::mat4>& boneTransforms, int index);
	};
}