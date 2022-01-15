#pragma once


#ifndef NDEBUG
#include <stdlib.h>
#include <iostream>
#include <glad/glad.h>

GLenum glCheckError(const char* file, int line);

#define SD_ASSERT(value, message) \
	if(!(value)) \
	{ \
		std::cerr << message << " | " << __FILE__ << " (" << __LINE__ << ")" << std::endl; \
		abort(); \
	}

#define GL_CHECK(x) x; if(glCheckError(__FILE__, __LINE__) != GL_NO_ERROR) abort();

#else

#define SD_ASSERT(value, message) value;
#define GL_CHECK(x) x;

#endif