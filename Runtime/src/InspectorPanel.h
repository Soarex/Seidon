#pragma once
#include <Seidon.h>
#include "Utils/DrawFunctions.h"

namespace Seidon
{
	class InspectorPanel
	{
	private:
		Entity selectedEntity;
	public:
		InspectorPanel() = default;

		void Init();
		void Draw();
		void SetSelectedEntity(Entity& entity);
		void DrawComponents();
	};
}