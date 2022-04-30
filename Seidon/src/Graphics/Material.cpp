#include "Material.h"
#include "../Core/Application.h"

#include <iostream>
#include <fstream>

namespace Seidon
{
	Material::Material(UUID id)
		: id(id)
	{
		memset(data, 0, 500);
		ResourceManager& resourceManager = *Application::Get()->GetResourceManager();

		name = "";
		shader = resourceManager.GetShader("default_shader");
		
		byte* ptr = data;

		*((glm::vec3*)ptr) = glm::vec3(1);
		ptr += sizeof(glm::vec3);

		*((Texture**)ptr) = resourceManager.GetTexture("albedo_default");
		ptr += sizeof(Texture*);

		*((Texture**)ptr) = resourceManager.GetTexture("normal_default");
		ptr += sizeof(Texture*);

		//*((float*)ptr) = 0.0f;
		//ptr += sizeof(float);

		*((Texture**)ptr) = resourceManager.GetTexture("roughness_default");
		ptr += sizeof(Texture*);

		//*((float*)ptr) = 0.0f;
		//ptr += sizeof(float);

		*((Texture**)ptr) = resourceManager.GetTexture("metallic_default");
		ptr += sizeof(Texture*);

		*((Texture**)ptr) = resourceManager.GetTexture("ao_default");
		ptr += sizeof(Texture*);
	}

	void Material::Save(const std::string& path)
	{
		std::ofstream out(path, std::ios::out | std::ios::binary);

		out.write((char*)&id, sizeof(UUID));

		size_t size = name.length() + 1;
		out.write((char*)&size, sizeof(size_t));

		out.write(name.c_str(), size * sizeof(char));

		UUID shaderId = shader->GetId();
		out.write((char*)&shaderId, sizeof(UUID));

		MetaType& layout = *shader->GetBufferLayout();
		layout.Save(out, data);
	}

	void Material::SaveAsync(const std::string& path)
	{
		Application::Get()->GetWorkManager()->Execute([&]()
			{
				Save(path);
			});
	}

	void Material::Load(const std::string& path)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);

		if (!in)
		{
			std::cerr << "Error opening material file: " << path << std::endl;
			return;
		}

		in.read((char*)&id, sizeof(UUID));

		size_t size = 0;
		in.read((char*)&size, sizeof(size_t));

		char buffer[2048];
		in.read(buffer, size * sizeof(char));
		name = buffer;

		ResourceManager* resourceManager = Application::Get()->GetResourceManager();
		UUID shaderId;

		in.read((char*)&shaderId, sizeof(UUID));
		shader = resourceManager->GetOrLoadShader(shaderId);

		MetaType& layout = *shader->GetBufferLayout();
		layout.Load(in, data);
	}

	void Material::LoadAsync(const std::string& path)
	{
		ResourceManager* resourceManager = Application::Get()->GetResourceManager();
		/*
		albedo = resourceManager->GetTexture("albedo_default");
		roughness = resourceManager->GetTexture("roughness_default");
		normal = resourceManager->GetTexture("normal_default");
		metallic = resourceManager->GetTexture("metallic_default");
		ao = resourceManager->GetTexture("ao_default");
		*/
		Application::Get()->GetWorkManager()->Execute([&]()
			{
				Load(path);
			});
	}
}