#pragma once
#include <glad/glad.h>

#include "Texture.h"
#include "RenderBuffer.h"

namespace Seidon
{
	class Framebuffer
	{
	private:
		unsigned int renderId;

	public:
		Framebuffer() = default;
		~Framebuffer();

		void Create();

		void Bind();
		void Unbind();

		void SetColorTexture(const Texture& texture, unsigned int index = 0);
		void SetDepthTexture(const Texture& texture);
		void SetDepthStencilRenderBuffer(const RenderBuffer& renderBuffer);

		void DisableColorBuffer();

		inline unsigned int GetRenderId() { return renderId; }
	};
}