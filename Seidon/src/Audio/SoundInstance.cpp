#include "SoundInstance.h"

namespace Seidon
{
	void SoundInstance::Stop()
	{
		soundEngine->stop(instanceHandle);
	}

	void SoundInstance::SetPosition(const glm::vec3& position)
	{
		soundEngine->set3dSourcePosition(instanceHandle, position.x, position.y, position.z);
	}

	void SoundInstance::SetVelocity(const glm::vec3& velocity)
	{
		soundEngine->set3dSourceVelocity(instanceHandle, velocity.x, velocity.y, velocity.z);
	}

	void SoundInstance::SetPositionAndVelocity(const glm::vec3& position, const glm::vec3& velocity)
	{
		SetPosition(position);
		SetVelocity(velocity);
	}
}