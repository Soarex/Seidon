#include "FileBrowserPanel.h"

#include "../Editor.h"

namespace Seidon
{
	extern const std::filesystem::path assetsPath = "Assets";

	void FileBrowserPanel::Init()
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;

		backIcon = resourceManager.LoadTexture("Resources/BackIcon.sdtex");
		fileIcon = resourceManager.LoadTexture("Resources/FileIcon.sdtex");
		folderIcon = resourceManager.LoadTexture("Resources/FolderIcon.sdtex");
		modelIcon = resourceManager.LoadTexture("Resources/ModelIcon.sdtex");
		materialIcon = resourceManager.LoadTexture("Resources/MaterialIcon.sdtex");
		animationIcon = resourceManager.LoadTexture("Resources/AnimationIcon.sdtex");
		armatureIcon = resourceManager.LoadTexture("Resources/ArmatureIcon.sdtex"); 
		prefabIcon = resourceManager.LoadTexture("Resources/PrefabIcon.sdtex");


		currentDirectory = assetsPath;
		UpdateEntries();
	}

	void FileBrowserPanel::Draw()
	{
		ResourceManager& resourceManager = *Application::Get()->GetResourceManager();

		if (!ImGui::Begin("File Browser"))
		{
			ImGui::End();
			return;
		}

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create New Material"))
			{
				Material m;
				if (std::filesystem::exists(currentDirectory.string() + "\\New Material.sdmat"))
				{
					int i = 1;
					while (std::filesystem::exists(currentDirectory.string() + "\\" + "New Material (" + std::to_string(i) + ").sdmat"))
						i++;

					m.name = "New Material (" + std::to_string(i) + ")";
				}
				else
					m.name = "New Material";

				m.Save(currentDirectory.string() + "\\" + m.name + ".sdmat");

				resourceManager.RegisterMaterial(&m, currentDirectory.string() + "\\" + m.name + ".sdmat");

				UpdateEntries();
			}

			if (ImGui::MenuItem("Refresh"))
				UpdateEntries();

			ImGui::EndPopup();
		}

		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;

		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		if (currentDirectory != std::filesystem::path(assetsPath))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			if (ImGui::ImageButton((ImTextureID)backIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }))
			{
				currentDirectory = currentDirectory.parent_path();
				UpdateEntries();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_MATERIAL"))
				{
					std::filesystem::path currentPath = (const char*)payload->Data;
					std::filesystem::path newPath = currentDirectory.parent_path() / currentPath.filename();

					std::filesystem::rename(currentPath, newPath);

					ResourceManager& resourceManager = *Application::Get()->GetResourceManager();

					UpdateEntries();
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::PopStyleColor();

			ImGui::TextWrapped("Back");
			ImGui::NextColumn();
		}

		DrawDirectories();
		DrawFiles();

		ImGui::Columns(1);

		ImGui::End();
	}

	void FileBrowserPanel::UpdateEntries()
	{
		currentDirectories.clear();
		currentFiles.clear();

		for (auto& entry : std::filesystem::directory_iterator(currentDirectory))
		{
			if (entry.is_directory())
			{
				DirectoryEntry directory;
				directory.path = entry.path().string();
				directory.name = entry.path().filename().string();

				currentDirectories.push_back(directory);
			}
			else
			{
				FileEntry file;
				file.path = entry.path().string();
				file.name = entry.path().filename().string();
				file.extension = entry.path().extension().string();

				currentFiles.push_back(file);
			}
		}
	}

	void FileBrowserPanel::DrawDirectories()
	{
		for (DirectoryEntry& directory : currentDirectories)
		{
			ImGui::PushID(directory.name.c_str());

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)folderIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				currentDirectory /= directory.name;
				UpdateEntries();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_MATERIAL"))
				{
					std::filesystem::path currentPath = (const char*)payload->Data;
					std::filesystem::path newPath = currentDirectory / directory.name / currentPath.filename();

					std::filesystem::rename(currentPath, newPath);

					ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
					UpdateEntries();
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::TextWrapped(directory.name.c_str());
			ImGui::NextColumn();

			ImGui::PopID();
		}
	}

	void FileBrowserPanel::DrawFiles()
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		for (FileEntry& file : currentFiles)
		{
			ImGui::PushID(file.name.c_str());

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			if (file.extension == ".png" || file.extension == ".jpg")
				DrawExternalImageFile(file);

			else if (file.extension == ".hdr")
				DrawExternalCubemapFile(file);

			else if (file.extension == ".fbx")
				DrawExternalModelFile(file);

			else if (file.extension == ".sdtex")
				DrawTextureFile(file);

			else if (file.extension == ".sdmesh")
				DrawMeshFile(file);

			else if (file.extension == ".sdmat")
				DrawMaterialFile(file);

			else if (file.extension == ".sdhdr")
				DrawCubemapFile(file);

			else if (file.extension == ".sdarm")
				DrawArmatureFile(file);

			else if (file.extension == ".sdanim")
				DrawAnimationFile(file);

			else if (file.extension == ".sdscene")
				DrawSceneFile(file);

			else if (file.extension == ".sdpref")
				DrawPrefabFile(file);

			else if (file.extension == ".sdshader")
				DrawShaderFile(file);

			else
				DrawGenericFile(file);


			ImGui::PopStyleColor();

			ImGui::PopID();
		}
	}

	void FileBrowserPanel::DrawExternalImageFile(FileEntry& file)
	{
		if (ImGui::ImageButton((ImTextureID)fileIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }))
			ImGui::OpenPopup("Texture Import");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Texture Import", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Import texture file?");

			ImGui::Separator();

			static bool gammaCorrection;
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Gamma correction: ");
			ImGui::SameLine();
			ImGui::Checkbox("##Gamma correction", &gammaCorrection);

			if (ImGui::Button("Import"))
			{
				importer.ImportTexture(file.path, gammaCorrection);
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawExternalModelFile(FileEntry& file)
	{
		bool clicked = false;
		if (ImGui::ImageButton((ImTextureID)fileIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }))
			ImGui::OpenPopup("Model Import");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Model Import", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Import model file?");

			ImGui::Separator();

			if (ImGui::Button("Import"))
			{
				importer.ImportModelFile(file.path);
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawExternalCubemapFile(FileEntry& file)
	{
		if (ImGui::ImageButton((ImTextureID)fileIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }))
			ImGui::OpenPopup("Cubemap Import");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Cubemap Import", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Import cubemap file?");

			ImGui::Separator();

			if (ImGui::Button("Import"))
			{
				importer.ImportCubemap(file.path);
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawTextureFile(FileEntry& file)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;

		Texture* t = resourceManager.GetOrLoadTexture(file.path);
		ImGui::ImageButton((ImTextureID)t->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = file.path;
			ImGui::SetDragDropPayload("FILE_BROWSER_TEXTURE", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawMeshFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)modelIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = file.path;
			ImGui::SetDragDropPayload("FILE_BROWSER_MESH", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawMaterialFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)materialIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			ImGui::OpenPopup("Rename");

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = file.path;
			ImGui::SetDragDropPayload("FILE_BROWSER_MATERIAL", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}
		/*
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::Selectable("Rename ###Test"))
			{
				ImGui::OpenPopup("RenameP");
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		*/
		if (ImGui::BeginPopupModal("Rename", 0, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Name: ");
			ImGui::SameLine();

			static char buffer[512];
			ImGui::InputText("##NewName", buffer, 512);
			
			ImGuiStyle& style = ImGui::GetStyle();
			float size = ImGui::CalcTextSize("Rename").x + style.FramePadding.x * 2.0f + ImGui::CalcTextSize("Close").x + style.FramePadding.x * 2.0f;

			float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
			if (ImGui::Button("Rename"))
			{
				std::filesystem::rename(file.path, currentDirectory / (std::string(buffer) + ".sdmat"));
				UpdateEntries();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			
			ImGui::EndPopup();
		}
		
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawArmatureFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)armatureIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = file.path;
			ImGui::SetDragDropPayload("FILE_BROWSER_ARMATURE", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawAnimationFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)animationIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = file.path;
			ImGui::SetDragDropPayload("FILE_BROWSER_ANIMATION", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawCubemapFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)fileIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = file.path;
			ImGui::SetDragDropPayload("FILE_BROWSER_CUBEMAP", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawSceneFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)fileIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = file.path;
			ImGui::SetDragDropPayload("FILE_BROWSER_SCENE", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawPrefabFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)prefabIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			ImGui::OpenPopup("Rename");

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = file.path;
			ImGui::SetDragDropPayload("FILE_BROWSER_PREFAB", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginPopupModal("Rename", 0, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Name: ");
			ImGui::SameLine();

			static char buffer[512];
			ImGui::InputText("##NewName", buffer, 512);

			ImGuiStyle& style = ImGui::GetStyle();
			float size = ImGui::CalcTextSize("Rename").x + style.FramePadding.x * 2.0f + ImGui::CalcTextSize("Close").x + style.FramePadding.x * 2.0f;

			float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
			if (ImGui::Button("Rename"))
			{
				std::filesystem::rename(file.path, currentDirectory / (std::string(buffer) + ".sdpref"));
				UpdateEntries();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawShaderFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)fileIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = file.path;
			ImGui::SetDragDropPayload("FILE_BROWSER_SHADER", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawGenericFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)fileIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}
}