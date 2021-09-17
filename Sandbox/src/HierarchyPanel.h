#pragma once
#include <Seidon.h>

class HierarchyPanel
{
public:
	HierarchyPanel();

	void OnImGuiRender();

	Seidon::Entity GetSelectedEntity() const { return selectedEntity; }
	void SetSelectedEntity(Seidon::Entity entity);

private:
	void DrawEntityNode(Seidon::Entity entity);
	void DrawComponents(Seidon::Entity entity);

private:
	Seidon::Entity selectedEntity;
};
