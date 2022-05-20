#pragma once
#include <Seidon.h>
#include "../SelectedItem.h"
#include "Utils/DrawFunctions.h"

namespace Seidon
{
	class InspectorPanel
	{
	private:
		SelectedItem& selectedItem;
	public:
		InspectorPanel(SelectedItem& selectedItem) : selectedItem(selectedItem) {}

		void Init();
		void Draw();

	private:
		void DrawEntity(Entity e);
		void DrawMaterial(Material* m);
		void DrawBone(BoneSelectionData& boneData);
	};
}