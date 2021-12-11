#include "CaptureCube.h"
#include <glad/glad.h>
#include <iostream>

#include "../Debug/Debug.h"

namespace Seidon
{
	CaptureCube::~CaptureCube()
	{
		Destroy();
	}

	void CaptureCube::Init()
	{
        SD_ASSERT(!initialized, "Capture cube already initialized");

		cubeVertices = new float[]
		{
			// positions          
            -1.0f, 1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,

                -1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,

                -1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, 1.0f
		};

        GL_CHECK(glGenVertexArrays(1, &cubeVAO));
        GL_CHECK(glGenBuffers(1, &cubeVBO));

		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, cubeVBO));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(float), cubeVertices, GL_STATIC_DRAW));

		GL_CHECK(glBindVertexArray(cubeVAO));
		GL_CHECK(glEnableVertexAttribArray(0));
		GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GL_CHECK(glBindVertexArray(0));

        initialized = true;
	}

	void CaptureCube::Draw()
	{
        SD_ASSERT(initialized, "Capture cube not initialized");

        GL_CHECK(glBindVertexArray(cubeVAO));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 36));
        GL_CHECK(glBindVertexArray(0));
    }  

	void CaptureCube::Destroy()
	{
        SD_ASSERT(initialized, "Capture cube not initialized");

		GL_CHECK(glDeleteBuffers(1, &cubeVBO));
		GL_CHECK(glDeleteVertexArrays(1, &cubeVAO));

		delete[] cubeVertices;
	}
}