#pragma once
#include <Seidon.h>
#include "Panel.h"

namespace Seidon
{
	class AnimationPanel : public Panel
	{
	public:
		AnimationPanel(Editor& editor) : Panel(editor) {}

		void Init() override;
		void Draw() override;
		
	private:
		Animation* openedAnimation = nullptr;
		Entity attachedEntity = {NullEntityId, nullptr};
		
		uint32_t currentFrame = 0;
		uint32_t startFrame = 0;
		uint32_t endFrame = 64;

	private:
		void UpdateAttachedEntity();
	};
}