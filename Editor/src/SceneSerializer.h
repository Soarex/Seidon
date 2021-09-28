#pragma once
#include <Seidon.h>
#include <yaml-cpp/yaml.h>
#include "EditorCameraControlSystem.h"

namespace Seidon
{
	typedef std::function<void(YAML::Emitter&, Entity&)> ComponentSerializeFunction;
	typedef std::function<void(YAML::Node&, Entity&)> ComponentDeserializeFunction;

	class SceneSerializer
	{
	private:
		std::map<std::string, ComponentSerializeFunction> componentSerializeFunctions;
		std::map<std::string, ComponentDeserializeFunction> componentDeserializeFunctions;
	public:
		SceneSerializer();

		void Save(Scene* scene, const std::string& path);
		Scene* Load(const std::string& path);

		template <typename T>
		void AddSerializeFunction(const ComponentSerializeFunction& function)
		{
			componentSerializeFunctions[typeid(T).name()] = function;
		}

		template <typename T>
		void AddDeserializeFunction(const ComponentDeserializeFunction& function)
		{
			componentDeserializeFunctions[typeid(T).name()] = function;
		}

	private:
		void SerializeEntity(YAML::Emitter& out, Entity& entity);
		void DeserializeEntity(YAML::Node& node, Scene* scene);

		template <typename T>
		void SerializeComponent(YAML::Emitter& out, Entity& entity)
		{
			componentSerializeFunctions[typeid(T).name()](out, entity);
		}

		template <typename T>
		void DeserializeComponent(YAML::Node& node, Entity& entity)
		{
			componentDeserializeFunctions[typeid(T).name()](node, entity);
		}
	};

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v);
}