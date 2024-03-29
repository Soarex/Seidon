#include "Application.h"
#include "Ecs/Components.h"
#include "../Graphics/RenderSystem.h"
#include "../Animation/AnimationSystem.h"
#include "../Physics/PhysicSystem.h"

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
		physicsApi = new PhysicsApi();
		soundApi = new SoundApi();

		window->Init("Seidon Game");
		inputManager->Init(window);
		physicsApi->Init();
		soundApi->Init();
		resourceManager->Init();
		workManager->Init();

		RegisterSystem<RenderSystem>();

		RegisterSystem<AnimationSystem>();
		RegisterSystem<PhysicSystem>();

		RegisterComponent<IDComponent>()
			.AddMember("ID", &IDComponent::ID);

		RegisterComponent<NameComponent>()
			.AddMember("Name", &NameComponent::name);

		RegisterComponent<MouseSelectionComponent>();

		RegisterComponent<TransformComponent>()
			.AddMember("Position", &TransformComponent::position)
			.AddMember("Rotation", &TransformComponent::rotation, Types::VECTOR3_ANGLES)
			.AddMember("Scale", &TransformComponent::scale)
			.AddMember("Parent", &TransformComponent::parent)
			.AddMember("Children", &TransformComponent::children);

		RegisterComponent<RenderComponent>()
			.AddMember("Mesh", &RenderComponent::mesh)
			.AddMember("Materials", &RenderComponent::materials)
			.OnChange = RenderComponent::Revalidate;

		RegisterComponent<SkinnedRenderComponent>()
			.AddMember("Mesh", &SkinnedRenderComponent::mesh)
			.AddMember("Materials", &SkinnedRenderComponent::materials)
			.OnChange = SkinnedRenderComponent::Revalidate;

		RegisterComponent<SpriteRenderComponent>()
			.AddMember("Sprite", &SpriteRenderComponent::sprite)
			.AddMember("Tint", &SpriteRenderComponent::tint);

		RegisterComponent<TextRenderComponent>()
			.AddMember("Text", &TextRenderComponent::text)
			.AddMember("Font", &TextRenderComponent::font)
			.AddMember("Color", &TextRenderComponent::color, Types::VECTOR3_COLOR)
			.AddMember("Shadow Distance", &TextRenderComponent::shadowDistance, Types::FLOAT_NORMALIZED)
			.AddMember("Shadow Color", &TextRenderComponent::shadowColor, Types::VECTOR3_COLOR);

		RegisterComponent<UISpriteComponent>()
			.AddMember("Sprite", &UISpriteComponent::sprite)
			.AddMember("Tint", &UISpriteComponent::tint)
			.AddMember("Id", &UISpriteComponent::id);

		RegisterComponent<UITextComponent>()
			.AddMember("Text", &UITextComponent::text)
			.AddMember("Font", &UITextComponent::font)
			.AddMember("Color", &UITextComponent::color, Types::VECTOR3_COLOR)
			.AddMember("Shadow Distance", &UITextComponent::shadowDistance, Types::FLOAT_NORMALIZED)
			.AddMember("Shadow Color", &UITextComponent::shadowColor, Types::VECTOR3_COLOR)
			.AddMember("Id", &UITextComponent::id);

		RegisterComponent<UIAnchorComponent>()
			.AddMember("Anchor point", &UIAnchorComponent::anchorPoint);

		RegisterComponent<WireframeRenderComponent>()
			.AddMember("Mesh", &WireframeRenderComponent::mesh)
			.AddMember("Color", &WireframeRenderComponent::color, Types::VECTOR3_COLOR);

		RegisterComponent<ProceduralSkylightComponent>()
			.AddMember("Material", &ProceduralSkylightComponent::material)
			.OnChange = ProceduralSkylightComponent::Invalidate;

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

		RegisterComponent<AnimationComponent>()
			.AddMember("Animation", &AnimationComponent::animation);

		RegisterComponent<CubeColliderComponent>()
			.AddMember("Offset", &CubeColliderComponent::offset)
			.AddMember("Half Extents", &CubeColliderComponent::halfExtents)
			.OnChange = CubeColliderComponent::Invalidate;

		RegisterComponent<MeshColliderComponent>()
			.AddMember("Mesh collider", &MeshColliderComponent::collider);

		RegisterComponent<DynamicRigidbodyComponent>()
			.AddMember("Mass", &DynamicRigidbodyComponent::mass)
			.AddMember("Kinematic", &DynamicRigidbodyComponent::kinematic)
			.AddMember("Lock X Rotation", &DynamicRigidbodyComponent::lockXRotation)
			.AddMember("Lock Y Rotation", &DynamicRigidbodyComponent::lockYRotation)
			.AddMember("Lock Z Rotation", &DynamicRigidbodyComponent::lockZRotation);

		RegisterComponent<StaticRigidbodyComponent>();

		RegisterComponent<CharacterControllerComponent>()
			.AddMember("Offset", &CharacterControllerComponent::offset)
			.AddMember("Collider Height", &CharacterControllerComponent::colliderHeight)
			.AddMember("Collider Radius", &CharacterControllerComponent::colliderRadius)
			.AddMember("Contact Offset", &CharacterControllerComponent::contactOffset)
			.AddMember("Min Movement Distance", &CharacterControllerComponent::minMoveDistance)
			.AddMember("Max Slope Angle", &CharacterControllerComponent::maxSlopeAngle);

		Init();
	}

	void Application::AppUpdate()
	{
		window->BeginFrame();
		inputManager->Update();
		soundApi->Update();

		Update();
		sceneManager->UpdateActiveScene(window->GetDeltaTime());
		
		workManager->Update();
		window->EndFrame();
	}

	void Application::AppDestroy()
	{
		Destroy();
		soundApi->Destroy();
		physicsApi->Destroy();
		resourceManager->Destroy();
		workManager->Destroy();
		window->Destroy();

		delete window;
		delete resourceManager;
		delete workManager;
		delete inputManager;
		delete physicsApi;
		delete soundApi;
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