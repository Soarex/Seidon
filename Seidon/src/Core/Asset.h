#pragma once
#include "UUID.h"

#include <string>
#include <fstream>

namespace Seidon
{
	struct Asset
	{
		UUID id;
		std::string name;
		std::string path;

		virtual void Save(std::ofstream& out) = 0;
		virtual void Load(std::ifstream& in) = 0;

		virtual void Save(const std::string& path)
		{
			std::ofstream out(path, std::ios::out | std::ios::binary);
			Save(out);
		}

		virtual void Load(const std::string& path)
		{
			std::ifstream in(path, std::ios::in | std::ios::binary);
			Load(in);
		}
	};
}