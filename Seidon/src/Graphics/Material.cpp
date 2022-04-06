#include "Material.h"
#include "../Core/Application.h"

namespace Seidon
{
	Material::Material(UUID id)
		: id(id)
	{
		ResourceManager& resourceManager = *Application::Get()->GetResourceManager();

		name = "";
		tint = glm::vec3(1);

		albedo = resourceManager.GetTexture("albedo_default");
		normal = resourceManager.GetTexture("normal_default");
		roughness = resourceManager.GetTexture("roughness_default");
		metallic = resourceManager.GetTexture("metallic_default");
		ao = resourceManager.GetTexture("ao_default");
	}

	void Material::Save(const std::string& path)
	{
		std::ofstream out(path, std::ios::out | std::ios::binary);

		out.write((char*)&id, sizeof(UUID));

		size_t size = name.length() + 1;
		out.write((char*)&size, sizeof(size_t));

		out.write(name.c_str(), size * sizeof(char));

		out.write((char*)&tint, sizeof(glm::vec3));

		UUID textureId = albedo->GetId();
		out.write((char*)&textureId, sizeof(UUID));

		textureId = roughness->GetId();
		out.write((char*)&textureId, sizeof(UUID));

		textureId = normal->GetId();
		out.write((char*)&textureId, sizeof(UUID));

		textureId = metallic->GetId();
		out.write((char*)&textureId, sizeof(UUID));

		textureId = ao->GetId();
		out.write((char*)&textureId, sizeof(UUID));
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

		in.read((char*)&tint, sizeof(glm::vec3));

		ResourceManager* resourceManager = Application::Get()->GetResourceManager();
		UUID textureId;

		in.read((char*)&textureId, sizeof(UUID));
		albedo = resourceManager->GetOrLoadTexture(textureId);

		in.read((char*)&textureId, sizeof(UUID));
		roughness = resourceManager->GetOrLoadTexture(textureId);

		in.read((char*)&textureId, sizeof(UUID));
		normal = resourceManager->GetOrLoadTexture(textureId);

		in.read((char*)&textureId, sizeof(UUID));
		metallic = resourceManager->GetOrLoadTexture(textureId);

		in.read((char*)&textureId, sizeof(UUID));
		ao = resourceManager->GetOrLoadTexture(textureId);
	}

	void Material::LoadAsync(const std::string& path)
	{
		ResourceManager* resourceManager = Application::Get()->GetResourceManager();
		albedo = resourceManager->GetTexture("albedo_default");
		roughness = resourceManager->GetTexture("roughness_default");
		normal = resourceManager->GetTexture("normal_default");
		metallic = resourceManager->GetTexture("metallic_default");
		ao = resourceManager->GetTexture("ao_default");

		Application::Get()->GetWorkManager()->Execute([&]()
			{
				Load(path);
			});
	}
}