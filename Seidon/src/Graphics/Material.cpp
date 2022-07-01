#include "Material.h"
#include "../Core/Application.h"

#include <iostream>
#include <fstream>

namespace Seidon
{
	Material::Material(UUID id)
	{
		this->id = id;

		memset(data, 0, 500);
		ResourceManager& resourceManager = *Application::Get()->GetResourceManager();

		name = "";
		shader = resourceManager.GetAsset<Shader>("default_shader");
		
		MetaType* layout = shader->GetBufferLayout();

		layout->ModifyMember<glm::vec2>("UV Scale", data, glm::vec2(1));
		layout->ModifyMember<glm::vec3>("Tint", data, glm::vec3(1));
		layout->ModifyMember<Texture*>("Albedo", data, resourceManager.GetAsset<Texture>("albedo_default"));
		layout->ModifyMember<Texture*>("Normal", data, resourceManager.GetAsset<Texture>("normal_default"));
		layout->ModifyMember<Texture*>("Roughness", data, resourceManager.GetAsset<Texture>("roughness_default"));
		layout->ModifyMember<Texture*>("Metallic", data, resourceManager.GetAsset<Texture>("metallic_default"));
		layout->ModifyMember<Texture*>("Ambient Occlusion", data, resourceManager.GetAsset<Texture>("ao_default"));
	}

	void Material::Save(std::ofstream& out)
	{
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

	void Material::Load(std::ifstream& in)
	{
		in.read((char*)&id, sizeof(UUID));

		size_t size = 0;
		in.read((char*)&size, sizeof(size_t));

		char buffer[2048];
		in.read(buffer, size * sizeof(char));
		name = buffer;

		ResourceManager* resourceManager = Application::Get()->GetResourceManager();
		UUID shaderId;

		in.read((char*)&shaderId, sizeof(UUID));
		shader = resourceManager->GetOrLoadAsset<Shader>(shaderId);

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