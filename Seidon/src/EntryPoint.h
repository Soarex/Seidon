#pragma once
#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/InputManager.h"
#include "Core/WorkManager.h"
#include "Graphics/RenderSystem.h"
#include "Ecs/Components.h"

extern Seidon::Application* Seidon::CreateApplication();

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