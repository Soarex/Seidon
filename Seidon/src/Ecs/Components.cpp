#include "Components.h"
#include "../Core/Application.h"

#include <Physx/PxPhysics.h>

namespace Seidon
{
	RenderComponent::RenderComponent()
	{
		mesh = Application::Get()->GetResourceManager()->GetMesh("empty_mesh");
	}

	SkinnedRenderComponent::SkinnedRenderComponent()
	{
		mesh = Application::Get()->GetResourceManager()->GetMesh("empty_mesh");
		armature = Application::Get()->GetResourceManager()->GetArmature("default_armature");
	}

	WireframeRenderComponent::WireframeRenderComponent()
	{
		mesh = Application::Get()->GetResourceManager()->GetMesh("empty_mesh");
		color = glm::vec3(1);
	}

	void RenderComponent::SetMesh(Mesh* mesh)
	{
		this->mesh = mesh;

		Revalidate(this);
	}

	void RenderComponent::Revalidate(void* component)
	{
		RenderComponent& rc = *(RenderComponent*)component;
		int oldSize = rc.materials.size();

		if (oldSize < rc.mesh->subMeshes.size())
		{
			rc.materials.resize(rc.mesh->subMeshes.size());
			for (int i = oldSize; i < rc.mesh->subMeshes.size(); i++)
				rc.materials[i] = Application::Get()->GetResourceManager()->GetMaterial("default_material");
		}
	}

	CubemapComponent::CubemapComponent()
	{
		cubemap = Application::Get()->GetResourceManager()->GetCubemap("default_cubemap");
	}

	AnimationComponent::AnimationComponent()
	{
		animation = Application::Get()->GetResourceManager()->GetAnimation("default_animation");
		runtimeBoneMatrices.reserve(MAX_BONE_COUNT);

		for (int i = 0; i < MAX_BONE_COUNT; i++)
		{
			localBoneMatrices[i] = glm::identity<glm::mat4>();
			lastPositionKeyIndices[i] = 0;
			lastRotationKeyIndices[i] = 0;
			lastScalingKeyIndices[i] = 0;
		}
	}

	MeshColliderComponent::MeshColliderComponent()
	{
		mesh = Application::Get()->GetResourceManager()->GetMesh("empty_mesh");
	}

	void CharacterControllerComponent::Move(TransformComponent& transform, glm::vec3 velocity, float deltaTime)
	{
		if (!runtimeController) return;

		physx::PxController& controller = *(physx::PxController*)runtimeController;

		physx::PxVec3 v = { velocity.x, velocity.y, velocity.z };

		physx::PxControllerCollisionFlags collisionFlags = controller.move(v * deltaTime, 0.01, deltaTime, physx::PxControllerFilters());
		isGrounded = collisionFlags.isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN);

		physx::PxExtendedVec3 p = controller.getPosition();

		transform.position = { p.x, p.y, p.z };
	}
}