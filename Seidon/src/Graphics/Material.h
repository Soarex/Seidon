#pragma once
#include "Texture.h"
#include "Shader.h"
#include "../Reflection/Reflection.h"

#include <glm/vec3.hpp>

#include <string>

namespace Seidon
{
	typedef unsigned char byte;

	class Material : public Asset
	{
	public:
		Shader* shader;

		byte data[500];

		Material(UUID id = UUID());
	public:
		using Asset::Save;
		using Asset::Load;

		void Save(std::ofstream& out);
		void SaveAsync(const std::string& path);
		void Load(std::ifstream& in);
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