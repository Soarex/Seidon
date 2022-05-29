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

	ChangeStatus DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, glm::vec3* oldValue = nullptr);
	ChangeStatus DrawColorControl(const std::string& label, glm::vec3& values, float resetValue = 0.0f, glm::vec3* oldValue = nullptr);
	ChangeStatus DrawIdControl(const std::string& label, UUID& value, UUID* oldValue = nullptr);
	ChangeStatus DrawFloatControl(const std::string& label, float& value, float* oldValue = nullptr);
	ChangeStatus DrawFloatSliderControl(const std::string& label, float& value, float min = 0, float max = 1, float* oldValue = nullptr);
	ChangeStatus DrawBoolControl(const std::string& label, bool& value, bool* oldValue = nullptr);
	ChangeStatus DrawStringControl(const std::string& label, std::string& value, std::string* oldValue = nullptr);
	ChangeStatus DrawTextureControl(const std::string& label, Texture** texture, float size = 64, Texture** oldValue = nullptr);
	ChangeStatus DrawCubemapControl(const std::string& label, HdrCubemap** texture, float size = 64, HdrCubemap** oldValue = nullptr);
	ChangeStatus DrawMeshControl(const std::string& label, Mesh** mesh, float size = 64, Mesh** oldValue = nullptr);
	ChangeStatus DrawSkinnedMeshControl(const std::string& label, SkinnedMesh** mesh, float size = 64, SkinnedMesh** oldValue = nullptr);
	ChangeStatus DrawMaterialControl(const std::string& label, Material** material, float size = 64, bool* clicked = nullptr, Material** oldValue = nullptr);
	ChangeStatus DrawMaterialEditor(const std::string& label, Material* material, bool* open = 0);
	ChangeStatus DrawAnimationControl(const std::string& label, Animation** animation, float size = 64, Animation** oldValue = nullptr);
	ChangeStatus DrawShaderControl(const std::string& label, Shader** shader, float size = 64, Shader** oldValue = nullptr);
	ChangeStatus DrawFontControl(const std::string& label, Font** font, float size = 64, Font** oldValue = nullptr);

	ChangeData DrawReflectedMember(void* object, MemberData& member);
	ChangeData DrawMetaType(void* object, MetaType& type);
}