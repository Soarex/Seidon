#include "Project.h"

#include <yaml-cpp/yaml.h>
#include <filesystem>

namespace Seidon
{
	void Project::AddEditorSystem(SystemMetaType& metatype)
	{
		System* system = metatype.Instantiate();

		system->scene = loadedScene;
		system->SysInit();

		editorSystems[metatype.name] = system;
	}

	void Project::RemoveEditorSystem(SystemMetaType& metatype)
	{
		System* system = editorSystems[metatype.name];

		system->SysDestroy();

		delete system;

		editorSystems.erase(metatype.name);
	}

	void Project::Save(const std::string& path)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		
		out << YAML::Key << "Id" << YAML::Value << id;
		out << YAML::Key << "Name" << YAML::Value << name;
		out << YAML::Key << "Path" << YAML::Value << path;
		out << YAML::Key << "Root Directory" << YAML::Value << rootDirectory;
		out << YAML::Key << "Assets Directory" << YAML::Value << assetsDirectory;

		out << YAML::Key << "Scene Name" << YAML::Value << loadedScene->name;
		out << YAML::Key << "Scene Id" << YAML::Value << loadedScene->id;

		out << YAML::Key << "Extensions" << YAML::Value;
		out << YAML::BeginSeq;

		for (Extension* extension : loadedExtensions)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << extension->name;
			out << YAML::Key << "Id" << YAML::Value << extension->id;
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;

		out << YAML::Key << "Editor Systems" << YAML::Value;
		out << YAML::BeginSeq;

		for (auto& [name, system] : editorSystems)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << name;
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;

		out << YAML::EndMap;

		std::ofstream outStream(path);
		outStream << out.c_str();

		std::ofstream fout(assetsDirectory + "\\ResourceRegistry.sdreg", std::ios::out | std::ios::binary);
		Application::Get()->GetResourceManager()->Save(fout);
	}

	void Project::Load(const std::string& path)
	{
		YAML::Node data;

		try
		{
			data = YAML::LoadFile(path);
		}
		catch (YAML::ParserException e)
		{
			std::cerr << e.msg << std::endl;
			return;
		}

		id = data["Id"].as<uint64_t>();
		name = data["Name"].as<std::string>();
		this->path = data["Path"].as<std::string>();

		rootDirectory = data["Root Directory"].as<std::string>();
		assetsDirectory = data["Assets Directory"].as<std::string>();

		if (std::filesystem::exists(assetsDirectory + "\\ResourceRegistry.sdreg"))
		{
			std::ifstream in(assetsDirectory + "\\ResourceRegistry.sdreg", std::ios::in | std::ios::binary);
			Application::Get()->GetResourceManager()->Load(in);

		}

		Application::Get()->GetResourceManager()->SetAssetDirectory(assetsDirectory);

		for (YAML::Node extensionNode : data["Extensions"])
		{
			UUID id = extensionNode["Id"].as<uint64_t>();

			if (!Application::Get()->GetResourceManager()->IsAssetRegistered(id))
			{
				std::cerr << "Error loading extension " << extensionNode["Name"].as<std::string>() << ", id not registered" << std::endl;
				continue;
			}
			
			std::string extensionPath = Application::Get()->GetResourceManager()->GetAssetPath(id);

			if (!std::filesystem::exists(extensionPath))
			{
				std::cerr << "Error loading extension " << extensionNode["Name"].as<std::string>() << ", file not found" << std::endl;
				continue;
			}
			
			std::string hotswapPath = rootDirectory
				+ "\\Temp\\Hotswap\\" + GetNameFromPath(extensionPath);

			std::filesystem::copy_file(extensionPath, hotswapPath, std::filesystem::copy_options::overwrite_existing);


			Extension* extension = new Extension();
			extension->Load(hotswapPath);
			extension->id = id;

			loadedExtensions.push_back(extension);
		}

		loadedScene = new Scene();

		UUID sceneId = data["Scene Id"].as<uint64_t>();

		if (Application::Get()->GetResourceManager()->IsAssetRegistered(sceneId))
		{
			std::string scenePath = Application::Get()->GetResourceManager()->GetAssetPath(sceneId);
			loadedScene->Load(scenePath);
		}
		else
		{
			std::cerr << "Scene id not valid, replacing with new scene" << std::endl;

			loadedScene->AddSystem<RenderSystem>();
			loadedScene->AddSystem<PhysicSystem>();

			loadedScene->Save(assetsDirectory + "\\Scenes\\Default Scene.sdscene");

			Application::Get()->GetResourceManager()->RegisterAsset(loadedScene, assetsDirectory + "\\Scenes\\Default Scene.sdscene");
		}


		for (YAML::Node systemNode : data["Editor Systems"])
		{
			std::string systemName = systemNode["Name"].as<std::string>();

			if (!Application::Get()->IsSystemRegistered(systemName))
			{
				std::cerr << name << " is not a registered system, make sure that the relative extension is loaded and try again" << std::endl;
				continue;
			}

			SystemMetaType metatype = Application::Get()->GetSystemMetaTypeByName(systemName);
			AddEditorSystem(metatype);
		}

	}
}