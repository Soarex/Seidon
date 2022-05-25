#pragma once
#include <Seidon.h>
#include <filesystem>
#include <Imgui/imgui_internal.h>

namespace Seidon
{
	enum class ChangeStatus
	{
		NO_CHANGE = 0,
		CHANGING,
		CHANGED
	};

	struct ChangeData
	{
		ChangeStatus status;
		MemberData modifiedMember;
		byte oldValue[50];
		byte newValue[50];
	};

	ChangeStatus DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	ChangeStatus DrawColorControl(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	ChangeStatus DrawIdControl(const std::string& label, UUID& value, float columnWidth = 100.0f);
	ChangeStatus DrawFloatControl(const std::string& label, float& value, float columnWidth = 100.0f);
	ChangeStatus DrawFloatSliderControl(const std::string& label, float& value, float min = 0, float max = 1, float columnWidth = 100.0f);
	ChangeStatus DrawBoolControl(const std::string& label, bool& value, float columnWidth = 100.0f);
	ChangeStatus DrawStringControl(const std::string& label, std::string& value, float columnWidth = 100.0f);
	ChangeStatus DrawTextureControl(const std::string& label, Texture** texture, float size = 64);
	ChangeStatus DrawCubemapControl(const std::string& label, HdrCubemap** texture, float size = 64);
	ChangeStatus DrawMeshControl(const std::string& label, Mesh** mesh, float size = 64);
	ChangeStatus DrawSkinnedMeshControl(const std::string& label, SkinnedMesh** mesh, float size = 64);
	ChangeStatus DrawMaterialControl(const std::string& label, Material** material, float size = 64, bool* clicked = nullptr);
	ChangeStatus DrawMaterialEditor(const std::string& label, Material* material, bool* open = 0);
	ChangeStatus DrawAnimationControl(const std::string& label, Animation** animation, float size = 64);
	ChangeStatus DrawShaderControl(const std::string& label, Shader** shader, float size = 64);

	ChangeStatus DrawReflectedMember(void* object, MemberData& member);
	ChangeData DrawMetaType(void* object, MetaType& type);
}