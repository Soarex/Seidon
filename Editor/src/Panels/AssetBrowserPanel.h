#pragma once
#include <Seidon.h>
#include <filesystem>

namespace Seidon
{
	class AssetBrowserPanel
	{
	private:
		std::filesystem::path currentDirectory;
		Texture fileIcon, folderIcon, modelIcon, materialIcon;

	public:
		AssetBrowserPanel() = default;
		void Init();
		void Draw();
	};
}