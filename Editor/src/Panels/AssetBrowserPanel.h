#pragma once
#include <Seidon.h>

namespace Seidon
{
	enum class ResourceType
	{
		TEXTURE = 0,
		CUBEMAP,
		MESH,
		MATERIAL,
		SHADER,
		ARMATURE,
		ANIMATION,
		ALL,
		NONE
	};

	class AssetBrowserPanel
	{
	private:
		Texture* fileIcon;
		Texture* folderIcon;
		Texture* modelIcon;
		Texture* materialIcon;
		Texture* armatureIcon;
		Texture* animationIcon;
		Texture* backIcon;
		ResourceType selectedResource;
	public:
		AssetBrowserPanel() = default;
		void Init();
		void Draw();
	};
}