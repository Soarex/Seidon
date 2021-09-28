#pragma once
#include "Application.h"
#include "Core/Window.h"
#include "Core/InputManager.h"
#include "Core/WorkManager.h"
#include "Graphics/RenderSystem.h"

extern Seidon::Application* Seidon::CreateApplication();

int main(int argc, char** argv) 
{
	float deltaTime = 0;

	Seidon::Window::Init("Seidon", 800, 600);
	Seidon::ResourceManager::Init();
	Seidon::InputManager::Init();
	Seidon::WorkManager::Init();
	
	Seidon::Application* app = Seidon::CreateApplication();

	while (!Seidon::Window::ShouldClose())
	{
		Seidon::Window::BeginFrame();

		Seidon::InputManager::Update();
		Seidon::SceneManager::UpdateActiveScene(deltaTime);
		Seidon::WorkManager::Update();
		app->Run();

		deltaTime = Seidon::Window::EndFrame();
	}

	delete app;
	Seidon::ResourceManager::Destroy();
	Seidon::WorkManager::Destroy();
	Seidon::Window::Destroy();
	return 0;
}