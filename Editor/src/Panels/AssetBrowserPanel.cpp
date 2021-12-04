#include "AssetBrowserPanel.h"
#include "../Editor.h"
namespace Seidon
{
	void AssetBrowserPanel::Init()
	{
		selectedResource = ResourceType::NONE;

		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;

		backIcon = resourceManager.ImportTexture("Resources/BackIcon.png");
		fileIcon = resourceManager.ImportTexture("Resources/FileIcon.png");
		folderIcon = resourceManager.ImportTexture("Resources/FolderIcon.png");
		modelIcon = resourceManager.ImportTexture("Resources/ModelIcon.png");
		materialIcon = resourceManager.ImportTexture("Resources/MaterialIcon.png");
	}

	void AssetBrowserPanel::Draw()
	{
		ResourceManager* resourceManager = Application::Get()->GetResourceManager();
		ImGui::Begin("Content Browser");

		static float padding = 32.0f;
		static float thumbnailSize = 90.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;

		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		if (selectedResource == ResourceType::NONE)
		{
			ImGui::PushID("TexturesButton");

			ImGui::ImageButton((ImTextureID)folderIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				selectedResource = ResourceType::TEXTURE;

			ImGui::TextWrapped("Textures");
			ImGui::NextColumn();

			ImGui::PopID();
			//------------------------------
			ImGui::PushID("CubemapsButton");

			ImGui::ImageButton((ImTextureID)folderIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				selectedResource = ResourceType::CUBEMAP;

			ImGui::TextWrapped("Cubemaps");
			ImGui::NextColumn();

			ImGui::PopID();
			//------------------------------
			ImGui::PushID("MeshesButton");

			ImGui::ImageButton((ImTextureID)folderIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				selectedResource = ResourceType::MESH;

			ImGui::TextWrapped("Meshes");
			ImGui::NextColumn();

			ImGui::PopID();
			//------------------------------
			ImGui::PushID("MaterialsButton");

			ImGui::ImageButton((ImTextureID)folderIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				selectedResource = ResourceType::MATERIAL;

			ImGui::TextWrapped("Materials");
			ImGui::NextColumn();

			ImGui::PopID();
			//------------------------------
			ImGui::PushID("ShadersButton");

			ImGui::ImageButton((ImTextureID)folderIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				selectedResource = ResourceType::SHADER;

			ImGui::TextWrapped("Shaders");
			ImGui::NextColumn();

			ImGui::PopID();
		}
		else
		{
			if (ImGui::ImageButton((ImTextureID)backIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }))
				selectedResource = ResourceType::NONE;

			ImGui::TextWrapped("Back");
			ImGui::NextColumn();
		}

		if (selectedResource == ResourceType::TEXTURE)
			for (Texture* texture : resourceManager->GetTextures())
			{
				ImGui::PushID(texture);
				
				ImGui::ImageButton((ImTextureID)texture->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("CONTENT_BROWSER_TEXTURE", &texture, sizeof(Texture*));
					ImGui::EndDragDropSource();
				}

				ImGui::TextWrapped(texture->path.c_str());
				ImGui::NextColumn();

				ImGui::PopID();
			}

		if (selectedResource == ResourceType::CUBEMAP)
			for (HdrCubemap* cubemap : resourceManager->GetCubemaps())
			{
				ImGui::PushID(cubemap);

				ImGui::ImageButton((ImTextureID)fileIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("CONTENT_BROWSER_CUBEMAP", &cubemap, sizeof(HdrCubemap*));
					ImGui::EndDragDropSource();
				}

				ImGui::TextWrapped(cubemap->GetPath().c_str());
				ImGui::NextColumn();

				ImGui::PopID();
			}

		if (selectedResource == ResourceType::MESH)
			for (Mesh* mesh : resourceManager->GetMeshes())
			{
				ImGui::PushID(mesh);

				ImGui::ImageButton((ImTextureID)modelIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("CONTENT_BROWSER_MESH", &mesh, sizeof(Mesh*));
					ImGui::EndDragDropSource();
				}

				ImGui::TextWrapped(mesh->name.c_str());
				ImGui::NextColumn();

				ImGui::PopID();
			}

		if (selectedResource == ResourceType::MATERIAL)
			for (Material* material : resourceManager->GetMaterials())
			{
				ImGui::PushID(material);

				ImGui::ImageButton((ImTextureID)materialIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("CONTENT_BROWSER_MATERIAL", &material, sizeof(Material*));
					ImGui::EndDragDropSource();
				}

				ImGui::TextWrapped(material->name.c_str());
				ImGui::NextColumn();

				ImGui::PopID();
			}

		if (selectedResource == ResourceType::SHADER)
			for (Shader* shader : resourceManager->GetShaders())
			{
				ImGui::PushID(shader);

				ImGui::ImageButton((ImTextureID)fileIcon->GetRenderId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("CONTENT_BROWSER_SHADER", &shader, sizeof(Shader*));
					ImGui::EndDragDropSource();
				}

				ImGui::TextWrapped(shader->GetPath().c_str());
				ImGui::NextColumn();

				ImGui::PopID();
			}
		ImGui::Columns(1);

		ImGui::PopStyleColor();
		ImGui::End();
	}
}