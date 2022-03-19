#pragma once
#include <Seidon.h>
#include <filesystem>
#include <vector>

namespace Seidon
{
	struct DirectoryEntry
	{
		std::string name;
		std::string path;
	};

	struct FileEntry
	{
		std::string name;
		std::string path;
		std::string extension;
	};

	class FileBrowserPanel
	{
	private:
		AssetImporter importer;

		std::filesystem::path currentDirectory;

		Texture* fileIcon;
		Texture* folderIcon;
		Texture* modelIcon;
		Texture* materialIcon;
		Texture* animationIcon;
		Texture* armatureIcon;
		Texture* backIcon;

		std::vector<DirectoryEntry> currentDirectories;
		std::vector<FileEntry> currentFiles;


		float padding = 32.0f;
		float thumbnailSize = 90.0f;

	public:
		FileBrowserPanel() = default;
		void Init();
		void Draw();

	private:
		void UpdateEntries();

		void DrawDirectories();
		void DrawFiles();

		void DrawExternalImageFile(FileEntry& file);
		void DrawExternalModelFile(FileEntry& file);
		void DrawExternalCubemapFile(FileEntry& file);
		
		void DrawTextureFile(FileEntry& file);
		void DrawMeshFile(FileEntry& file);
		void DrawMaterialFile(FileEntry& file);
		void DrawArmatureFile(FileEntry& file);
		void DrawAnimationFile(FileEntry& file);
		void DrawCubemapFile(FileEntry& file);
		void DrawSceneFile(FileEntry& file);
		void DrawGenericFile(FileEntry& file);
	};
}