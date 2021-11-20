#include "RenderBuffer.h"

namespace Seidon
{
	RenderBuffer::RenderBuffer()
	{
		
	}

	RenderBuffer::~RenderBuffer()
	{
		glDeleteRenderbuffers(1, &renderId);
	}

	void RenderBuffer::Create(int width, int height, RenderBufferType type)
	{
		this->width = width; this->height = height;

		glGenRenderbuffers(1, &renderId);
		glBindRenderbuffer(GL_RENDERBUFFER, renderId);
		glRenderbufferStorage(GL_RENDERBUFFER, (int)type, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void RenderBuffer::Destroy()
	{
		glDeleteRenderbuffers(1, &renderId);
	}
}