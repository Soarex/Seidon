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
		mesh = Application::Get()->GetResourceManager()->GetSkinnedMesh("empty_skinned_mesh");

		Revalidate(this);
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

	void SkinnedRenderComponent::SetMesh(SkinnedMesh* mesh)
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

	void SkinnedRenderComponent::Revalidate(void* component)
	{
		SkinnedRenderComponent& rc = *(SkinnedRenderComponent*)component;
		int oldSize = rc.materials.size();

		if (oldSize < rc.mesh->subMeshes.size())
		{
			rc.materials.resize(rc.mesh->subMeshes.size());
			for (int i = oldSize; i < rc.mesh->subMeshes.size(); i++)
				rc.materials[i] = Application::Get()->GetResourceManager()->GetMaterial("default_material");
		}

		oldSize = rc.boneTransforms.size();

		if (oldSize < rc.mesh->armature.bones.size())
		{
			rc.boneTransforms.resize(rc.mesh->armature.bones.size());
			rc.worldSpaceBoneTransforms.resize(rc.mesh->armature.bones.size());
		}

		if(rc.boneTransforms.size() > 0)
			rc.boneTransforms[0] = glm::inverse(rc.mesh->armature.bones[0].inverseBindPoseMatrix);

		for (int i = 1; i < rc.mesh->armature.bones.size(); i++)
		{
			int parentId = rc.mesh->armature.bones[i].parentId;
			rc.boneTransforms[i] = rc.mesh->armature.bones[parentId].inverseBindPoseMatrix * glm::inverse(rc.mesh->armature.bones[i].inverseBindPoseMatrix);

		}
	}

	SpriteRenderComponent::SpriteRenderComponent()
	{
		material = Application::Get()->GetResourceManager()->GetMaterial("default_material");
	}

	TextRenderComponent::TextRenderComponent()
	{
		text = "";
		font = Application::Get()->GetResourceManager()->GetFont("empty_font");
		color = glm::vec3(0);
	}

	CubemapComponent::CubemapComponent()
	{
		cubemap = Application::Get()->GetResourceManager()->GetCubemap("default_cubemap");
	}

	ProceduralSkylightComponent::ProceduralSkylightComponent()
	{
		material = Application::Get()->GetResourceManager()->GetMaterial("Preetham Sky Material");
	}

	AnimationComponent::AnimationComponent()
	{
		animation = Application::Get()->GetResourceManager()->GetAnimation("default_animation");

		for (int i = 0; i < MAX_BONE_COUNT; i++)
		{
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
		if (!runtimeController.IsInitialized()) return;

		physx::PxController& controller = *runtimeController.GetInternalController();

		physx::PxVec3 v = { velocity.x, velocity.y, velocity.z };

		physx::PxControllerCollisionFlags collisionFlags = controller.move(v * deltaTime, minMoveDistance, deltaTime, physx::PxControllerFilters());
		isGrounded = collisionFlags.isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN);

		physx::PxExtendedVec3 p = controller.getPosition();

		transform.position = { p.x, p.y, p.z };
	}
}