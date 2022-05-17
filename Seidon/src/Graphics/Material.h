#pragma once
#include "Texture.h"
#include "Shader.h"
#include "../Reflection/Reflection.h"

#include <glm/vec3.hpp>

#include <string>

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

		template<typename T>
		void ModifyProperty(const std::string& propertyName, const T& value)
		{
			shader->GetBufferLayout()->ModifyMember<T>(propertyName, data, value);
		}

		template<typename T>
		T& GetProperty(const std::string& propertyName)
		{
			return shader->GetBufferLayout()->GetMember<T>(propertyName, data);
		}
	};
}