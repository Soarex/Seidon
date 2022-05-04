#include "Reflection.h"

#include "../Core/Application.h"

namespace Seidon
{
	void MetaType::Save(std::ofstream& out, byte* data)
	{
		for (MemberData& m : members)
		{
			switch (m.type)
			{
			case Types::ID:
			{
				UUID* item = (UUID*)&data[m.offset];
				out.write((char*)item, sizeof(UUID));
				break;
			}
			case Types::STRING:
			{
				std::string* item = (std::string*)&data[m.offset];
				size_t size = item->length() + 1;

				out.write((char*)&size, sizeof(size_t));
				out.write(item->c_str(), size * sizeof(char));
				break;
			}
			case Types::FLOAT: case Types::FLOAT_NORMALIZED :
			{
				float* item = (float*)&data[m.offset];
				out.write((char*)item, sizeof(float));
				break;
			}
			case Types::BOOL:
			{
				bool* item = (bool*)&data[m.offset];
				out.write((char*)item, sizeof(bool));
				break;
			}
			case Types::VECTOR3: case Types::VECTOR3_ANGLES: case Types::VECTOR3_COLOR:
			{
				glm::vec3* item = (glm::vec3*)&data[m.offset];
				out.write((char*)item, sizeof(glm::vec3));
				break;
			}
			case Types::VECTOR4: case Types::VECTOR4_COLOR:
			{
				glm::vec4* item = (glm::vec4*)&data[m.offset];
				out.write((char*)item, sizeof(glm::vec4));
				break;
			}
			case Types::TEXTURE:
			{
				Texture* item = *(Texture**)&data[m.offset];
				UUID id = item->GetId();
				out.write((char*)&id, sizeof(UUID));
				break;
			}
			case Types::TEXTURE_VECTOR:
			{
				std::vector<Texture*>* item = (std::vector<Texture*>*) & data[m.offset];

				size_t size = item->size();
				out.write((char*)&size, sizeof(size_t));

				for (Texture* t : *item)
				{
					UUID id = t->GetId();
					out.write((char*)&id, sizeof(UUID));
				}
				break;
			}
			case Types::MESH:
			{
				Mesh* item = *(Mesh**)&data[m.offset];
				UUID id = item->id;
				out.write((char*)&id, sizeof(UUID));
				break;
			}
			case Types::MESH_VECTOR:
			{
				std::vector<Mesh*>* item = (std::vector<Mesh*>*) & data[m.offset];

				size_t size = item->size();
				out.write((char*)&size, sizeof(size_t));

				for (Mesh* m : *item)
				{
					UUID id = m->id;
					out.write((char*)&id, sizeof(UUID));
				}
				break;
			}
			case Types::MATERIAL:
			{
				Material* item = *(Material**)&data[m.offset];
				UUID id = item->id;
				out.write((char*)&id, sizeof(UUID));
				break;
			}
			case Types::MATERIAL_VECTOR:
			{
				std::vector<Material*>* item = (std::vector<Material*>*) & data[m.offset];

				size_t size = item->size();
				out.write((char*)&size, sizeof(size_t));

				for (Material* m : *item)
				{
					UUID id = m->id;
					out.write((char*)&id, sizeof(UUID));
				}
				break;
			}
			case Types::CUBEMAP:
			{
				HdrCubemap* item = *(HdrCubemap**)&data[m.offset];
				UUID id = item->GetId();
				out.write((char*)&id, sizeof(UUID));
				break;
			}
			case Types::ARMATURE:
			{
				Armature* item = *(Armature**)&data[m.offset];
				UUID id = item->id;
				out.write((char*)&id, sizeof(UUID));
				break;
			}
			case Types::ANIMATION:
			{
				Animation* item = *(Animation**)&data[m.offset];
				UUID id = item->id;
				out.write((char*)&id, sizeof(UUID));
				break;
			}
			case Types::SHADER:
			{
				Shader* item = *(Shader**)&data[m.offset];
				UUID id = item->GetId();
				out.write((char*)&id, sizeof(UUID));
				break;
			}
			case Types::SPRITE:
			{
				Sprite* item = *(Sprite**)&data[m.offset];
				UUID id = item->id;
				out.write((char*)&id, sizeof(UUID));
				break;
			}
			case Types::UNKNOWN:
				break;
			}
		}
	}

	void MetaType::Load(std::ifstream& in, byte* data)
	{
		ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
		for (MemberData& m : members)
		{
			switch (m.type)
			{
			case Types::ID:
			{
				UUID item;
				in.read((char*)&item, sizeof(UUID));

				*(UUID*)&data[m.offset] = item;
				break;
			}
			case Types::STRING:
			{
				size_t size;
				in.read((char*)&size, sizeof(size_t));

				char buffer[1024];
				in.read(buffer, size * sizeof(char));

				*(std::string*)&data[m.offset] = buffer;
				break;
			}
			case Types::FLOAT: case Types::FLOAT_NORMALIZED:
			{
				float item;
				in.read((char*)&item, sizeof(float));

				*(float*)&data[m.offset] = item;
				break;
			}
			case Types::BOOL:
			{
				bool item;
				in.read((char*)&item, sizeof(bool));

				*(bool*)&data[m.offset] = item;
				break;
			}
			case Types::VECTOR3: case Types::VECTOR3_ANGLES: case Types::VECTOR3_COLOR:
			{
				glm::vec3 item;
				in.read((char*)&item, sizeof(glm::vec3));

				*(glm::vec3*)&data[m.offset] = item;
				break;
			}
			case Types::VECTOR4: case Types::VECTOR4_COLOR:
			{
				glm::vec4 item;
				in.read((char*)&item, sizeof(glm::vec4));

				*(glm::vec4*)&data[m.offset] = item;
				break;
			}
			case Types::TEXTURE:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if(resourceManager.IsTextureRegistered(id))
					*(Texture**)&data[m.offset] = resourceManager.GetOrLoadTexture(id);
				else
					*(Texture**)&data[m.offset] = resourceManager.GetTexture("albedo_default");

				break;
			}
			case Types::TEXTURE_VECTOR:
			{
				size_t size;
				in.read((char*)&size, sizeof(size_t));

				std::vector<Texture*> item;
				item.reserve(size);

				for (int i = 0; i < size; i++)
				{
					UUID id;
					in.read((char*)&id, sizeof(UUID));

					if (resourceManager.IsTextureRegistered(id))
						item.push_back(resourceManager.GetOrLoadTexture(id));
					else
						item.push_back(resourceManager.GetTexture("albedo_default"));
				}

				*(std::vector<Texture*>*)& data[m.offset] = item;
				break;
			}
			case Types::MESH:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsMeshRegistered(id))
					*(Mesh**)&data[m.offset] = resourceManager.GetOrLoadMesh(id);
				else
					*(Mesh**)&data[m.offset] = resourceManager.GetMesh("empty_mesh");

				break;
			}
			case Types::MESH_VECTOR:
			{
				size_t size;
				in.read((char*)&size, sizeof(size_t));

				std::vector<Mesh*> item;
				item.reserve(size);

				for (int i = 0; i < size; i++)
				{
					UUID id;
					in.read((char*)&id, sizeof(UUID));

					if (resourceManager.IsMeshRegistered(id))
						item.push_back(resourceManager.GetOrLoadMesh(id));
					else
						item.push_back(resourceManager.GetMesh("empty_mesh"));
				}

				*(std::vector<Mesh*>*)& data[m.offset] = item;
				break;
			}
			case Types::MATERIAL:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsMaterialRegistered(id))
					*(Material**)&data[m.offset] = resourceManager.GetOrLoadMaterial(id);
				else
					*(Material**)&data[m.offset] = resourceManager.GetMaterial("default_material");

				break;
			}
			case Types::MATERIAL_VECTOR:
			{
				size_t size;
				in.read((char*)&size, sizeof(size_t));

				std::vector<Material*> item;
				item.reserve(size);

				for (int i = 0; i < size; i++)
				{
					UUID id;
					in.read((char*)&id, sizeof(UUID));

					if (resourceManager.IsMeshRegistered(id))
						item.push_back(resourceManager.GetOrLoadMaterial(id));
					else
						item.push_back(resourceManager.GetMaterial("default_material"));
				}

				*(std::vector<Material*>*)& data[m.offset] = item;
				break;
			}
			case Types::CUBEMAP:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsCubemapRegistered(id))
					*(HdrCubemap**)&data[m.offset] = resourceManager.GetOrLoadCubemap(id);
				else
					*(HdrCubemap**)&data[m.offset] = resourceManager.GetCubemap("default_cubemap");

				break;
			}
			case Types::ARMATURE:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsArmatureRegistered(id))
					*(Armature**)&data[m.offset] = resourceManager.GetOrLoadArmature(id);
				else
					*(Armature**)&data[m.offset] = resourceManager.GetArmature("default_armature");

				break;
			}
			case Types::ANIMATION:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsAnimationRegistered(id))
					*(Animation**)&data[m.offset] = resourceManager.GetOrLoadAnimation(id);
				else
					*(Animation**)&data[m.offset] = resourceManager.GetAnimation("default_animation");

				break;
			}
			case Types::SHADER:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsShaderRegistered(id))
					*(Shader**)&data[m.offset] = resourceManager.GetOrLoadShader(id);
				else
					*(Shader**)&data[m.offset] = resourceManager.GetShader("default_shader");

				break;
			}
			case Types::SPRITE:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));
				/*
				if (resourceManager.IsShaderRegistered(id))
					*(Shader**)&data[m.offset] = resourceManager.GetOrLoadShader(id);
				else
					*(Shader**)&data[m.offset] = resourceManager.GetShader("default_shader");
				*/
				break;
			}
			case Types::UNKNOWN:
				break;
			}
		}
	}
}