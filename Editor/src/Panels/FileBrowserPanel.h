#pragma once
#include <Seidon.h>
#include "Panel.h"

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

	class FileBrowserPanel : public Panel
	{
	public:
		FileBrowserPanel(Editor& editor) : Panel(editor) {}
		void Init() override;
		void Draw() override;

	private:
		AssetImporter importer;

		std::filesystem::path currentDirectory;

		Texture* fileIcon;
		Texture* folderIcon;
		Texture* modelIcon;
		Texture* materialIcon;
		Texture* animationIcon;
		Texture* skinnedMeshIcon;
		Texture* prefabIcon;
		Texture* fontIcon;
		Texture* colliderIcon;
		Texture* soundIcon;
		Texture* backIcon;

		std::vector<DirectoryEntry> currentDirectories;
		std::vector<FileEntry> currentFiles;


		float padding = 32.0f;
		float thumbnailSize = 90.0f;

	private:
		void UpdateEntries();

		void DrawDirectories();
		void DrawFiles();

		void DrawExternalImageFile(FileEntry& file);
		void DrawExternalModelFile(FileEntry& file);
		void DrawExternalCubemapFile(FileEntry& file);
		void DrawExternalFontFile(FileEntry& file);
		
		void DrawTextureFile(FileEntry& file);
		void DrawMeshFile(FileEntry& file);
		void DrawSkinnedMeshFile(FileEntry& file);
		void DrawMaterialFile(FileEntry& file);
		void DrawAnimationFile(FileEntry& file);
		void DrawCubemapFile(FileEntry& file);
		void DrawSceneFile(FileEntry& file);
		void DrawPrefabFile(FileEntry& file);
		void DrawShaderFile(FileEntry& file);
		void DrawFontFile(FileEntry& file);
		void DrawColliderFile(FileEntry& file);
		void DrawSoundFile(FileEntry& file);
		void DrawGenericFile(FileEntry& file);
	};
}