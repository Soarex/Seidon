#include "EditorPreferences.h"

#include <yaml-cpp/yaml.h>

namespace Seidon
{
	void EditorPreferences::Save(const std::string& path)
	{
		YAML::Emitter out;
		
		out << YAML::BeginMap << YAML::Key << "Project Records" << YAML::Value << YAML::BeginSeq;
		
		for (ProjectRecord& r : projectHistory)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << r.name;
			out << YAML::Key << "Path" << YAML::Value << r.path;
			out << YAML::EndMap;
		}

		out << YAML::EndSeq << YAML::EndMap;

		std::ofstream outStream(path);
		outStream << out.c_str();
	}

	void EditorPreferences::Load(const std::string& path)
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

		YAML::Node projectHistoryNodes = data["Project Records"];

		for (YAML::Node recordNode : projectHistoryNodes)
		{
			ProjectRecord r;

			r.name = recordNode["Name"].as<std::string>();
			r.path = recordNode["Path"].as<std::string>();

			projectHistory.push_back(r);
		}
	}
}