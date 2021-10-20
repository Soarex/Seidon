#include "AssetBrowserPanel.h"

namespace Seidon
{
	extern const std::filesystem::path assetsPath = "Assets";

	void AssetBrowserPanel::Init()
	{
		fileIcon.LoadFromFileAsync("Assets/FileIcon.png");
		folderIcon.LoadFromFileAsync("Assets/FolderIcon.png");

		currentDirectory = assetsPath;
	}

	void AssetBrowserPanel::Draw()
	{
		ImGui::Begin("Content Browser");

		if (currentDirectory != std::filesystem::path(assetsPath) && ImGui::Button("<-"))
			currentDirectory = currentDirectory.parent_path();

		static float padding = 32.0f;
		static float thumbnailSize = 90.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;

		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory))
		{
			if (!directoryEntry.is_directory()) continue;
			const std::filesystem::path& path = directoryEntry.path();

			const std::filesystem::path& relativePath = std::filesystem::relative(path, assetsPath);
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)folderIcon.GetId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					currentDirectory /= path.filename();

			ImGui::TextWrapped(filenameString.c_str());
			ImGui::NextColumn();

			ImGui::PopID();
		}

		for (auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory))
		{
			if (directoryEntry.is_directory()) continue;

			const std::filesystem::path& path = directoryEntry.path();

			const std::filesystem::path& relativePath = std::filesystem::relative(path, assetsPath);
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			if (path.extension() == ".png" || path.extension() == ".jpg")
			{
				ImGui::ImageButton((ImTextureID)Application::Get()->GetResourceManager()->LoadTexture(directoryEntry.path().string())->GetId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::BeginDragDropSource())
				{
					const std::string& itemPath = directoryEntry.path().string();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_TEXTURE", itemPath.c_str(), itemPath.length() + 1);
					ImGui::EndDragDropSource();
				}

			}
			else
			{
				ImGui::ImageButton((ImTextureID)fileIcon.GetId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (path.extension() == ".sdscene" && ImGui::BeginDragDropSource())
				{
					const std::string& itemPath = directoryEntry.path().string();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_SCENE", itemPath.c_str(), itemPath.length() + 1);
					ImGui::EndDragDropSource();
				}

				if (path.extension() == ".fbx" && ImGui::BeginDragDropSource())
				{
					const std::string& itemPath = directoryEntry.path().string();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_MODEL", itemPath.c_str(), itemPath.length() + 1);
					ImGui::EndDragDropSource();
				}
			}

			
			ImGui::PopStyleColor();

			ImGui::TextWrapped(filenameString.c_str());
			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::End();
	}
}