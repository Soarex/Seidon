#pragma once
#include "Core/UUID.h"
#include "Graphics/Mesh.h"
#include "Graphics/Armature.h"
#include "Graphics/Material.h"
#include "Graphics/HdrCubemap.h"

#include "Animation/Animation.h"

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
			return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
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

	struct CubemapComponent
	{
		HdrCubemap* cubemap;

		CubemapComponent();
		CubemapComponent(const CubemapComponent&) = default;

		CubemapComponent(HdrCubemap* cubemap)
			:cubemap(cubemap) {}
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
			return glm::lookAt(transform.position, transform.position + transform.GetForwardDirection(), transform.GetUpDirection());
		}

		glm::mat4 GetProjectionMatrix()
		{
			return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
		}
	};

	struct AnimationComponent
	{
		Animation* animation;

		float runtimeTime = 0;
		std::vector<glm::mat4> runtimeBoneMatrices;

		AnimationComponent();
		AnimationComponent(const AnimationComponent&) = default;
	};

	struct CubeColliderComponent
	{
		glm::vec3 halfExtents = { 0.5f, 0.5f, 0.5f };

		CubeColliderComponent() = default;
		CubeColliderComponent(const CubeColliderComponent&) = default;
	};


	struct RigidbodyComponent
	{
		float mass = 0;

		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent&) = default;
	};
}