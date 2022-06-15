#pragma once
#include "../Core/Asset.h"

#include "SoundInstance.h"

#include <Soloud/soloud_wav.h>

namespace Seidon
{
	enum class AttenuationModes
	{
		NONE = SoLoud::AudioSource::ATTENUATION_MODELS::NO_ATTENUATION,
		INVERSE_DISTANCE = SoLoud::AudioSource::ATTENUATION_MODELS::INVERSE_DISTANCE,
		EXPONENTIAL_DISTANCE = SoLoud::AudioSource::ATTENUATION_MODELS::EXPONENTIAL_DISTANCE,
		LINEAR_DISTANCE = SoLoud::AudioSource::ATTENUATION_MODELS::LINEAR_DISTANCE
	};

	class Sound : public Asset
	{
	public:
		Sound(UUID id = UUID());

		SoundInstance Play(float volume = 1.0f, float pan = 0.0f, bool paused = false);

		SoundInstance Play3d(const glm::vec3& position, const glm::vec3& velocity = glm::vec3(0), float volume = 1.0f, bool paused = false,
			AttenuationModes attenuationMode = AttenuationModes::EXPONENTIAL_DISTANCE, float rollofFactor = 1.5f, 
			float minDistance = 30.0f, float maxDistance = 10000.0f);
		
		void SetLooping(bool value);

		void Save(const std::string& path);
		void Load(const std::string& path);

		void Save(std::ofstream& out) {}
		void Load(std::ifstream& in)  {}
	private:
		SoLoud::Wav* source;
	};
}