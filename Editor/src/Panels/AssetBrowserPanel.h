#pragma once
#include <Seidon.h>
#include <filesystem>

namespace Seidon
{
	class AssetBrowserPanel
	{
	private:
		std::filesystem::path currentDirectory;
		Texture fileIcon, folderIcon;

	public:
		AssetBrowserPanel();
		void Draw();
	};
}