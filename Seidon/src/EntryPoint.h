#pragma once
#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/InputManager.h"
#include "Core/WorkManager.h"
#include "Graphics/RenderSystem.h"
#include "Ecs/Components.h"

extern Seidon::Application* Seidon::CreateApplication();
/*
template<typename Type>
struct entt::type_seq<Type> 
{
	[[nodiscard]] static id_type value() ENTT_NOEXCEPT 
	{
		static const entt::id_type value = EcsContext::Instance()->value(entt::type_hash<Type>::value());
		return value;
	}

	[[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};
*/
int main(int argc, char** argv) 
{
	Seidon::Application* app = Seidon::CreateApplication();
	app->AppInit();

	while (!app->GetWindow()->ShouldClose())
		app->AppUpdate();

	app->AppDestroy();
	delete app;
	return 0;
}