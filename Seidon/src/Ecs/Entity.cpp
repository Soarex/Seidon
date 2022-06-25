#include "Entity.h"

#include "../Core/Application.h"
#include "../Reflection/Reflection.h"

namespace Seidon
{
	Entity::Entity(entt::entity id, Scene* scene)
		:ID(id), scene(scene) {}

	const std::string& Entity::GetName()
	{
		return GetComponent<NameComponent>().name;
	}

	const void Entity::SetName(const std::string& name)
	{
		GetComponent<NameComponent>().name = name;
	}

	UUID Entity::GetId()
	{
		return GetComponent<IDComponent>().ID;
	}

	glm::mat4 Entity::GetLocalTransformMatrix()
	{
		return GetComponent<TransformComponent>().GetTransformMatrix();
	}

	glm::mat4 Entity::GetGlobalTransformMatrix()
	{
		TransformComponent& localTransform = GetComponent<TransformComponent>();

		if (localTransform.cacheValid) return localTransform.chachedWorldSpaceMatrix;

		glm::mat4 worldSpaceMatrix(1.0f);

		if (HasParent())
			worldSpaceMatrix = GetParent().GetGlobalTransformMatrix();

		localTransform.chachedWorldSpaceMatrix = worldSpaceMatrix * GetLocalTransformMatrix();
		localTransform.cacheValid = true;

		return localTransform.chachedWorldSpaceMatrix;
	}

	bool Entity::HasParent()
	{
		return scene->IsIdValid(GetComponent<TransformComponent>().parent);
	}

	Entity Entity::GetParent()
	{
		return scene->GetEntityById(GetComponent<TransformComponent>().parent);
	}

	void Entity::SetParent(Entity parent)
	{
		TransformComponent& t = GetComponent<TransformComponent>();

		if (HasParent())
		{
			Entity currentParent = GetParent();
			if (currentParent == parent) return;

			currentParent.RemoveChild(*this);

			t.SetFromMatrix(currentParent.GetGlobalTransformMatrix() * t.GetTransformMatrix());
		}

		t.parent = parent.GetId();
		t.SetFromMatrix(glm::inverse(parent.GetGlobalTransformMatrix()) * t.GetTransformMatrix());

		std::vector<UUID>& parentChildren = parent.GetChildrenIds();

		UUID id = GetId();
		if (std::find(parentChildren.begin(), parentChildren.end(), id) == parentChildren.end())
			parentChildren.emplace_back(id);
		
	}

	void Entity::RemoveParent()
	{
		if (!HasParent()) return;

		Entity currentParent = GetParent();
		currentParent.RemoveChild(*this);

		TransformComponent& t = GetComponent<TransformComponent>();
		t.SetFromMatrix(currentParent.GetGlobalTransformMatrix() * t.GetTransformMatrix());
		t.parent = 0;
	}

	std::vector<UUID>& Entity::GetChildrenIds()
	{
		return GetComponent<TransformComponent>().children;
	}

	void Entity::AddChild(Entity e)
	{
		GetComponent<TransformComponent>().children.push_back(e.GetId());
	}

	void Entity::RemoveChild(Entity e)
	{
		UUID childId = e.GetId();

		std::vector<UUID>& children = GetChildrenIds();
		auto it = std::find(children.begin(), children.end(), childId);

		if (it != children.end())
			children.erase(it);
	}

	bool Entity::IsAncestorOf(Entity e)
	{
		const auto& children = GetChildrenIds();

		if (children.empty())
			return false;

		for (UUID child : children)
			if (child == e.GetId())
				return true;

		for (UUID child : children)
			if (scene->GetEntityById(child).IsAncestorOf(e))
				return true;

		return false;
	}

	bool Entity::IsDescendantOf(Entity e)
	{
		return e.IsAncestorOf(*this);
	}

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

			size_t metaTypeSize = metaType.GetSerializedDataSize((byte*)metaType.Get(*this));
			out.write((char*)&metaTypeSize, sizeof(size_t));

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

			size_t metaTypeSize = 0;
			in.read((char*)&metaTypeSize, sizeof(size_t));

			if (!Application::Get()->IsComponentRegistered(buffer))
			{
				std::cerr << "Serialized component '" << buffer << "' is not a registered component" << std::endl;

				in.seekg(metaTypeSize + sizeof(size_t), std::ios::cur);
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