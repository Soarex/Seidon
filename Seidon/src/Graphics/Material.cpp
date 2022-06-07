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
		
		byte* ptr = data;

		*((glm::vec3*)ptr) = glm::vec3(1);
		ptr += sizeof(glm::vec3);

		*((Texture**)ptr) = resourceManager.GetAsset<Texture>("albedo_default");
		ptr += sizeof(Texture*);

		*((Texture**)ptr) = resourceManager.GetAsset<Texture>("normal_default");
		ptr += sizeof(Texture*);

		//*((float*)ptr) = 0.0f;
		//ptr += sizeof(float);

		*((Texture**)ptr) = resourceManager.GetAsset<Texture>("roughness_default");
		ptr += sizeof(Texture*);

		//*((float*)ptr) = 0.0f;
		//ptr += sizeof(float);

		*((Texture**)ptr) = resourceManager.GetAsset<Texture>("metallic_default");
		ptr += sizeof(Texture*);

		*((Texture**)ptr) = resourceManager.GetAsset<Texture>("ao_default");
		ptr += sizeof(Texture*);
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