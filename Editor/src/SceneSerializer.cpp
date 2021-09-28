#include "SceneSerializer.h"

namespace YAML {

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}

namespace Seidon
{
	SceneSerializer::SceneSerializer()
	{
		AddSerializeFunction<NameComponent>([](YAML::Emitter& out, Entity& entity)
			{
				out << YAML::Key << "Name Component";
				out << YAML::BeginMap;

				NameComponent& nameComponent = entity.GetComponent<NameComponent>();
				out << YAML::Key << "Name" << YAML::Value << nameComponent.name;

				out << YAML::EndMap;
			});

		AddDeserializeFunction<NameComponent>([](YAML::Node& node, Entity& entity)
			{
				entity.GetComponent<NameComponent>().name = node["Name"].as<std::string>();
			});

		AddSerializeFunction<TransformComponent>([](YAML::Emitter& out, Entity& entity)
			{
				out << YAML::Key << "Transform Component";
				out << YAML::BeginMap;

				TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();
				out << YAML::Key << "Position" << YAML::Value << transformComponent.position;
				out << YAML::Key << "Rotation" << YAML::Value << transformComponent.rotation;
				out << YAML::Key << "Scale" << YAML::Value << transformComponent.scale;

				out << YAML::EndMap;
			});

		AddDeserializeFunction<TransformComponent>([](YAML::Node& node, Entity& entity)
			{
				TransformComponent& transform = entity.GetComponent<TransformComponent>();
				transform.position = node["Position"].as<glm::vec3>();
				transform.rotation = node["Rotation"].as<glm::vec3>();
				transform.scale = node["Scale"].as<glm::vec3>();
			});

		AddSerializeFunction<CameraComponent>([](YAML::Emitter& out, Entity& entity)
			{
				out << YAML::Key << "Camera Component";
				out << YAML::BeginMap;

				CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();
				out << YAML::Key << "Aspect Ratio" << YAML::Value << cameraComponent.aspectRatio;
				out << YAML::Key << "Exposure" << YAML::Value << cameraComponent.exposure;
				out << YAML::Key << "Near Plane" << YAML::Value << cameraComponent.nearPlane;
				out << YAML::Key << "Far Plane" << YAML::Value << cameraComponent.farPlane;
				out << YAML::Key << "Fov" << YAML::Value << cameraComponent.fov;
				out << YAML::Key << "Perspective" << YAML::Value << cameraComponent.perspective;

				out << YAML::EndMap;
			});

		AddDeserializeFunction<CameraComponent>([](YAML::Node& node, Entity& entity)
			{
				CameraComponent& camera = entity.AddComponent<CameraComponent>();

				camera.aspectRatio = node["Aspect Ratio"].as<float>();
				camera.exposure = node["Exposure"].as<float>();
				camera.nearPlane = node["Near Plane"].as<float>();
				camera.farPlane = node["Far Plane"].as<float>();
				camera.fov = node["Fov"].as<float>();
				camera.perspective = node["Perspective"].as<bool>();
			});

		AddSerializeFunction<DirectionalLightComponent>([](YAML::Emitter& out, Entity& entity)
			{
				out << YAML::Key << "Directional Light Component";
				out << YAML::BeginMap;

				DirectionalLightComponent& directionalLightComponent = entity.GetComponent<DirectionalLightComponent>();
				out << YAML::Key << "Color" << YAML::Value << directionalLightComponent.color;
				out << YAML::Key << "Intensity" << YAML::Value << directionalLightComponent.intensity;

				out << YAML::EndMap;
			});

		AddDeserializeFunction<DirectionalLightComponent>([](YAML::Node& node, Entity& entity)
			{
				DirectionalLightComponent& light = entity.AddComponent<DirectionalLightComponent>();

				light.color = node["Color"].as<glm::vec3>();
				light.intensity = node["Intensity"].as<float>();
			});

		AddSerializeFunction<CubemapComponent>([](YAML::Emitter& out, Entity& entity)
			{
				out << YAML::Key << "Cubemap Component";
				out << YAML::BeginMap;

				CubemapComponent& cubemapComponent = entity.GetComponent<CubemapComponent>();
				out << YAML::Key << "Path" << YAML::Value << cubemapComponent.cubemap->GetPath();

				out << YAML::EndMap;
			});

		AddDeserializeFunction<CubemapComponent>([](YAML::Node& node, Entity& entity)
			{
				CubemapComponent& cubemap = entity.AddComponent<CubemapComponent>();

				cubemap.cubemap = new HdrCubemap();
				cubemap.cubemap->LoadFromEquirectangularMap(node["Path"].as<std::string>());
			});

		AddSerializeFunction<RenderComponent>([](YAML::Emitter& out, Entity& entity)
			{
				out << YAML::Key << "Render Component";
				out << YAML::BeginMap;

				RenderComponent& renderComponent = entity.GetComponent<RenderComponent>();
				out << YAML::Key << "Path" << YAML::Value << renderComponent.mesh->filepath;
				out << YAML::Key << "Name" << YAML::Value << renderComponent.mesh->name;

				out << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;

				out << YAML::BeginMap;
				for (SubMesh* submesh : renderComponent.mesh->subMeshes)
				{
					Material* material = submesh->material;
					out << YAML::Key << "Name" << material->name;
					out << YAML::Key << "Tint" << material->tint;
					out << YAML::Key << "Albedo Path" << material->albedo->path;
					out << YAML::Key << "Roughness Path" << material->roughness->path;
					out << YAML::Key << "Normal Path" << material->normal->path;
					out << YAML::Key << "Metallic Path" << material->metallic->path;
					out << YAML::Key << "Ao Path" << material->ao->path;
				}
				out << YAML::EndMap;

				out << YAML::EndSeq;
				out << YAML::EndMap;
			});

		AddDeserializeFunction<RenderComponent>([](YAML::Node& node, Entity& entity)
			{
				RenderComponent& renderComponent = entity.AddComponent<RenderComponent>();
				std::string meshName = node["Name"].as<std::string>();
				std::string modelPath = node["Path"].as<std::string>();

				if (!ResourceManager::IsMeshLoaded(meshName))
				{
					ModelImporter importer;
					ModelImportData importData = importer.Import("Assets/untitled.fbx");
					ResourceManager::CreateFromImportData(importData);

				}
				
				renderComponent.mesh = ResourceManager::GetMesh(meshName);
			});
	}

	void SceneSerializer::Save(Scene* scene, const std::string& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << scene->GetName();
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		scene->GetRegistry().each([&](auto entityID)
			{
				Entity entity = { entityID, &scene->GetRegistry() };
				SerializeEntity(out, entity);
			});
		out << YAML::EndSeq;

		out << YAML::Key << "Systems";

		out << YAML::BeginMap;

		if (scene->HasSystem<RenderSystem>())
		{
			out << YAML::Key << "Render System" ;
			out << YAML::BeginMap;
			out << YAML::EndMap;
		}

		if (scene->HasSystem<EditorCameraControlSystem>())
		{
			auto& editorCamera = scene->GetSystem<EditorCameraControlSystem>();
			out << YAML::Key << "Editor Camera Control System";
			out << YAML::BeginMap;
			out << YAML::Key << "Movement Speed" << YAML::Value << editorCamera.movementSpeed;
			out << YAML::Key << "Pan Speed" << YAML::Value << editorCamera.panSpeed;
			out << YAML::Key << "Mouse Sensitivity" << YAML::Value << editorCamera.mouseSensitivity;
			out << YAML::Key << "Scrollwheel Sensitivy" << YAML::Value << editorCamera.scrollWheelSensitivity;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
		out << YAML::EndMap;

		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity& entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "ID" << YAML::Value << (int)entity.ID;

		SerializeComponent<NameComponent>(out, entity);
		SerializeComponent<TransformComponent>(out, entity);

		if (entity.HasComponent<CameraComponent>())
			SerializeComponent<CameraComponent>(out, entity);

		if (entity.HasComponent<DirectionalLightComponent>())
			SerializeComponent<DirectionalLightComponent>(out, entity);

		if (entity.HasComponent<CubemapComponent>())
			SerializeComponent<CubemapComponent>(out, entity);

		if (entity.HasComponent<RenderComponent>())
			SerializeComponent<RenderComponent>(out, entity);

		out << YAML::EndMap;
	}

	Scene* SceneSerializer::Load(const std::string& path)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path);
		}
		catch (YAML::ParserException e)
		{
			std::cout << e.msg << std::endl;
			return nullptr;
		}

		if (!data["Scene"])
		{
			std::cout << "Scene not found" << std::endl;
			return nullptr;
		}


		std::string sceneName = data["Scene"].as<std::string>();
		Scene* scene = new Scene(sceneName);

		YAML::Node entities = data["Entities"];
		if (entities)
			for (auto entity : entities)
				DeserializeEntity(entity, scene);

		YAML::Node systems = data["Systems"];

		if(systems)
		{
			YAML::Node renderSystemNode = systems["Render System"];

			if (renderSystemNode)
				scene->AddSystem<RenderSystem>();

			YAML::Node cameraNode = systems["Editor Camera Control System"];

			if (cameraNode)
			{
				auto& editorCamera = scene->AddSystem<EditorCameraControlSystem>();
				editorCamera.movementSpeed = cameraNode["Movement Speed"].as<float>();
				editorCamera.panSpeed = cameraNode["Pan Speed"].as<float>();
				editorCamera.mouseSensitivity = cameraNode["Mouse Sensitivity"].as<float>();
				editorCamera.scrollWheelSensitivity = cameraNode["Scrollwheel Sensitivy"].as<float>();
			}
		}

		return scene;
	}

	void SceneSerializer::DeserializeEntity(YAML::Node& node, Scene* scene)
	{
		Entity entity = scene->CreateEntity();

		YAML::Node nameNode = node["Name Component"];
		if (nameNode)
			DeserializeComponent<NameComponent>(nameNode, entity);

		YAML::Node transformNode = node["Transform Component"];
		if (transformNode)
			DeserializeComponent<TransformComponent>(transformNode, entity);


		YAML::Node cameraNode = node["Camera Component"];
		if (cameraNode)
			DeserializeComponent<CameraComponent>(cameraNode, entity);

		YAML::Node cubemapNode = node["Cubemap Component"];
		if (cubemapNode)
			DeserializeComponent<CubemapComponent>(cubemapNode, entity);

		YAML::Node lightNode = node["Directional Light Component"];
		if (lightNode)
			DeserializeComponent<DirectionalLightComponent>(lightNode, entity);

		YAML::Node renderNode = node["Render Component"];
		if (renderNode)
			DeserializeComponent<RenderComponent>(renderNode, entity);

	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
}