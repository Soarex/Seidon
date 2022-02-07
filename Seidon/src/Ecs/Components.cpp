#include "Components.h"
#include "../Core/Application.h"

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

	void RenderComponent::SetMesh(Mesh* mesh)
	{
		this->mesh = mesh;
		int oldSize = materials.size();

		materials.resize(mesh->subMeshes.size());

		if (oldSize < mesh->subMeshes.size())
			for (int i = oldSize; i < mesh->subMeshes.size(); i++)
				materials[i] = Application::Get()->GetResourceManager()->GetMaterial("default_material");
	}

	CubemapComponent::CubemapComponent()
	{
		cubemap = Application::Get()->GetResourceManager()->GetCubemap("default_cubemap");
	}

	AnimationComponent::AnimationComponent()
	{
		animation = Application::Get()->GetResourceManager()->GetAnimation("default_animation");
	}
}