#pragma once
#include <Seidon.h>
#include "Panel.h"

namespace Seidon
{
	class HierarchyPanel : public Panel
	{
	public:
		HierarchyPanel(Editor& editor) : Panel(editor) {}

		void Init() override;
		void Draw() override;

	private:
		void DrawEntityNode(Entity& entity);
		void DrawBoneNode(Entity& owningEntity, Armature& armature, std::vector<glm::mat4>& boneTransforms, int index);
	};
}