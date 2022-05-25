#pragma once
#include <Seidon.h>
#include "../SelectedItem.h"

namespace Seidon
{
	class AnimationPanel
	{
	public:
		AnimationPanel(SelectedItem& selectedItem) : selectedItem(selectedItem) {}

		void Init();
		void Draw();
		
	private:
		SelectedItem& selectedItem;

		Animation* openedAnimation = nullptr;
		Entity attachedEntity = {NullEntityId, nullptr};
		
		uint32_t currentFrame = 0;
		uint32_t startFrame = 0;
		uint32_t endFrame = 64;

	private:
		void UpdateAttachedEntity();
	};
}