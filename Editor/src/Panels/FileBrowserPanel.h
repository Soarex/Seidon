#pragma once
#include <Seidon.h>
#include <filesystem>

namespace Seidon
{
	class FileBrowserPanel
	{
	private:
		std::filesystem::path currentDirectory;
		Texture* fileIcon;
		Texture* folderIcon;
		Texture* modelIcon;
		Texture* materialIcon;
		Texture* backIcon;

	public:
		FileBrowserPanel() = default;
		void Init();
		void Draw();
	};
}