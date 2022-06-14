#pragma once
#include <Soloud/soloud.h>
#include <glm/glm.hpp>

namespace Seidon
{
	class SoundInstance
	{
	public:
		void Stop();
		void SetPosition(const glm::vec3& position);
		void SetVelocity(const glm::vec3& velocity);
		void SetPositionAndVelocity(const glm::vec3& position, const glm::vec3& velocity);
	private:
		SoLoud::handle instanceHandle;
		SoLoud::Soloud* soundEngine;

		friend class Sound;
	};
}