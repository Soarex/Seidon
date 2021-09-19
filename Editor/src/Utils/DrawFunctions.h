#pragma once
#include <Seidon.h>
#include <filesystem>
#include <Imgui/imgui_internal.h>

namespace Seidon
{
	void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	void DrawFloatControl(const std::string& label, float& value, float columnWidth = 100.0f);
	void DrawTextureControl(const std::string& label, Texture*& texture, float size = 64);
}