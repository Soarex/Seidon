#pragma once
#include <Seidon.h>
#include <filesystem>
#include <Imgui/imgui_internal.h>

namespace Seidon
{
	bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	bool DrawColorControl(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	bool DrawFloatControl(const std::string& label, float& value, float columnWidth = 100.0f);
	bool DrawFloatSliderControl(const std::string& label, float& value, float min = 0, float max = 1, float columnWidth = 100.0f);
	bool DrawBoolControl(const std::string& label, bool& value, float columnWidth = 100.0f);
	bool DrawStringControl(const std::string& label, std::string& value, float columnWidth = 100.0f);
	bool DrawTextureControl(const std::string& label, Texture** texture, float size = 64);
	bool DrawCubemapControl(const std::string& label, HdrCubemap** texture, float size = 64);
	bool DrawMeshControl(const std::string& label, Mesh** mesh, float size = 64);
	bool DrawMaterialControl(const std::string& label, Material** material, float size = 64);
	bool DrawMaterialEditor(const std::string& label, Material* material, bool* open = 0);
	bool DrawArmatureControl(const std::string& label, Armature** armature, float size = 64);
	bool DrawAnimationControl(const std::string& label, Animation** animation, float size = 64);
	bool DrawShaderControl(const std::string& label, Shader** shader, float size = 64);

	bool DrawReflectedMember(void* object, MemberData& member);
	bool DrawMetaType(void* object, MetaType& type);
}