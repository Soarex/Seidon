#pragma once
#include <glad/glad.h>

namespace Seidon
{
	enum class RenderBufferType
	{
		DEPTH_STENCIL = GL_DEPTH24_STENCIL8
	};

	class RenderBuffer
	{
	private:
		unsigned int renderId;
		int width, height;
	public:
		RenderBuffer();
		~RenderBuffer();

		void Create(int width, int height, RenderBufferType type);
		void Destroy();

		inline unsigned int GetRenderId() const { return renderId; }
	};
}