#include "Material.h"
#include "../Core/Application.h"

namespace Seidon
{
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

	void Material::Load(const std::string& path)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);

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
		albedo = resourceManager->GetTexture(textureId);

		in.read((char*)&textureId, sizeof(UUID));
		roughness = resourceManager->GetTexture(textureId);

		in.read((char*)&textureId, sizeof(UUID));
		normal = resourceManager->GetTexture(textureId);

		in.read((char*)&textureId, sizeof(UUID));
		metallic = resourceManager->GetTexture(textureId);

		in.read((char*)&textureId, sizeof(UUID));
		ao = resourceManager->GetTexture(textureId);
	}
}