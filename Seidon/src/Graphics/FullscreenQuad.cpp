#include "FullscreenQuad.h"
#include <glad/glad.h>
#include <iostream>

#include "../Debug/Debug.h"

namespace Seidon
{
    FullscreenQuad::~FullscreenQuad()
    {
        Destroy();
    }

    void FullscreenQuad::Init()
    {
        SD_ASSERT(!initialized, "Fullscreen quad already initialized");

        vertices = new float[]
        {
            // positions        // uvs
            -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        GL_CHECK(glGenVertexArrays(1, &VAO));
        GL_CHECK(glGenBuffers(1, &VBO));

        GL_CHECK(glBindVertexArray(VAO));

        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(float), vertices, GL_STATIC_DRAW));

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        GL_CHECK(glBindVertexArray(0));

        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

        initialized = true;
    }

    void FullscreenQuad::Draw()
    {
        SD_ASSERT(initialized, "Fullscreen quad not initialized");

        GL_CHECK(glBindVertexArray(VAO));
        GL_CHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
        GL_CHECK(glBindVertexArray(0));
    }

    void FullscreenQuad::Destroy()
    {
        SD_ASSERT(initialized, "Fullscreen quad not initialized");

        GL_CHECK(glDeleteBuffers(1, &VBO));
        GL_CHECK(glDeleteVertexArrays(1, &VAO));

        delete[] vertices;
    }
}