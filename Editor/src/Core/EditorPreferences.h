#pragma once
#include <Seidon.h>

namespace Seidon
{
	struct ProjectRecord
	{
		std::string name;
		std::string path;

		bool operator ==(const ProjectRecord& other) { return path == other.path; }
	};

	struct EditorPreferences : public Asset
	{
		std::vector<ProjectRecord> projectHistory;

		void Save(const std::string& path);
		void Load(const std::string& path);

		void Save(std::ofstream& out) {}
		void Load(std::ifstream& in) {}
	};
}