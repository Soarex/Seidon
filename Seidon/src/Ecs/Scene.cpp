#include "Scene.h"
#include "Entity.h"
#include "../Core/Application.h"

#include <yaml-cpp/yaml.h>

namespace Seidon
{

	Scene::Scene(const std::string& name)
		: name(name)
	{

	}


	Scene::~Scene()
	{
		Destroy();
	}

	void Scene::Init()
	{
		for (auto& [typeName, system] : systems)
			system->Init();
	}

	void Scene::Update(float deltaTime)
	{
		for (auto& [typeName, system] : systems)
			system->Update(deltaTime);
	}

	void SaveSystems(YAML::Emitter& out, Scene& scene)
	{
		for (auto& metaType : Application::Get()->GetSystemsMetaTypes())
		{
			if (!metaType.Has(scene)) continue;

			out << YAML::BeginMap;
			out << YAML::Key << "Name" << metaType.name;
			out << YAML::Key << "Members" << YAML::BeginSeq;

			for (auto& member : metaType.members)
			{
				out << YAML::BeginMap;
				unsigned char* obj = (unsigned char*)metaType.Get(scene);

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
	}

	void LoadSystems(YAML::Node& node, Scene& scene)
	{
		for (YAML::Node systemNode : node)
		{
			SystemMetaType metaType = Application::Get()->GetSystemMetaTypeByName(systemNode["Name"].as<std::string>());

			YAML::Node membersNode = systemNode["Members"];

			Byte* data = (Byte*)metaType.Add(scene);

			for (int i = 0; i < metaType.members.size(); i++)
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

				switch (memberData.type)
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
					break;

				case Types::MESH:
					break;

				case Types::MATERIAL_VECTOR:
					break;

				case Types::CUBEMAP:
					break;

				case Types::UNKNOWN:
					break;
				}
			}
		}
	}

	void Scene::SaveText(std::ofstream& fileOut)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Id" << YAML::Value << id;
		out << YAML::Key << "Name" << YAML::Value << name;
		out << YAML::Key << "Entities" << YAML::BeginSeq;

		registry.each([&](auto entityID)
			{
				Entity entity = { entityID, &registry };
				entity.SaveText(out);
			});

		out << YAML::EndSeq;

		out << YAML::Key << "Systems" << YAML::BeginSeq;

		SaveSystems(out, *this);

		out << YAML::EndSeq;

		out << YAML::EndMap;
		fileOut << out.c_str();
	}

	void Scene::LoadText(std::string& path)
	{
		YAML::Node data;

		try
		{
			data = YAML::LoadFile(path);
		}
		catch (YAML::ParserException e)
		{
			std::cout << e.msg << std::endl;
			return;
		}

		id = data["Id"].as<uint64_t>();
		name = data["Name"].as<std::string>();

		for (YAML::Node entityNode : data["Entities"])
			CreateEntity().LoadText(entityNode);

		YAML::Node systemsNode = data["Systems"];
		LoadSystems(systemsNode, *this);
	}

	void Scene::Destroy()
	{
		for (auto& [typeName, system] : systems)
		{
			system->Destroy();
			delete system;
		}

		systems.clear();
		registry.clear();
	}

	Scene* Scene::Duplicate()
	{
		Scene* newScene = new Scene();

		CopyEntities(newScene);
		CopySystems(newScene);

		return newScene;
	}

	void Scene::CopyEntities(Scene* other)
	{
		auto& srcRegistry = registry;
		auto& dstRegistry = other->registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		auto idView = srcRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcRegistry.get<IDComponent>(e).ID;
			std::string& name = srcRegistry.get<NameComponent>(e).name;

			Entity newEntity = other->CreateEntity(name, uuid);
			enttMap[uuid] = newEntity.ID;
		}

		const std::vector<ComponentMetaType>& components = Application::Get()->GetComponentMetaTypes();
		for (auto& metaType : components)
			metaType.Copy(srcRegistry, dstRegistry, enttMap);
	}

	void Scene::CopySystems(Scene* other)
	{
		const std::vector<SystemMetaType>& systems = Application::Get()->GetSystemsMetaTypes();
		for (auto& metaType : systems)
			if (metaType.Has(*this))
				metaType.Copy(*this, *other);
	}

	Entity Scene::CreateEntity(const std::string& name, const UUID& id)
	{
		Entity e(registry.create(), &registry);
		e.AddComponent<TransformComponent>();
		e.AddComponent<IDComponent>();

		if (name == "")
			e.AddComponent<NameComponent>("Entity #" + std::to_string((int)e.ID));
		else
			e.AddComponent<NameComponent>(name);

		return e;
	}

	void Scene::DestroyEntity(const Entity& entity)
	{
		registry.destroy(entity.ID);
	}
}
