#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <StbImage/stb_image.h> 

#include <string>
#include <iostream>

#include "Core/WorkManager.h"

namespace Seidon
{
    enum class TextureFormat
    {
        RED = GL_RED,
        RED_GREEN = GL_RG,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
        SRGB = GL_SRGB,
        SRGBA = GL_SRGB_ALPHA,
        DEPTH = GL_DEPTH_COMPONENT,
        FLOAT16 = GL_RGB16F,
        FLOAT16_ALPHA = GL_RGBA16F,
        FLOAT16_RED_GREEN = GL_RG16F,
        FLOAT32 = GL_RGBA32F
    };

    enum class ClampingMode
    {
        CLAMP = GL_CLAMP_TO_EDGE,
        REPEAT = GL_REPEAT,
        BORDER = GL_CLAMP_TO_BORDER
    };

    class Texture
    {
    public:
        std::string path;

    private:
        static Texture temporaryTexture;
        unsigned int ID;

    public:
        Texture();
        ~Texture();

        void Create(int width, int height, unsigned char* rgbData, TextureFormat startFormat = TextureFormat::RGB, TextureFormat endFormat = TextureFormat::RGB, 
            ClampingMode clampingMode = ClampingMode::REPEAT, const glm::vec3& borderColor = glm::vec3(1.0f));

        void Create(int width, int height, float* rgbData, TextureFormat startFormat = TextureFormat::RGB, TextureFormat endFormat = TextureFormat::RGB,
            ClampingMode clampingMode = ClampingMode::REPEAT, const glm::vec3& = glm::vec3(1.0f));

        void LoadFromFile(const std::string& path, bool gammaCorrection = true);
        void LoadFromFileAsync(const std::string& path, bool gammaCorrection = true);

        void Bind(unsigned int slot = 0) const;

        void Destroy();

        inline unsigned int GetId() const { return ID; }

    };
}