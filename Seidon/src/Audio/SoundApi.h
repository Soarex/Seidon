#pragma once
#include <Soloud/soloud.h>

namespace Seidon
{
	class SoundApi
	{
	public:
		void Init();
		void Update();
		void Destroy();

		inline SoLoud::Soloud* GetSoundEngine() { return engine; }
	private:
		SoLoud::Soloud* engine;
	};
}