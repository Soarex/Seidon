#include "Framebuffer.h"
#include "../Debug/Debug.h"

namespace Seidon
{
	Framebuffer::~Framebuffer()
	{
		Destroy();
	}

	void Framebuffer::Create()
	{
		SD_ASSERT(!initialized, "Framebuffer already initialized");
		
		GL_CHECK(glGenFramebuffers(1, &renderId));

		initialized = true;
	}

	void Framebuffer::Destroy()
	{
		SD_ASSERT(initialized, "Framebuffer not initialized");

		GL_CHECK(glDeleteFramebuffers(1, &renderId));
		
		initialized = false;
	}

	void Framebuffer::SetColorTexture(const Texture& texture, unsigned int index)
	{
		SD_ASSERT(initialized, "Framebuffer not initialized");

		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderId));
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture.GetRenderId(), 0));
	}

	void Framebuffer::DisableColorBuffer()
	{
		SD_ASSERT(initialized, "Framebuffer not initialized");

		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderId));
		GL_CHECK(glDrawBuffer(GL_NONE));
		GL_CHECK(glReadBuffer(GL_NONE));
	}

	void Framebuffer::SetDepthTexture(const Texture& texture)
	{
		SD_ASSERT(initialized, "Framebuffer not initialized");

		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderId));
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.GetRenderId(), 0));
	}

	void Framebuffer::SetDepthStencilRenderBuffer(const RenderBuffer& renderBuffer)
	{
		SD_ASSERT(initialized, "Framebuffer not initialized");

		GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.GetRenderId()));
	}

	void Framebuffer::Bind()
	{
		SD_ASSERT(initialized, "Framebuffer not initialized");

		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderId));
	}

	void Framebuffer::Unbind()
	{
		SD_ASSERT(initialized, "Framebuffer not initialized");

		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
}