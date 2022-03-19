#pragma once
#include <string>
#include <glm/vec3.hpp>
#include "Texture.h"
#include "Shader.h"

namespace Seidon
{
	class Material
	{
	public:
		UUID id;
		std::string name;
		glm::vec3 tint;

		Texture* albedo;
		Texture* roughness;
		Texture* normal;
		Texture* metallic;
		Texture* ao;

		Material(UUID id = UUID());
	public:
		void Save(const std::string& path);
		void SaveAsync(const std::string& path);
		void Load(const std::string& path);
		void LoadAsync(const std::string& path);
	};
}