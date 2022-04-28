#pragma once
#include <Seidon.h>
#include <filesystem>
#include <Imgui/imgui_internal.h>

namespace Seidon
{
	void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	void DrawColorControl(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	void DrawFloatControl(const std::string& label, float& value, float columnWidth = 100.0f);
	void DrawBoolControl(const std::string& label, bool& value, float columnWidth = 100.0f);
	void DrawStringControl(const std::string& label, std::string& value, float columnWidth = 100.0f);
	void DrawTextureControl(const std::string& label, Texture** texture, float size = 64);
	void DrawCubemapControl(const std::string& label, HdrCubemap** texture, float size = 64);
	void DrawMeshControl(const std::string& label, Mesh** mesh, float size = 64);
	bool DrawMaterialControl(const std::string& label, Material** material, float size = 64);
	void DrawMaterialEditor(const std::string& label, Material* material, bool* open = 0);
	void DrawArmatureControl(const std::string& label, Armature** armature, float size = 64);
	void DrawAnimationControl(const std::string& label, Animation** animation, float size = 64);
	bool DrawShaderControl(const std::string& label, Shader** shader, float size = 64);

	void DrawReflectedMember(void* object, MemberData& member);
	void DrawReflectedMembers(void* object, std::vector<MemberData>& members);
}