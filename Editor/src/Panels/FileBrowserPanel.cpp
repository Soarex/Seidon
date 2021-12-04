#include "FileBrowserPanel.h"

#include "../Editor.h"

namespace Seidon
{
	extern const std::filesystem::path assetsPath = "Assets";

	void FileBrowserPanel::Init()
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;

		backIcon = resourceManager.ImportTexture("Resources/BackIcon.png");
		fileIcon = resourceManager.ImportTexture("Resources/FileIcon.png");
		folderIcon = resourceManager.ImportTexture("Resources/FolderIcon.png");
		modelIcon = resourceManager.ImportTexture("Resources/ModelIcon.png");
		materialIcon = resourceManager.ImportTexture("Resources/MaterialIcon.png");

		currentDirectory = assetsPath;
	}

	void FileBrowserPanel::Draw()
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		ImGui::Begin("File Browser");


		static float padding = 32.0f;
		static float thumbnailSize = 90.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;

		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		if (currentDirectory != std::filesystem::path(assetsPath))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			if(ImGui::ImageButton((ImTextureID)backIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }))
				currentDirectory = currentDirectory.parent_path();

			ImGui::PopStyleColor();

			ImGui::TextWrapped("Back");
			ImGui::NextColumn();
		}

		for (auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory))
		{
			if (!directoryEntry.is_directory()) continue;
			const std::filesystem::path& path = directoryEntry.path();

			std::string filenameString = path.filename().string();

			ImGui::PushID(filenameString.c_str());

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)folderIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

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

			std::string filenameString = path.filename().string();
			ImGui::PushID(filenameString.c_str());

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			if (path.extension() == ".png" || path.extension() == ".jpg")
			{
				if (ImGui::ImageButton((ImTextureID)resourceManager.ImportTexture(directoryEntry.path().string())->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }))
					ImGui::OpenPopup("Texture Import Popup");
				
				if (ImGui::BeginDragDropSource())
				{
					const std::string& itemPath = directoryEntry.path().string();
					ImGui::SetDragDropPayload("FILE_BROWSER_TEXTURE", itemPath.c_str(), itemPath.length() + 1);
					ImGui::EndDragDropSource();
				}

				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

				if (ImGui::BeginPopupModal("Texture Import Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Import texture file?");

					ImGui::Separator();

					static bool gammaCorrection;
					ImGui::Checkbox("Gamma correction", &gammaCorrection);

					if (ImGui::Button("Import"))
					{
						Application::Get()->GetResourceManager()->ImportTexture(directoryEntry.path().string(), gammaCorrection);
						ImGui::CloseCurrentPopup();
					}

					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
						ImGui::CloseCurrentPopup();

					ImGui::EndPopup();
				}

				ImGui::TextWrapped(filenameString.c_str());
				ImGui::NextColumn();
			}
			else if (path.extension() == ".hdr")
			{
				if(ImGui::ImageButton((ImTextureID)fileIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }))
					ImGui::OpenPopup("Cubemap Import Popup");

				if (ImGui::BeginDragDropSource())
				{
					const std::string& itemPath = directoryEntry.path().string();
					ImGui::SetDragDropPayload("FILE_BROWSER_CUBEMAP", itemPath.c_str(), itemPath.length() + 1);
					ImGui::EndDragDropSource();
				}

				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

				if (ImGui::BeginPopupModal("Cubemap Import Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Import cubemap file?");

					ImGui::Separator();

					if (ImGui::Button("Import"))
					{
						Application::Get()->GetResourceManager()->ImportCubemap(directoryEntry.path().string());
						ImGui::CloseCurrentPopup();
					}

					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
						ImGui::CloseCurrentPopup();

					ImGui::EndPopup();
				}

				ImGui::TextWrapped(filenameString.c_str());
				ImGui::NextColumn();
			}
			else if (path.extension() == ".fbx")
			{
				bool clicked = false;
				if (ImGui::ImageButton((ImTextureID)modelIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }))
					ImGui::OpenPopup("Model Import Popup");

				if (ImGui::BeginDragDropSource())
				{
					const std::string& itemPath = directoryEntry.path().string();
					ImGui::SetDragDropPayload("FILE_BROWSER_MODEL", itemPath.c_str(), itemPath.length() + 1);
					ImGui::EndDragDropSource();
				}

				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

				if (ImGui::BeginPopupModal("Model Import Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Import model file?");
						
					ImGui::Separator();

					if (ImGui::Button("Import"))
					{
						ModelImporter importer;
						ModelImportData importData = importer.Import(directoryEntry.path().string());
						
						Application::Get()->GetResourceManager()->CreateFromImportData(importData);
						ImGui::CloseCurrentPopup();
					}

					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
						ImGui::CloseCurrentPopup();

					ImGui::EndPopup();
				}

				ImGui::TextWrapped(filenameString.c_str());
				ImGui::NextColumn();

			}
			else
			{
				ImGui::ImageButton((ImTextureID)fileIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (path.extension() == ".sdscene" && ImGui::BeginDragDropSource())
				{
					const std::string& itemPath = directoryEntry.path().string();
					ImGui::SetDragDropPayload("FILE_BROWSER_SCENE", itemPath.c_str(), itemPath.length() + 1);
					ImGui::EndDragDropSource();
				}
				ImGui::TextWrapped(filenameString.c_str());
				ImGui::NextColumn();
			}


			ImGui::PopStyleColor();

			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::End();
	}
}