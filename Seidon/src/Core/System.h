#pragma once

namespace Seidon
{
	class Scene;

	class System
	{
	protected:
		Scene* scene;
	public:
		virtual void Init() {};
		virtual void Update(float deltaTime) {};
		virtual void Destroy() {};

		friend class Scene;
	};
}