#include "Entity.h"

#include "../Core/Application.h"
#include "../Reflection/Reflection.h"

namespace Seidon
{
	Entity::Entity(entt::entity id, Scene* scene)
		:ID(id), scene(scene) {}

	void Entity::Save(std::ofstream& out)
	{
		int componentCount = 0;
		for (auto& metaType : Application::Get()->GetComponentMetaTypes())
			if (metaType.Has(*this)) componentCount++;

		out.write((char*)&componentCount, sizeof(int));

		for (auto& metaType : Application::Get()->GetComponentMetaTypes())
		{
			if (!metaType.Has(*this)) continue;

			size_t size = metaType.name.length() + 1;

			out.write((char*)&size, sizeof(size_t));
			out.write(metaType.name.c_str(), size * sizeof(char));

			size_t memberCount = metaType.members.size();
			out.write((char*)&memberCount, sizeof(size_t));

			metaType.Save(out, (byte*)metaType.Get(*this));
		}
	}

	void Entity::Load(std::ifstream& in)
	{
		if (HasComponent<IDComponent>())
		{
			RemoveComponent<IDComponent>();
			RemoveComponent<NameComponent>();
			RemoveComponent<TransformComponent>();
			RemoveComponent<MouseSelectionComponent>();
		}

		int componentCount = 0;
		in.read((char*)&componentCount, sizeof(int));

		for (int i = 0; i < componentCount; i++)
		{
			size_t size = 0;
			char buffer[500];

			in.read((char*)&size, sizeof(size_t));
			in.read(buffer, size * sizeof(char));

			if (!Application::Get()->IsComponentRegistered(buffer))
			{
				std::cerr << "Serialized component '" << buffer << "' is not a registered component" << std::endl;
				continue;
			}

			ComponentMetaType metaType = Application::Get()->GetComponentMetaTypeByName(buffer);
			size_t memberCount = 0;
			in.read((char*)&memberCount, sizeof(size_t));

			if (metaType.members.size() != memberCount)
			{
				std::cerr << "Serialized component '" << buffer << "' not compatible with current types" << std::endl;
				continue;
			}

			void* component = metaType.Add(*this);

			metaType.Load(in, (byte*)component);
		}
	}

	Entity Entity::Duplicate()
	{
		Entity e = scene->CreateEntity();

		e.RemoveComponent<NameComponent>();
		e.RemoveComponent<IDComponent>();
		e.RemoveComponent<MouseSelectionComponent>();
		e.RemoveComponent<TransformComponent>();

		const std::vector<ComponentMetaType>& components = Application::Get()->GetComponentMetaTypes();
		for (auto& metaType : components)
			if (metaType.Has(*this))
				metaType.Copy(*this, e);

		e.AddComponent<IDComponent>();

		return e;
	}

}