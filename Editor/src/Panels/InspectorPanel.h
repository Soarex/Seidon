#pragma once
#include <Seidon.h>
#include "Utils/DrawFunctions.h"

namespace Seidon
{
	typedef std::function<void(Entity&)> ComponentDrawFunction;

	class InspectorPanel
	{
	private:
		Entity selectedEntity;
		std::map<std::string, ComponentDrawFunction> componentDrawFunctions;

	public:
		InspectorPanel();

		void Draw();
		void SetSelectedEntity(Entity& entity);
		void DrawComponents();

		template <typename T>
		void AddDrawFunction(const ComponentDrawFunction& function)
		{
			componentDrawFunctions[typeid(T).name()] = function;
		}

	private:
		template <typename T>
		void DrawComponent(Entity& entity)
		{
			componentDrawFunctions[typeid(T).name()](entity);
		}
	};
}