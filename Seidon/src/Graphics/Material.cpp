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

		*((Texture**)ptr) = resourceManager.GetTexture("roughness_default");
		ptr += sizeof(Texture*);

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
		int layoutSize = layout.members.size();
		out.write((char*)&layoutSize, sizeof(int));
		//out.write((char*)&layout.members[0], layoutSize);


		for (MemberData& m : layout.members)
		{
			switch (m.type)
			{
			case Types::TEXTURE:
			{
				Texture* t = *(Texture**)&data[m.offset];
				UUID id = t->GetId();
				out.write((char*)&id, sizeof(UUID));
				break;
			}
			case Types::VECTOR3_COLOR:
			{
				glm::vec3* color = (glm::vec3*)&data[m.offset];
				out.write((char*)color, sizeof(glm::vec3));
				break;
			}
			}
		}
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
		
		int layoutSize = 0;
		in.read((char*)&layoutSize, sizeof(int));

		//MetaType oldLayout;
		//in.read((char*)oldLayout.members.data(), layoutSize);

		MetaType& newLayout = *shader->GetBufferLayout();

		if (layoutSize != newLayout.members.size())
		{
			ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
			memset(data, 0, 500);

			for (MemberData& m : newLayout.members)
				switch (m.type)
				{
				case Types::VECTOR3_COLOR:
					*(glm::vec3*)(data + m.offset) = glm::vec3(1);
					break;

				case Types::TEXTURE:
					*(Texture**)(data + m.offset) = resourceManager.GetTexture("albedo_default");
					break;
				}
		}
		else
		{
			for (MemberData& m : newLayout.members)
			{
				switch (m.type)
				{
				case Types::TEXTURE:
				{
					UUID id;
					in.read((char*)&id, sizeof(UUID));

					Texture* t = resourceManager->GetOrLoadTexture(id);
					*(Texture**)&data[m.offset] = t;
					break;
				}
				case Types::VECTOR3_COLOR:
				{
					glm::vec3 color;
					in.read((char*)&color, sizeof(glm::vec3));

					*(glm::vec3*)&data[m.offset] = color;
					break;
				}
				}
			}
		}
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