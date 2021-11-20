#include "Framebuffer.h"

namespace Seidon
{
	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &renderId);
	}

	void Framebuffer::Create()
	{
		glGenFramebuffers(1, &renderId);
	}

	void Framebuffer::SetColorTexture(const Texture& texture, unsigned int index)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture.GetRenderId(), 0);
	}

	void Framebuffer::DisableColorBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderId);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	void Framebuffer::SetDepthTexture(const Texture& texture)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.GetRenderId(), 0);
	}

	void Framebuffer::SetDepthStencilRenderBuffer(const RenderBuffer& renderBuffer)
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.GetRenderId());
	}

	void Framebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderId);
	}

	void Framebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}