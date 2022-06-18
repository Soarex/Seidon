#pragma once
#include "Panel.h"
#include "../SelectedItem.h"
#include "Utils/DrawFunctions.h"

#include <Seidon.h>

namespace Seidon
{
	class InspectorPanel : public Panel
	{
	public:
		InspectorPanel(Editor& editor) : Panel(editor) {}

		void Init() override;
		void Draw() override;

	private:
		void DrawEntity(Entity e);
		void DrawMaterial(Material* m);
		void DrawBone(BoneSelectionData& boneData);
	};
}