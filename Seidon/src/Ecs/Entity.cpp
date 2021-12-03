#include "Entity.h"

#include "../Core/Application.h"
#include "../Reflection/Reflection.h"

#include <yaml-cpp/yaml.h>


namespace Seidon
{
	Entity::Entity(entt::entity id, entt::registry* registry)
		:ID(id), registry(registry) {}

	void Entity::SaveText(YAML::Emitter& out)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Components" <<  YAML::BeginSeq;

		for (auto& metaType : Application::Get()->GetComponentMetaTypes())
		{
			if (!metaType.Has(*this)) continue;

			out << YAML::BeginMap;
			out << YAML::Key << "Name" << metaType.name;
			out << YAML::Key << "Members" << YAML::BeginSeq;

			for (auto& member : metaType.members)
			{
				out << YAML::BeginMap;
				unsigned char* obj = (unsigned char*)metaType.Get(*this);

				out << YAML::Key << "Name" << YAML::Value << member.name;
				out << YAML::Key << "Type" << YAML::Value << (int)member.type;
				
				switch (member.type)
				{
				case Types::ID:
				{
					UUID* data = (UUID*)(obj + member.offset);
					out << YAML::Key << "Value" << YAML::Value << *data;
					break;
				}
				case Types::STRING:
				{
					std::string* data = (std::string*)(obj + member.offset);
					out << YAML::Key << "Value" << YAML::Value << *data;
					break;
				}
				case Types::FLOAT:
				{
					float* data = (float*)(obj + member.offset);
					out << YAML::Key << "Value" << YAML::Value << *data;
					break;
				}
				case Types::VECTOR3: case Types::VECTOR3_ANGLES: case Types::VECTOR3_COLOR:
				{
					glm::vec3* data = (glm::vec3*)(obj + member.offset);
					out << YAML::Key << "Value" << YAML::Value << *data;
					break;
				}
				case Types::TEXTURE:
				{
					Texture* t = *(Texture**)(obj + member.offset);
					out << YAML::Key << "Value" << YAML::Value << t->GetId();
					break;
				}
				case Types::MESH:
				{
					Mesh* m = *(Mesh**)(obj + member.offset);
					out << YAML::Key << "Value" << YAML::Value << m->id;
					break;
				}
				case Types::MATERIAL_VECTOR:
				{
					std::vector<Material*>* v = (std::vector<Material*>*)(obj + member.offset);

					out << YAML::Key << "Value" << YAML::BeginSeq;

					for (Material* m : *v)
					{
						out << YAML::BeginMap;
						out << YAML::Key << "Id" << YAML::Value << m->id;
						out << YAML::EndMap;
					}
					out << YAML::EndSeq;
					break;
				}
				case Types::CUBEMAP:
				{
					HdrCubemap* c = *(HdrCubemap**)(obj + member.offset);
					out << YAML::Key << "Value" << YAML::Value << c->GetId();
					break;
				}
				case Types::UNKNOWN:
				{
					unsigned char* data = obj + member.offset;
					out << YAML::Key << "Value" << YAML::BeginSeq;

					for (int i = 0; i < member.size; i++)
						out << (unsigned int)data[i];

					out << YAML::EndSeq;
					break;
				}
				}
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	void Entity::LoadText(YAML::Node& entityNode)
	{	
		if (HasComponent<IDComponent>())
		{
			RemoveComponent<IDComponent>();
			RemoveComponent<NameComponent>();
			RemoveComponent<TransformComponent>();
		}

		YAML::Node components = entityNode["Components"];

		for (YAML::Node componentNode : components)
		{
			ComponentMetaType metaType = Application::Get()->GetComponentMetaTypeByName(componentNode["Name"].as<std::string>());

			YAML::Node membersNode = componentNode["Members"];

			Byte* data = (Byte*)metaType.Add(*this);

			for (int i = 0 ; i < metaType.members.size(); i++)
			{
				YAML::Node memberNode = membersNode[i];
				MemberData memberData = metaType.members[i];

				if (memberData.name != memberNode["Name"].as<std::string>() || (int)memberData.type != memberNode["Type"].as<int>())
				{
					std::cout << "Error parsing file for " << metaType.name << std::endl;
					std::cout << "Should be " << memberData.name << " but is " << memberNode["Name"].as<std::string>() << std::endl;
					std::cout << "Should be " << (int)memberData.type << " but is " << memberNode["Type"].as<int>() << std::endl;
					return;
				}

				Byte* member = data + memberData.offset;

				switch(memberData.type)
				{
				case Types::ID:
					*(UUID*)member = memberNode["Value"].as<uint64_t>();
					break;

				case Types::STRING:
					*(std::string*)member = memberNode["Value"].as<std::string>();
					break;

				case Types::FLOAT:
					*(float*)member = memberNode["Value"].as<float>();
					break;

				case Types::VECTOR3: case Types::VECTOR3_ANGLES: case Types::VECTOR3_COLOR:
					*(glm::vec3*)member = memberNode["Value"].as<glm::vec3>();
					break;

				case Types::TEXTURE:
					*(Texture**)member = Application::Get()->GetResourceManager()->GetTexture(memberNode["Value"].as<uint64_t>());
					break;

				case Types::MESH:
					*(Mesh**)member = Application::Get()->GetResourceManager()->GetMesh(memberNode["Value"].as<uint64_t>());
					break;

				case Types::MATERIAL_VECTOR:
					for(YAML::Node material : memberNode["Value"])
						((std::vector<Material*>*)member)->push_back(
							Application::Get()->GetResourceManager()->GetMaterial(material["Id"].as<uint64_t>())
							);
					break;

				case Types::CUBEMAP:
					*(HdrCubemap**)member = Application::Get()->GetResourceManager()->GetCubemap(memberNode["Value"].as<uint64_t>());
					break;

				case Types::UNKNOWN:
					break;
				}
			}
		}
	}

}