#pragma once
#include "Panel.h"

#include <Seidon.h>
#include <ImGuizmo/ImGuizmo.h>

namespace Seidon
{
	class ViewportPanel : public Panel
	{
	public:
		ViewportPanel(Editor& editor) : Panel(editor) {}

		void Init() override;
		void Draw() override;
		void Destroy() override;

	private:
		BoundingBox viewportBounds;

		bool colliderRenderingEnabled = false;

		int guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		bool local = false;
	private:
		void ProcessInput();

		void DrawTransformGuizmos();
		void DrawCubeColliders(Renderer& renderer);
		void DrawMeshColliders(Renderer& renderer);
		void DrawCharacterControllers(Renderer& renderer);
	};
}