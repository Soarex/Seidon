#include "AnimationPanel.h"

#include "../Editor.h"
#include "Utils/Dialogs.h"

#include <ImGui/imgui_neo_sequencer.h>

namespace Seidon
{
	void AnimationPanel::Init()
	{	
	}

	void AnimationPanel::Draw()
	{
		if(!ImGui::Begin("Animator")) 
		{
			ImGui::End();
			return;
		}

		Scene* activeScene = editor.GetSceneManager()->GetActiveScene();

		if (ImGui::Button("Open"))
		{
			std::string filepath = LoadFile("Seidon Animation (*.sdanim)\0*.sdanim\0");
			if (!filepath.empty())
				openedAnimation = editor.GetResourceManager()->GetOrLoadAsset<Animation>(filepath);
		}

		ImGui::SameLine();

		if (ImGui::Button("Attach Entity") && editor.selectedItem.type == SelectedItemType::ENTITY)
			attachedEntity = editor.selectedItem.entity;

		ImGui::SameLine();

		if (openedAnimation) ImGui::Text(openedAnimation->name.c_str());
		else ImGui::Text("No opened animation");
		
		ImGui::SameLine();

		if (activeScene->IsEntityIdValid(attachedEntity.ID)) ImGui::Text(attachedEntity.GetName().c_str());
		else ImGui::Text("No entity attached");

		if (!openedAnimation)
		{
			ImGui::End();
			return;
		}
			
		if (ImGui::BeginNeoSequencer("Sequencer", &currentFrame, &startFrame, &endFrame)) 
		{
			for (AnimationChannel& channel : openedAnimation->channels)
			{
				if (ImGui::BeginNeoGroup(channel.boneName.c_str()))
				{
					ImGui::PushID(channel.boneName.c_str());
					
					std::vector<uint32_t> keys;

					keys.reserve(channel.positionKeys.size());
					for (PositionKey& key : channel.positionKeys)
						keys.push_back(key.time);

					if (ImGui::BeginNeoTimeline("Position", keys, nullptr, ImGuiNeoTimelineFlags_AllowFrameChanging))
						ImGui::EndNeoTimeLine();

					keys.reserve(channel.rotationKeys.size());
					keys.clear();
					for (RotationKey& key : channel.rotationKeys)
						keys.push_back(key.time);

					if (ImGui::BeginNeoTimeline("Rotation", keys, nullptr, ImGuiNeoTimelineFlags_AllowFrameChanging))
						ImGui::EndNeoTimeLine();

					keys.reserve(channel.scalingKeys.size());
					keys.clear();
					for (ScalingKey& key : channel.scalingKeys)
						keys.push_back(key.time);

					if (ImGui::BeginNeoTimeline("Scale", keys, nullptr, ImGuiNeoTimelineFlags_AllowFrameChanging))
						ImGui::EndNeoTimeLine();

					ImGui::EndNeoTimeLine();
					ImGui::PopID();
				}
			}
			ImGui::EndNeoSequencer();
		}

		ImGui::End();

		if (activeScene->IsEntityIdValid(attachedEntity.ID)) UpdateAttachedEntity();
	}

	void AnimationPanel::UpdateAttachedEntity()
	{
	}
}