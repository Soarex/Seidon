#include "FileBrowserPanel.h"

#include "../Editor.h"

namespace Seidon
{
	void FileBrowserPanel::Init()
	{
		ResourceManager& resourceManager = editor.editorResourceManager;

		std::string currentPath = std::filesystem::current_path().string();
		backIcon = resourceManager.GetOrLoadAsset<Texture>(currentPath + "/Resources/BackIcon.sdtex", true);
		fileIcon = resourceManager.GetOrLoadAsset<Texture>(currentPath + "/Resources/FileIcon.sdtex", true);
		folderIcon = resourceManager.GetOrLoadAsset<Texture>(currentPath + "/Resources/FolderIcon.sdtex", true);
		modelIcon = resourceManager.GetOrLoadAsset<Texture>(currentPath + "/Resources/ModelIcon.sdtex", true);
		materialIcon = resourceManager.GetOrLoadAsset<Texture>(currentPath + "/Resources/MaterialIcon.sdtex", true);
		animationIcon = resourceManager.GetOrLoadAsset<Texture>(currentPath + "/Resources/AnimationIcon.sdtex", true);
		skinnedMeshIcon = resourceManager.GetOrLoadAsset<Texture>(currentPath + "/Resources/SkinnedMeshIcon.sdtex", true);
		prefabIcon = resourceManager.GetOrLoadAsset<Texture>(currentPath + "/Resources/PrefabIcon.sdtex", true);
		fontIcon = resourceManager.GetOrLoadAsset<Texture>(currentPath + "/Resources/FontIcon.sdtex", true);
		colliderIcon = resourceManager.GetOrLoadAsset<Texture>(currentPath + "/Resources/MeshColliderIcon.sdtex", true);
		soundIcon = resourceManager.GetOrLoadAsset<Texture>(currentPath + "/Resources/SoundIcon.sdtex", true);

		currentDirectory = editor.openProject->assetsDirectory;
		UpdateEntries();
	}

	void FileBrowserPanel::Draw()
	{
		ResourceManager& resourceManager = *editor.GetResourceManager();

		if (!ImGui::Begin("File Browser"))
		{
			ImGui::End();
			return;
		}

		static bool openPopup;
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create New Directory"))
				openPopup = true;
		
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

				resourceManager.RegisterAsset(&m, currentDirectory.string() + "\\" + m.name + ".sdmat");

				UpdateEntries();
			}

			if (ImGui::MenuItem("Update Asset Registry"))
				UpdateRegistry(editor.openProject->assetsDirectory);

			if (ImGui::MenuItem("Refresh"))
				UpdateEntries();


			ImGui::EndPopup();
		}

		if (openPopup)
			ImGui::OpenPopup("Create Directory");

		if (ImGui::BeginPopupModal("Create Directory", 0, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Name: ");
			ImGui::SameLine();

			static char buffer[512];
			ImGui::InputText("##Name", buffer, 512);

			ImGuiStyle& style = ImGui::GetStyle();
			float size = ImGui::CalcTextSize("Create").x + style.FramePadding.x * 2.0f + ImGui::CalcTextSize("Close").x + style.FramePadding.x * 2.0f;

			float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
			if (ImGui::Button("Create"))
			{
				std::filesystem::create_directory(currentDirectory / std::string(buffer));
				UpdateEntries();
				ImGui::CloseCurrentPopup();
				openPopup = false;
			}

			ImGui::SameLine();

			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
				openPopup = false;
			}

			ImGui::EndPopup();
		}

		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;

		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		if (currentDirectory != std::filesystem::path(editor.openProject->assetsDirectory))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			if (ImGui::ImageButton((ImTextureID)backIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }))
			{
				currentDirectory = currentDirectory.parent_path();
				UpdateEntries();
			}

					
			if (ImGui::BeginDragDropTarget())
			{
				const char* payloadNames[] =
				{
					"FILE_BROWSER_MATERIAL",
					"FILE_BROWSER_MESH",
					"FILE_BROWSER_SKINNED_MESH",
					"FILE_BROWSER_TEXTURE",
					"FILE_BROWSER_FONT",
					"FILE_BROWSER_SCENE",
					"FILE_BROWSER_SHADER",
					"FILE_BROWSER_CUBEMAP",
					"FILE_BROWSER_COLLIDER",
					"FILE_BROWSER_SOUND",
					"FILE_BROWSER_ANIMATION"
				};

				for (const char* payloadName : payloadNames)
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadName))
					{
						std::filesystem::path currentPath = (const char*)payload->Data;
						std::filesystem::path newPath = currentDirectory.parent_path() / currentPath.filename();

						MoveAsset(currentPath.string(), newPath.string());

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

	void FileBrowserPanel::UpdateRegistry(const std::string& directoryPath)
	{
		for (auto& entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_directory())
			{
				UpdateRegistry(entry.path().string());
				continue;
			}

			std::string fileExtension = entry.path().extension().string();

			if (fileExtension == ".sdmesh" || fileExtension == ".sdskmesh" || fileExtension == ".sdtex" || fileExtension == ".sdmat"
				|| fileExtension == ".sdhdr" || fileExtension == ".sdanim" || fileExtension == ".sdfont" || fileExtension == ".sdcoll")
			{
				std::ifstream in(entry.path().string(), std::ios::binary | std::ios::in);

				UUID id;
				in.read((char*)&id, sizeof(UUID));

				editor.GetResourceManager()->RegisterAssetId(id, entry.path().string());
			}
		}
	}

	void FileBrowserPanel::MoveAsset(const std::string& from, const std::string& to)
	{
		std::filesystem::rename(from, to);

		ResourceManager& resourceManager = *editor.GetResourceManager();

		if (resourceManager.IsAssetRegistered(from))
			resourceManager.RegisterAssetId(resourceManager.GetAssetId(from), to);
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
				const char* payloadNames[] =
				{
					"FILE_BROWSER_MATERIAL",
					"FILE_BROWSER_MESH",
					"FILE_BROWSER_SKINNED_MESH",
					"FILE_BROWSER_TEXTURE",
					"FILE_BROWSER_FONT",
					"FILE_BROWSER_SCENE",
					"FILE_BROWSER_SHADER",
					"FILE_BROWSER_CUBEMAP",
					"FILE_BROWSER_COLLIDER",
					"FILE_BROWSER_SOUND",
					"FILE_BROWSER_ANIMATION"
				};

				for(const char* payloadName : payloadNames)
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadName))
					{
						std::filesystem::path currentPath = (const char*)payload->Data;
						std::filesystem::path newPath = currentDirectory / directory.name / currentPath.filename();

						MoveAsset(currentPath.string(), newPath.string());

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

			else if (file.extension == ".ttf")
				DrawExternalFontFile(file);

			else if (file.extension == ".sdtex")
				DrawTextureFile(file);

			else if (file.extension == ".sdmesh")
				DrawMeshFile(file);

			else if (file.extension == ".sdskmesh")
				DrawSkinnedMeshFile(file);

			else if (file.extension == ".sdmat")
				DrawMaterialFile(file);

			else if (file.extension == ".sdhdr")
				DrawCubemapFile(file);

			else if (file.extension == ".sdanim")
				DrawAnimationFile(file);

			else if (file.extension == ".sdscene")
				DrawSceneFile(file);

			else if (file.extension == ".sdpref")
				DrawPrefabFile(file);

			else if (file.extension == ".sdshader")
				DrawShaderFile(file);

			else if (file.extension == ".sdfont")
				DrawFontFile(file);

			else if (file.extension == ".sdcoll")
				DrawColliderFile(file);

			else if (file.extension == ".mp3" || file.extension == ".wav" || file.extension == ".ogg")
				DrawSoundFile(file);

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

			static bool flip;
			ImGui::Text("Flip vertically: ");
			ImGui::SameLine();
			ImGui::Checkbox("##Flip vertically", &flip);

			const char* clampModeNames[] = { "Repeat", "Clamp To Edge", "Clamp To Border" };
			ClampingMode clampModes[] = { ClampingMode::REPEAT, ClampingMode::CLAMP, ClampingMode::BORDER };
			static int selectedModeIndex = 0;

			if (ImGui::BeginCombo("##Clamp mode", clampModeNames[selectedModeIndex], NULL))
			{
				for (int i = 0; i < IM_ARRAYSIZE(clampModeNames); i++)
				{
					const bool selected = (selectedModeIndex == i);
					if (ImGui::Selectable(clampModeNames[i], selected))
						selectedModeIndex = i;

					if (selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Import"))
			{
				importer.ImportTexture(editor.GetResourceManager()->AbsoluteToRelativePath(file.path), gammaCorrection, flip, clampModes[selectedModeIndex]);
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

		bool imported = false;
		if (ImGui::BeginPopupModal("Model Import", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Import model file?");

			ImGui::Separator();

			if (ImGui::Button("Import"))
			{
				importer.ImportModelFile(editor.GetResourceManager()->AbsoluteToRelativePath(file.path));
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

	void FileBrowserPanel::DrawExternalFontFile(FileEntry& file)
	{
		bool clicked = false;
		if (ImGui::ImageButton((ImTextureID)fileIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }))
			ImGui::OpenPopup("Font Import");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		bool imported = false;
		if (ImGui::BeginPopupModal("Font Import", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Import font file?");

			ImGui::Separator();

			if (ImGui::Button("Import"))
			{
				delete importer.ImportFont(editor.GetResourceManager()->AbsoluteToRelativePath(file.path));
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

		bool imported = false;
		if (ImGui::BeginPopupModal("Cubemap Import", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Import cubemap file?");

			ImGui::Separator();

			if (ImGui::Button("Import"))
			{
				delete importer.ImportCubemap(editor.GetResourceManager()->AbsoluteToRelativePath(file.path));
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

		Texture* t = resourceManager.GetOrLoadAsset<Texture>(file.path, true);
		ImGui::ImageButton((ImTextureID)t->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = editor.GetResourceManager()->AbsoluteToRelativePath(file.path);
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
			const std::string& itemPath = editor.GetResourceManager()->AbsoluteToRelativePath(file.path);
			ImGui::SetDragDropPayload("FILE_BROWSER_MESH", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawSkinnedMeshFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)skinnedMeshIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = editor.GetResourceManager()->AbsoluteToRelativePath(file.path);
			ImGui::SetDragDropPayload("FILE_BROWSER_SKINNED_MESH", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawMaterialFile(FileEntry& file)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;

		ImGui::ImageButton((ImTextureID)materialIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			editor.selectedItem.type = SelectedItemType::MATERIAL;
			editor.selectedItem.material = resourceManager.GetOrLoadAsset<Material>(file.path);
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			ImGui::OpenPopup("Rename");

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = editor.GetResourceManager()->AbsoluteToRelativePath(file.path);
			ImGui::SetDragDropPayload("FILE_BROWSER_MATERIAL", itemPath.c_str(), itemPath.length() + 1);
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

	void FileBrowserPanel::DrawAnimationFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)animationIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = editor.GetResourceManager()->AbsoluteToRelativePath(file.path);
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
			const std::string& itemPath = editor.GetResourceManager()->AbsoluteToRelativePath(file.path);
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
			const std::string& itemPath = editor.GetResourceManager()->AbsoluteToRelativePath(file.path);
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
			const std::string& itemPath = editor.GetResourceManager()->AbsoluteToRelativePath(file.path);
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
			const std::string& itemPath = editor.GetResourceManager()->AbsoluteToRelativePath(file.path);
			ImGui::SetDragDropPayload("FILE_BROWSER_SHADER", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawFontFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)fontIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = editor.GetResourceManager()->AbsoluteToRelativePath(file.path);
			ImGui::SetDragDropPayload("FILE_BROWSER_FONT", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawColliderFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)colliderIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = editor.GetResourceManager()->AbsoluteToRelativePath(file.path);
			ImGui::SetDragDropPayload("FILE_BROWSER_COLLIDER", itemPath.c_str(), itemPath.length() + 1);
			ImGui::EndDragDropSource();
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::TextWrapped(file.name.c_str());
		ImGui::NextColumn();
	}

	void FileBrowserPanel::DrawSoundFile(FileEntry& file)
	{
		ImGui::ImageButton((ImTextureID)soundIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const std::string& itemPath = editor.GetResourceManager()->AbsoluteToRelativePath(file.path);
			ImGui::SetDragDropPayload("FILE_BROWSER_SOUND", itemPath.c_str(), itemPath.length() + 1);
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