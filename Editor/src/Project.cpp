#include "Project.h"

#include <yaml-cpp/yaml.h>

namespace Seidon
{
	void Project::Save(const std::string& path)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		
		out << YAML::Key << "Id" << YAML::Value << id;
		out << YAML::Key << "Name" << YAML::Value << name;
		out << YAML::Key << "Path" << YAML::Value << path;
		out << YAML::Key << "Root Directory" << YAML::Value << rootDirectory;
		out << YAML::Key << "Assets Directory" << YAML::Value << assetsDirectory;
		out << YAML::Key << "Scene Path" << YAML::Value << assetsDirectory + "\\Scenes\\" + currentScene->name + ".sdscene";

		out << YAML::EndMap;

		std::ofstream outStream(path);
		outStream << out.c_str();
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

		currentScene = new Scene();
		currentScene->Load(data["Scene Path"].as<std::string>());
	}
}