#pragma once

namespace Seidon
{
	class FullscreenQuad
	{
	private:
		bool initialized = false;
		unsigned int VAO = 0;
		unsigned int VBO = 0;

		float* vertices;

	public:
		FullscreenQuad() = default;
		~FullscreenQuad();

		void Init();
		void Draw();
		void Destroy();
	};
}