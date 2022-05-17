#pragma once
#include "Core/UUID.h"
#include "Graphics/Mesh.h"
#include "Graphics/Sprite.h"
#include "Graphics/Armature.h"
#include "Graphics/Material.h"
#include "Graphics/HdrCubemap.h"

#include "Animation/Animation.h"

#include "../Physics/CollisionData.h"
#include "../Physics/StaticActor.h"
#include "../Physics/DynamicActor.h"
#include "../Physics/CharacterController.h"
#include "../Physics/PhysicsShape.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Seidon
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(const UUID id)
			: ID(id) {}
	};

	enum class SelectionStatus
	{
		NONE = 0,
		HOVERED,
		CLICKED,
		HELD
	};

	struct MouseSelectionComponent
	{
		SelectionStatus status = SelectionStatus::NONE;

		MouseSelectionComponent() = default;
		MouseSelectionComponent(const MouseSelectionComponent&) = default;
	};

	struct NameComponent
	{
		std::string name;

		NameComponent() = default;
		NameComponent(const NameComponent&) = default;
		NameComponent(const std::string name)
			: name(name) {}
	};

	struct TransformComponent
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		UUID parent = 0;
		std::vector<UUID> children;

		glm::mat4 chachedWorldSpaceMatrix;
		bool cacheValid = false;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		TransformComponent(glm::vec3& position)
			: position(position) {}

		TransformComponent(const glm::mat4& transform)
		{
			SetFromMatrix(transform);
		}

		void SetFromMatrix(const glm::mat4& transform)
		{
			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(transform, scale, rotation, translation, skew, perspective);

			this->position = translation;
			this->rotation = glm::eulerAngles(rotation);
			this->scale = scale;
		}

		glm::mat4 GetTransformMatrix() const
		{
			glm::mat4 rot = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4(1.0f), position)
				* rot
				* glm::scale(glm::mat4(1.0f), scale);
		}

		glm::quat GetOrientation() const
		{
			return glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));
		}

		glm::vec3 GetUpDirection() const
		{
			return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		glm::vec3 GetRightDirection() const
		{
			return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
		}

		glm::vec3 GetForwardDirection() const
		{
			return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, 1.0f));
		}
	};

	struct RenderComponent
	{
		Mesh* mesh;
		std::vector<Material*> materials;

		RenderComponent();
		RenderComponent(const RenderComponent&) = default;

		RenderComponent(Mesh* mesh, const std::vector<Material*>& materials)
		{
			this->materials = materials;
			SetMesh(mesh);
		}

		void SetMesh(Mesh* mesh);

		static void Revalidate(void* component);
	};

	struct SkinnedRenderComponent : public RenderComponent
	{
		Armature* armature;

		SkinnedRenderComponent();
		SkinnedRenderComponent(const SkinnedRenderComponent&) = default;

		SkinnedRenderComponent(Mesh* mesh, const std::vector<Material*>& materials)
		{
			this->materials = materials;
			SetMesh(mesh);
		}

	};

	struct SpriteRenderComponent
	{
		Sprite* sprite;

		SpriteRenderComponent() = default;
		SpriteRenderComponent(const SpriteRenderComponent&) = default;

		SpriteRenderComponent(Sprite* sprite)
			: sprite(sprite) {}
	};

	struct WireframeRenderComponent
	{
		Mesh* mesh;
		glm::vec3 color;

		WireframeRenderComponent();
		WireframeRenderComponent(const WireframeRenderComponent&) = default;

		WireframeRenderComponent(Mesh* mesh, const glm::vec3& color = glm::vec3(1))
			: mesh(mesh), color(color) {}
	};

	struct CubemapComponent
	{
		HdrCubemap* cubemap;

		CubemapComponent();
		CubemapComponent(const CubemapComponent&) = default;

		CubemapComponent(HdrCubemap* cubemap)
			:cubemap(cubemap) {}
	};

	struct ProceduralSkylightComponent
	{
		Material* material;

		bool changed = false;
		HdrCubemap* cachedCubemap = nullptr;

		ProceduralSkylightComponent();
		ProceduralSkylightComponent(const ProceduralSkylightComponent&) = default;

		static void Invalidate(void* component)
		{
			((ProceduralSkylightComponent*)component)->changed = true;
		}
	};

	struct DirectionalLightComponent
	{
		glm::vec3 color;
		float intensity;

		DirectionalLightComponent(const DirectionalLightComponent&) = default;
		DirectionalLightComponent(glm::vec3 color = glm::vec3(1.0f), float intensity = 1.0f)
			:color(color), intensity(intensity) {}
	};

	struct CameraComponent
	{
		bool perspective = true;

		float fov = 45.0f;
		float aspectRatio = 16.0f / 9.0f;
		float nearPlane = 0.1f;
		float farPlane = 100.0f;
		float exposure = 1.0f;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(bool perspective, float fov = 45.0f, float aspectRatio = 16.0f/9.0f, float nearPlane = 0.1f, float farPlane = 100.0f, float exposure = 1.0f)
			:perspective(perspective), fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane), exposure(exposure) {}

		glm::mat4 GetViewMatrix(TransformComponent& transform)
		{
			return glm::lookAt(transform.position, transform.position - transform.GetForwardDirection(), transform.GetUpDirection());
		}

		glm::mat4 GetProjectionMatrix()
		{
			return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
		}
	};

	struct AnimationComponent
	{
		Animation* animation;

		//Runtime data
		float runtimeTime = 0;

		glm::mat4 localBoneMatrices[MAX_BONE_COUNT];

		float lastPositionKeyIndices[MAX_BONE_COUNT];
		float lastRotationKeyIndices[MAX_BONE_COUNT];
		float lastScalingKeyIndices[MAX_BONE_COUNT];

		std::vector<glm::mat4> runtimeBoneMatrices;

		AnimationComponent();
		AnimationComponent(const AnimationComponent&) = default;
	};

	struct CubeColliderComponent
	{
		glm::vec3 offset = glm::vec3(0);
		glm::vec3 halfExtents = { 0.5f, 0.5f, 0.5f };

		PhysicsShape shape;
		bool changed = false;

		CubeColliderComponent() = default;
		CubeColliderComponent(const CubeColliderComponent& other) = default;

		static void Invalidate(void* component)
		{
			((CubeColliderComponent*)component)->changed = true;
		}
	};

	struct MeshColliderComponent
	{
		Mesh* mesh;
		PhysicsShape shape;

		MeshColliderComponent();
		MeshColliderComponent(const MeshColliderComponent&) = default;
	};

	struct StaticRigidbodyComponent
	{
		StaticActor actor;

		StaticRigidbodyComponent() = default;
		StaticRigidbodyComponent(const StaticRigidbodyComponent&) = default;
	};

	struct DynamicRigidbodyComponent
	{
		float mass = 1;
		bool kinematic = false;

		bool lockXRotation = false;
		bool lockYRotation = false;
		bool lockZRotation = false;

		DynamicActor actor;

		DynamicRigidbodyComponent() = default;
		DynamicRigidbodyComponent(const DynamicRigidbodyComponent& other) = default;
	};

	struct CharacterControllerComponent
	{
		float colliderHeight = 1;
		float colliderRadius = 0.5f;
		float contactOffset = 0.01f;
		float minMoveDistance = 0.001f;
		float maxSlopeAngle = 0;

		CharacterController runtimeController;
		bool isGrounded = false;

		std::vector<CollisionData> collisions;

		CharacterControllerComponent() = default;
		CharacterControllerComponent(const CharacterControllerComponent& other) = default;

		void Move(TransformComponent& transform, glm::vec3 velocity, float deltaTime);
	};
}
