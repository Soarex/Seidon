#pragma once
#include <Seidon.h>

namespace Seidon
{
	struct Project : public Asset
	{
		std::string rootDirectory;
		std::string assetsDirectory;

		std::vector<Extension> loadedExtensions;
		Scene* currentScene;

		void Save(const std::string& path);
		void Load(const std::string& path);

		void Save(std::ofstream& out) {}
		void Load(std::ifstream& in) {}
	};
}