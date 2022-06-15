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
			case Types::ID_VECTOR:
			{
				std::vector<UUID>* item = (std::vector<UUID>*) &data[m.offset];

				size_t size = item->size();
				out.write((char*)&size, sizeof(size_t));

				for (UUID& id : *item)
					out.write((char*)&id, sizeof(UUID));
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
			case Types::FLOAT: case Types::FLOAT_NORMALIZED: case Types::FLOAT_ANGLE:
			{
				float* item = (float*)&data[m.offset];
				out.write((char*)item, sizeof(float));
				break;
			}
			case Types::INT:
			{
				int* item = (int*)&data[m.offset];
				out.write((char*)item, sizeof(int));
				break;
			}
			case Types::BOOL:
			{
				bool* item = (bool*)&data[m.offset];
				out.write((char*)item, sizeof(bool));
				break;
			}
			case Types::VECTOR2: case Types::VECTOR2_ANGLES:
			{
				glm::vec2* item = (glm::vec2*)&data[m.offset];
				out.write((char*)item, sizeof(glm::vec2));
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
			case Types::SKINNED_MESH:
			{
				SkinnedMesh* item = *(SkinnedMesh**)&data[m.offset];
				UUID id = item->id;
				out.write((char*)&id, sizeof(UUID));
				break;
			}
			case Types::SKINNED_MESH_VECTOR:
			{
				std::vector<SkinnedMesh*>* item = (std::vector<SkinnedMesh*>*) & data[m.offset];

				size_t size = item->size();
				out.write((char*)&size, sizeof(size_t));

				for (SkinnedMesh* m : *item)
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
			case Types::FONT:
			{
				Font* item = *(Font**)&data[m.offset];
				UUID id = item->GetId();
				out.write((char*)&id, sizeof(UUID));
				break;
			}
			case Types::MESH_COLLIDER:
			{
				MeshCollider* item = *(MeshCollider**)&data[m.offset];
				UUID id = item->id;
				out.write((char*)&id, sizeof(UUID));
				break;
			}
			case Types::SOUND:
			{
				Sound* item = *(Sound**)&data[m.offset];
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
			case Types::ID_VECTOR:
			{
				size_t size;
				in.read((char*)&size, sizeof(size_t));

				std::vector<UUID> item;
				item.reserve(size);

				for (int i = 0; i < size; i++)
				{
					UUID id;
					in.read((char*)&id, sizeof(UUID));

					item.push_back(id);
				}

				*(std::vector<UUID>*)& data[m.offset] = item;
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
			case Types::FLOAT: case Types::FLOAT_NORMALIZED: case Types::FLOAT_ANGLE:
			{
				float item;
				in.read((char*)&item, sizeof(float));

				*(float*)&data[m.offset] = item;
				break;
			}
			case Types::INT:
			{
				int item;
				in.read((char*)&item, sizeof(int));

				*(int*)&data[m.offset] = item;
				break;
			}
			case Types::BOOL:
			{
				bool item;
				in.read((char*)&item, sizeof(bool));

				*(bool*)&data[m.offset] = item;
				break;
			}
			case Types::VECTOR2: case Types::VECTOR2_ANGLES:
			{
				glm::vec3 item;
				in.read((char*)&item, sizeof(glm::vec2));

				*(glm::vec2*)&data[m.offset] = item;
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

				if(resourceManager.IsAssetRegistered(id))
					*(Texture**)&data[m.offset] = resourceManager.GetOrLoadAsset<Texture>(id);
				else
				{
					std::cerr << "Error loading member " << m.name << " of " << name << ": Texture id not registered" << std::endl;
					*(Texture**)&data[m.offset] = resourceManager.GetAsset<Texture>("albedo_default");
				}

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

					if (resourceManager.IsAssetRegistered(id))
						item.push_back(resourceManager.GetOrLoadAsset<Texture>(id));
					else
					{
						std::cerr << "Error loading member " << m.name << "[" << i << "] of " << name << ": Texture id not registered" << std::endl;
						item.push_back(resourceManager.GetAsset<Texture>("albedo_default"));
					}
				}

				*(std::vector<Texture*>*)& data[m.offset] = item;
				break;
			}
			case Types::MESH:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsAssetRegistered(id))
					*(Mesh**)&data[m.offset] = resourceManager.GetOrLoadAsset<Mesh>(id);
				else
				{
					std::cerr << "Error loading member " << m.name << " of " << name << ": Mesh id not registered" << std::endl;
					*(Mesh**)&data[m.offset] = resourceManager.GetAsset<Mesh>("empty_mesh");
				}

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

					if (resourceManager.IsAssetRegistered(id))
						item.push_back(resourceManager.GetOrLoadAsset<Mesh>(id));
					else
					{
						std::cerr << "Error loading member " << m.name << "[" << i << "] of " << name << ": Mesh id not registered" << std::endl;
						item.push_back(resourceManager.GetAsset<Mesh>("empty_mesh"));
					}
				}

				*(std::vector<Mesh*>*)& data[m.offset] = item;
				break;
			}
			case Types::SKINNED_MESH:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsAssetRegistered(id))
					*(SkinnedMesh**)&data[m.offset] = resourceManager.GetOrLoadAsset<SkinnedMesh>(id);
				else
				{
					std::cerr << "Error loading member " << m.name << " of " << name << ": Skinned mesh id not registered" << std::endl;
					*(SkinnedMesh**)&data[m.offset] = resourceManager.GetAsset<SkinnedMesh>("empty_skinned_mesh");
				}

				break;
			}
			case Types::SKINNED_MESH_VECTOR:
			{
				size_t size;
				in.read((char*)&size, sizeof(size_t));

				std::vector<SkinnedMesh*> item;
				item.reserve(size);

				for (int i = 0; i < size; i++)
				{
					UUID id;
					in.read((char*)&id, sizeof(UUID));

					if (resourceManager.IsAssetRegistered(id))
						item.push_back(resourceManager.GetOrLoadAsset<SkinnedMesh>(id));
					else
					{
						std::cerr << "Error loading member " << m.name << "[" << i << "] of " << name << ": Skinned mesh id not registered" << std::endl;
						item.push_back(resourceManager.GetAsset<SkinnedMesh>("empty_skinned_mesh"));
					}
				}

				*(std::vector<SkinnedMesh*>*)& data[m.offset] = item;
				break;
			}
			case Types::MATERIAL:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsAssetRegistered(id))
					*(Material**)&data[m.offset] = resourceManager.GetOrLoadAsset<Material>(id);
				else
				{
					std::cerr << "Error loading member " << m.name << " of " << name << ": Material id not registered" << std::endl;
					*(Material**)&data[m.offset] = resourceManager.GetAsset<Material>("default_material");
				}

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

					if (resourceManager.IsAssetRegistered(id))
						item.push_back(resourceManager.GetOrLoadAsset<Material>(id));
					else
					{
						std::cerr << "Error loading member " << m.name << "[" << i << "] of " << name << ": Material id not registered" << std::endl;
						item.push_back(resourceManager.GetAsset<Material>("default_material"));
					}
				}

				*(std::vector<Material*>*)& data[m.offset] = item;
				break;
			}
			case Types::CUBEMAP:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsAssetRegistered(id))
					*(HdrCubemap**)&data[m.offset] = resourceManager.GetOrLoadAsset<HdrCubemap>(id);
				else
				{
					std::cerr << "Error loading member " << m.name << " of " << name << ": Cubemap id not registered" << std::endl;
					*(HdrCubemap**)&data[m.offset] = resourceManager.GetAsset<HdrCubemap>("default_cubemap");
				}

				break;
			}
			case Types::ANIMATION:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsAssetRegistered(id))
					*(Animation**)&data[m.offset] = resourceManager.GetOrLoadAsset<Animation>(id);
				else
				{
					std::cerr << "Error loading member " << m.name << " of " << name << ": Animation id not registered" << std::endl;
					*(Animation**)&data[m.offset] = resourceManager.GetAsset<Animation>("default_animation");
				}

				break;
			}
			case Types::SHADER:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsAssetRegistered(id))
					*(Shader**)&data[m.offset] = resourceManager.GetOrLoadAsset<Shader>(id);
				else
				{
					std::cerr << "Error loading member " << m.name << " of " << name << ": Shader id not registered" << std::endl;
					*(Shader**)&data[m.offset] = resourceManager.GetAsset<Shader>("default_shader");
				}
				break;
			}
			case Types::FONT:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsAssetRegistered(id))
					*(Font**)&data[m.offset] = resourceManager.GetOrLoadAsset<Font>(id);
				else
				{
					std::cerr << "Error loading member " << m.name << " of " << name << ": Font id not registered" << std::endl;
					*(Font**)&data[m.offset] = resourceManager.GetAsset<Font>("empty_font");
				}
				break;
			}
			case Types::MESH_COLLIDER:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsAssetRegistered(id))
					*(MeshCollider**)&data[m.offset] = resourceManager.GetOrLoadAsset<MeshCollider>(id);
				else
				{
					std::cerr << "Error loading member " << m.name << " of " << name << ": Mesh Collider id not registered" << std::endl;
					*(MeshCollider**)&data[m.offset] = resourceManager.GetAsset<MeshCollider>("empty_mesh_collider");
				}
				break;
			}
			case Types::SOUND:
			{
				UUID id;
				in.read((char*)&id, sizeof(UUID));

				if (resourceManager.IsAssetRegistered(id))
					*(Sound**)&data[m.offset] = resourceManager.GetOrLoadAsset<Sound>(id);
				else
				{
					std::cerr << "Error loading member " << m.name << " of " << name << ": Sound id not registered" << std::endl;
					*(Sound**)&data[m.offset] = resourceManager.GetAsset<Sound>("empty_sound");
				}
				break;
			}
			case Types::UNKNOWN:
				break;
			}
		}

		if (OnChange) OnChange(data);
	}

	std::string MetaType::TypeToString(Types type)
	{
		if (type == Types::INT)
			return "Int";

		if (type == Types::FLOAT)
			return "Float";

		if (type == Types::FLOAT_NORMALIZED)
			return "Float normalized";

		if (type == Types::FLOAT_ANGLE)
			return "Float angle";

		if (type == Types::BOOL)
			return "Bool";

		if (type == Types::STRING)
			return "String";

		if (type == Types::ID)
			return "Id";

		if (type == Types::VECTOR2)
			return "Vec2";

		if (type == Types::VECTOR2_ANGLES)
			return "Vec2 Angles";

		if (type == Types::VECTOR3)
			return "Vec3";

		if (type == Types::VECTOR3_COLOR)
			return "Vec3 Color";

		if (type == Types::VECTOR3_ANGLES)
			return "Vec3 Angles";

		if (type == Types::MESH_VECTOR)
			return "Mesh Vector";

		if (type == Types::SKINNED_MESH_VECTOR)
			return "Skinned Mesh Vector";

		if (type == Types::MATERIAL_VECTOR)
			return "Material Vector";

		if (type == Types::TEXTURE_VECTOR)
			return "Texture Vector";

		if (type == Types::TEXTURE)
			return "Texture";

		if (type == Types::CUBEMAP)
			return "Cubemap";

		if (type == Types::MESH)
			return "Mesh";

		if (type == Types::SKINNED_MESH)
			return "Skinned Mesh";

		if (type == Types::ANIMATION)
			return "Animation";

		if (type == Types::SHADER)
			return "Shader";

		if (type == Types::FONT)
			return "Font";

		if (type == Types::SOUND)
			return "Sound";

		return "Unknown";
	}

	Types MetaType::StringToType(const std::string& string)
	{
		if (string == "INT")
			return Types::INT;

		if (string == "FLOAT")
			return Types::FLOAT;

		if (string == "FLOAT_NORMALIZED")
			return Types::FLOAT_NORMALIZED;

		if (string == "FLOAT_ANGLE")
			return Types::FLOAT_ANGLE;

		if (string == "BOOL")
			return Types::BOOL;

		if (string == "STRING")
			return Types::STRING;

		if (string == "ID")
			return Types::ID;

		if (string == "VECTOR2")
			return Types::VECTOR2;

		if (string == "VECTOR2_ANGLES")
			return Types::VECTOR2_ANGLES;

		if (string == "VECTOR3")
			return Types::VECTOR3;

		if (string == "VECTOR3_COLOR")
			return Types::VECTOR3_COLOR;

		if (string == "VECTOR3_ANGLES")
			return Types::VECTOR3_ANGLES;

		if (string == "MESH_VECTOR")
			return Types::MESH_VECTOR;

		if (string == "SKINNED_MESH_VECTOR")
			return Types::SKINNED_MESH_VECTOR;

		if (string == "MATERIAL_VECTOR")
			return Types::MATERIAL_VECTOR;

		if (string == "TEXTURE_VECTOR")
			return Types::TEXTURE_VECTOR;

		if (string == "TEXTURE")
			return Types::TEXTURE;

		if (string == "CUBEMAP")
			return Types::CUBEMAP;

		if (string == "MESH")
			return Types::MESH;

		if (string == "SKINNED_MESH")
			return Types::SKINNED_MESH;

		if (string == "ANIMATION")
			return Types::ANIMATION;

		if (string == "SHADER")
			return Types::SHADER;

		if (string == "FONT")
			return Types::FONT;

		if (string == "SOUND")
			return Types::SOUND;

		return Types::UNKNOWN;
	}

	void MetaType::CopyMember(const std::string& memberName, void* srcData, void* dstData)
	{
		MemberData& memberData = nameToMember.at(memberName);

		byte* srcMember = (byte*)srcData + memberData.offset;
		byte* dstMember = (byte*)dstData + memberData.offset;

		memcpy(dstMember, srcMember, memberData.size);
	}

	void MetaType::CopyMembers(void* srcData, void* dstData)
	{
		for (MemberData& data : members)
		{
			byte* srcMember = (byte*)srcData + data.offset;
			byte* dstMember = (byte*)dstData + data.offset;

			memcpy(dstMember, srcMember, data.size);
		}
	}
}