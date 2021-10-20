#include "Application.h"
#include "Ecs/Components.h"

namespace Seidon
{
	Application* Application::instance;

	Application::Application()
	{

	}

	void Application::AppInit()
	{
		instance = this;
		ecsContext = EcsContext::Instance();

		window = new Window();
		sceneManager = new SceneManager();
		inputManager = new InputManager();
		resourceManager = new ResourceManager();
		workManager = new WorkManager();

		window->Init("Seidon Game");
		inputManager->Init(window);
		resourceManager->Init();
		workManager->Init();

		RegisterComponent<NameComponent>();
		RegisterComponent<TransformComponent>();
		RegisterComponent<RenderComponent>();
		RegisterComponent<CubemapComponent>();
		RegisterComponent<DirectionalLightComponent>();
		RegisterComponent<CameraComponent>();
		RegisterComponent<CubeColliderComponent>();
		RegisterComponent<RigidbodyComponent>();

		Init();
	}

	void Application::AppUpdate()
	{
		window->BeginFrame();
		inputManager->Update();

		Update();
		sceneManager->UpdateActiveScene(window->GetDeltaTime());
		
		workManager->Update();
		window->EndFrame();
	}

	void Application::AppDestroy()
	{

		Destroy();
		window->Destroy();
		resourceManager->Destroy();
		workManager->Destroy();

		delete window;
		delete resourceManager;
		delete workManager;
		delete inputManager;
		instance = nullptr;
	}

	void Application::Init()
	{

	}

	void Application::Update()
	{

	}

	void Application::Destroy()
	{

	}

	Application::~Application()
	{

	}
}