#include "Sound.h"

#include "../Core/Application.h"

#include <Soloud/soloud.h>
#include <Soloud/soloud_wav.h>

namespace Seidon
{
	SoundInstance Sound::Play(float volume, float pan, bool paused)
	{
		SoundApi& api = *Application::Get()->GetSoundApi();

		SoundInstance res;
		res.instanceHandle = api.GetSoundEngine()->play(*source, volume, pan, paused);
		res.soundEngine = api.GetSoundEngine();

		return res;
	}

	SoundInstance Sound::Play3d(const glm::vec3& position, const glm::vec3& velocity, float volume, bool paused)
	{
		SoundApi& api = *Application::Get()->GetSoundApi();

		SoundInstance res;

		res.instanceHandle = api.GetSoundEngine()->play3d
		(
			*source, 
			position.x,
			position.y,
			position.z,
			velocity.x,
			velocity.y,
			velocity.z,
			volume, 
			paused
		);

		res.soundEngine = api.GetSoundEngine();

		return res;
	}

	void Sound::SetLooping(bool value)
	{
		source->setLooping(value);
	}

	void Sound::Save(const std::string& path)
	{
	
	}

	void Sound::Load(const std::string& path)
	{
		SoundApi& api = *Application::Get()->GetSoundApi();

		source = new SoLoud::Wav();
		source->load(path.c_str());
	}
}