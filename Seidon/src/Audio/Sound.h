#pragma once
#include "../Core/Asset.h"

#include "SoundInstance.h"
#include <glm/glm.hpp>

namespace SoLoud
{
	class Wav;
}

namespace Seidon
{
	class Sound : public Asset
	{
	public:
		SoundInstance Play(float volume = 1.0f, float pan = 0.0f, bool paused = false);
		SoundInstance Play3d(const glm::vec3& position, const glm::vec3& velocity = glm::vec3(0), float volume = 1.0f, bool paused = false);
		void SetLooping(bool value);

		void Save(const std::string& path);
		void Load(const std::string& path);

		void Save(std::ofstream& out) {}
		void Load(std::ifstream& in)  {}
	private:
		SoLoud::Wav* source;
	};
}