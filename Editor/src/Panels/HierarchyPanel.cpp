#include "HierarchyPanel.h"
#include "../Editor.h"

#include <ostream>

namespace Seidon
{
	void HierarchyPanel::Init()
	{
	}

	void HierarchyPanel::Draw()
	{
		if (!ImGui::Begin("Hierarchy"))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
		ImGui::Dummy({ ImGui::GetWindowSize().x, 10});
		ImGui::PopStyleVar();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_PANEL_ENTITY"))
			{
				Entity& e = *(Entity*)payload->Data;
				e.RemoveParent();
			}

			ImGui::EndDragDropTarget();
		}

		editor.activeScene->GetRegistry().each([&](auto entityId)
			{
				Scene* scene = editor.activeScene;
				if (!scene->IsEntityIdValid(entityId)) return;

				Entity entity(entityId, scene);

				UUID parentId = entity.GetComponent<TransformComponent>().parent;

				if (scene->IsIdValid(parentId)) return;

				DrawEntityNode(entity);

			});

		ImGui::Dummy({ ImGui::GetWindowSize().x, 25 });
		
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_PANEL_ENTITY"))
			{
				Entity& e = *(Entity*)payload->Data;

				e.RemoveParent();
			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Add Empty Entity"))
			{
				Entity e = editor.activeScene->CreateEntity();

				editor.selectedItem.type = SelectedItemType::ENTITY;
				editor.selectedItem.entity = e;
			}

			if (ImGui::MenuItem("Add Camera"))
			{
				Entity e = editor.activeScene->CreateEntity();
				e.AddComponent<CameraComponent>();

				e.SetName("Camera");

				editor.selectedItem.type = SelectedItemType::ENTITY;
				editor.selectedItem.entity = e;
			}

			if (ImGui::MenuItem("Add Directonal Light"))
			{
				Entity e = editor.activeScene->CreateEntity();
				e.AddComponent<DirectionalLightComponent>();

				e.SetName("Directional Light");

				editor.selectedItem.type = SelectedItemType::ENTITY;
				editor.selectedItem.entity = e;
			}

			if (ImGui::MenuItem("Add Skylight"))
			{
				Entity e = editor.activeScene->CreateEntity();
				e.AddComponent<ProceduralSkylightComponent>();

				e.SetName("Skylight");

				editor.selectedItem.type = SelectedItemType::ENTITY;
				editor.selectedItem.entity = e;
			}

			if (ImGui::BeginMenu("3D Shapes"))
			{
				if (ImGui::MenuItem("Add Cube"))
				{
					Entity e = editor.activeScene->CreateEntity();
					RenderComponent& r = e.AddComponent<RenderComponent>();
					r.SetMesh(editor.GetResourceManager()->GetOrLoadAsset<Mesh>("Resource\\Cube.sdmesh"));

					e.SetName("Cube");

					editor.selectedItem.type = SelectedItemType::ENTITY;
					editor.selectedItem.entity = e;
				}

				if (ImGui::MenuItem("Add Sphere"))
				{
					Entity e = editor.activeScene->CreateEntity();
					RenderComponent& r = e.AddComponent<RenderComponent>();
					r.SetMesh(editor.GetResourceManager()->GetOrLoadAsset<Mesh>("Resource\\Sphere.sdmesh"));

					e.SetName("Sphere");

					editor.selectedItem.type = SelectedItemType::ENTITY;
					editor.selectedItem.entity = e;
				}

				if (ImGui::MenuItem("Add Cylinder"))
				{
					Entity e = editor.activeScene->CreateEntity();
					RenderComponent& r = e.AddComponent<RenderComponent>();
					r.SetMesh(editor.GetResourceManager()->GetOrLoadAsset<Mesh>("Resource\\Cylinder.sdmesh"));

					e.SetName("Cylinder");

					editor.selectedItem.type = SelectedItemType::ENTITY;
					editor.selectedItem.entity = e;
				}

				if (ImGui::MenuItem("Add Plane"))
				{
					Entity e = editor.activeScene->CreateEntity();
					RenderComponent& r = e.AddComponent<RenderComponent>();
					r.SetMesh(editor.GetResourceManager()->GetOrLoadAsset<Mesh>("Resource\\Plane.sdmesh"));

					e.SetName("Plane");

					editor.selectedItem.type = SelectedItemType::ENTITY;
					editor.selectedItem.entity = e;
				}

				if (ImGui::MenuItem("Add Empty Mesh"))
				{
					Entity e = editor.activeScene->CreateEntity();
					e.AddComponent<RenderComponent>();

					e.SetName("Mesh");

					editor.selectedItem.type = SelectedItemType::ENTITY;
					editor.selectedItem.entity = e;
				}

				if (ImGui::MenuItem("Add Empty Animated Mesh"))
				{
					Entity e = editor.activeScene->CreateEntity();
					e.AddComponent<SkinnedRenderComponent>();
					e.AddComponent<AnimationComponent>();

					e.SetName("Animated Mesh");

					editor.selectedItem.type = SelectedItemType::ENTITY;
					editor.selectedItem.entity = e;
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Add Text"))
			{
				Entity e = editor.activeScene->CreateEntity();
				e.AddComponent<TextRenderComponent>();

				e.SetName("Text");

				editor.selectedItem.type = SelectedItemType::ENTITY;
				editor.selectedItem.entity = e;
			}

			if (ImGui::MenuItem("Add Sprite"))
			{
				Entity e = editor.activeScene->CreateEntity();
				e.AddComponent<SpriteRenderComponent>();

				e.SetName("Sprite");

				editor.selectedItem.type = SelectedItemType::ENTITY;
				editor.selectedItem.entity = e;
			}

			if (ImGui::MenuItem("Add UI Anchor"))
			{
				Entity e = editor.activeScene->CreateEntity();
				e.AddComponent<UIAnchorComponent>();

				e.SetName("UI Anchor");

				editor.selectedItem.type = SelectedItemType::ENTITY;
				editor.selectedItem.entity = e;
			}

			if (ImGui::MenuItem("Add UI Text"))
			{
				Entity e = editor.activeScene->CreateEntity();
				e.AddComponent<UITextComponent>();

				e.SetName("UI Text");

				editor.selectedItem.type = SelectedItemType::ENTITY;
				editor.selectedItem.entity = e;
			}

			if (ImGui::MenuItem("Add UI Sprite"))
			{
				Entity e = editor.activeScene->CreateEntity();
				e.AddComponent<UISpriteComponent>();

				e.SetName("UI Sprite");

				editor.selectedItem.type = SelectedItemType::ENTITY;
				editor.selectedItem.entity = e;
			}


			ImGui::EndPopup();
		}


		ImGui::End();
	}

	void HierarchyPanel::DrawEntityNode(Entity& entity)
	{
		std::string& name = entity.GetComponent<NameComponent>().name;
		UUID id = entity.GetComponent<IDComponent>().ID;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
		
		if (editor.selectedItem.type == SelectedItemType::ENTITY && editor.selectedItem.entity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		bool open = ImGui::TreeNodeEx((void*)(long long)id, flags, name.c_str());

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("HIERARCHY_PANEL_ENTITY", &entity, sizeof(Entity));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_PANEL_ENTITY"))
			{
				Entity& e = *(Entity*)payload->Data;

				if (!e.IsAncestorOf(entity))
					e.SetParent(entity);

			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			editor.selectedItem.type = SelectedItemType::ENTITY;
			editor.selectedItem.entity = entity;
		}

		ImGui::PopStyleVar();
		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		if(open)
		{
			if (entity.HasComponent<SkinnedRenderComponent>())
			{
				SkinnedRenderComponent& r = entity.GetComponent<SkinnedRenderComponent>();
				DrawBoneNode(entity, r.mesh->armature, r.boneTransforms, 0);
			}

			TransformComponent& t = entity.GetComponent<TransformComponent>();

			for (UUID i : t.children)
			{
				Entity e = entity.scene->GetEntityById(i);
				DrawEntityNode(e);
			}
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			if (editor.selectedItem.type == SelectedItemType::ENTITY && (editor.selectedItem.entity == entity || editor.selectedItem.entity.IsDescendantOf(entity)))
				editor.selectedItem.type = SelectedItemType::NONE;

			if(editor.selectedItem.type == SelectedItemType::BONE)
				editor.selectedItem.type = SelectedItemType::NONE;

			editor.activeScene->DestroyEntity(entity);
		}

		ImGui::PopStyleVar();
	}

	void HierarchyPanel::DrawBoneNode(Entity& owningEntity, Armature& armature, std::vector<glm::mat4>& boneTransforms, int index)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (editor.selectedItem.type == SelectedItemType::BONE && editor.selectedItem.boneData.armature->bones[editor.selectedItem.boneData.id] == armature.bones[index])
			flags |= ImGuiTreeNodeFlags_Selected;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 1.0f, 0.0, 0.7f});

		bool open = ImGui::TreeNodeEx(armature.bones[index].name.c_str(), flags, armature.bones[index].name.c_str());

		if (ImGui::IsItemClicked())
		{
			editor.selectedItem.type = SelectedItemType::BONE;
			editor.selectedItem.boneData.id = index;
			editor.selectedItem.boneData.owningEntity = owningEntity;
			editor.selectedItem.boneData.armature = &armature;
			editor.selectedItem.boneData.transforms = &boneTransforms;
		}

		if (open)
		{
			for (int i = index + 1; i < armature.bones.size(); i++)
				if(armature.bones[i].parentId == index)
					DrawBoneNode(owningEntity, armature, boneTransforms, i);

			ImGui::TreePop();
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
}
