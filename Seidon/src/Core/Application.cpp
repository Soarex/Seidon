#include "Application.h"
#include "Ecs/Components.h"
#include "../Graphics/RenderSystem.h"
#include "../Physics/PhysicSystem.h"
#include "../Utils/FlyingCameraControlSystem.h"

namespace Seidon
{
	Application* Application::instance;

	Application::Application()
	{

	}

	void Application::AppInit()
	{
		instance = this;

		window = new Window();
		sceneManager = new SceneManager();
		inputManager = new InputManager();
		resourceManager = new ResourceManager();
		workManager = new WorkManager();

		window->Init("Seidon Game");
		inputManager->Init(window);
		resourceManager->Init();
		workManager->Init();

		RegisterSystem<RenderSystem>();

		//RegisterSystem<FlyingCameraControlSystem>()
		//	.AddMember("Mouse Sensitivity", &FlyingCameraControlSystem::mouseSensitivity)
		//	.AddMember("Movement Speed", &FlyingCameraControlSystem::speed);

		RegisterSystem<PhysicSystem>();

		RegisterComponent<IDComponent>()
			.AddMember("ID", &IDComponent::ID);

		RegisterComponent<NameComponent>()
			.AddMember("Name", &NameComponent::name);
		
		RegisterComponent<TransformComponent>()
			.AddMember("Position", &TransformComponent::position)
			.AddMember("Rotation", &TransformComponent::rotation, Types::VECTOR3_ANGLES)
			.AddMember("Scale", &TransformComponent::scale);

		RegisterComponent<RenderComponent>()
			.AddMember("Mesh", &RenderComponent::mesh)
			.AddMember("Materials", &RenderComponent::materials);

		RegisterComponent<CubemapComponent>()
			.AddMember("Cubemap", &CubemapComponent::cubemap);

		RegisterComponent<DirectionalLightComponent>()
			.AddMember("Color", &DirectionalLightComponent::color, Types::VECTOR3_COLOR)
			.AddMember("Intensity", &DirectionalLightComponent::intensity);

		RegisterComponent<CameraComponent>()
			.AddMember("Perspective", &CameraComponent::perspective)
			.AddMember("Fov", &CameraComponent::fov)
			.AddMember("Aspect Ratio", &CameraComponent::aspectRatio)
			.AddMember("Near Plane", &CameraComponent::nearPlane)
			.AddMember("Far Plane", &CameraComponent::farPlane)
			.AddMember("Exposure", &CameraComponent::exposure);

		RegisterComponent<CubeColliderComponent>()
			.AddMember("Half Extents", &CubeColliderComponent::halfExtents);

		RegisterComponent<RigidbodyComponent>()
			.AddMember("Mass", &RigidbodyComponent::mass);

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