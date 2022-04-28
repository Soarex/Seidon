#pragma once
#include <string>
#include <glm/vec3.hpp>
#include "Texture.h"
#include "Shader.h"

namespace Seidon
{
	typedef unsigned char byte;

	class Material
	{
	public:
		UUID id;
		std::string name;
		Shader* shader;

		byte data[500];

		Material(UUID id = UUID());
	public:
		void Save(const std::string& path);
		void SaveAsync(const std::string& path);
		void Load(const std::string& path);
		void LoadAsync(const std::string& path);
	};
}