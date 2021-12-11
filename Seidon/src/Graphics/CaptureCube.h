#pragma once

namespace Seidon
{
	class CaptureCube
	{
	private:
		bool initialized = false;
		unsigned int cubeVAO = 0;
		unsigned int cubeVBO = 0;

		float* cubeVertices;

	public:
		CaptureCube() = default;
		~CaptureCube();

		void Init();
		void Draw();
		void Destroy();
	};
}