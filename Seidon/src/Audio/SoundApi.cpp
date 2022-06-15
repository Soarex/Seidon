#include "SoundApi.h"

namespace Seidon
{
	void SoundApi::Init()
	{
		engine = new SoLoud::Soloud();

		engine->init();
	}

	void SoundApi::Update()
	{
		engine->update3dAudio();
	}

	void SoundApi::Destroy()
	{
		engine->stopAll();
		engine->deinit();

		delete engine;
	}
}