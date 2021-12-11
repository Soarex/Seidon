#include "RenderBuffer.h"
#include "../Debug/Debug.h"

namespace Seidon
{
	RenderBuffer::RenderBuffer()
	{
		
	}

	RenderBuffer::~RenderBuffer()
	{
		Destroy();
	}

	void RenderBuffer::Create(int width, int height, RenderBufferType type)
	{
		SD_ASSERT(!initialized, "RenderBuffer already initialized");

		this->width = width; this->height = height;

		GL_CHECK(glGenRenderbuffers(1, &renderId));
		GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, renderId));
		GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, (int)type, width, height));
		GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, 0));

		initialized = true;
	}

	void RenderBuffer::Destroy()
	{
		SD_ASSERT(initialized, "RenderBuffer not initialized");

		GL_CHECK(glDeleteRenderbuffers(1, &renderId));

		initialized = false;
	}
}