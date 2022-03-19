#pragma once
#include "../Core/UUID.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <StbImage/stb_image.h> 

#include <string>
#include <iostream>


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
        FLOAT32 = GL_RGBA32F,
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
        bool initialized = false; 

        UUID id;
        unsigned int renderId;

        unsigned int width, height;
        TextureFormat format;
        bool gammaCorrected = false;

        static Texture* temporaryTexture;
    public:
        Texture(UUID id = UUID());
        ~Texture();

        void Create(int width, int height, unsigned char* rgbData, TextureFormat sourceFormat = TextureFormat::RGB, TextureFormat internalFormat = TextureFormat::RGB,
            ClampingMode clampingMode = ClampingMode::REPEAT, const glm::vec3& borderColor = glm::vec3(1.0f));

        void Create(int width, int height, float* rgbData, TextureFormat sourceFormat = TextureFormat::RGB, TextureFormat internalFormat = TextureFormat::RGB,
            ClampingMode clampingMode = ClampingMode::REPEAT, const glm::vec3& = glm::vec3(1.0f));

        void Save(const std::string& path);
        void SaveAsync(const std::string& path);

        void Load(const std::string& path);
        void LoadAsync(const std::string& path);

        void Import(const std::string& path, bool gammaCorrection = true);
        void ImportAsync(const std::string& path, bool gammaCorrection = true);

        void Bind(unsigned int slot = 0) const;

        void Destroy();

        inline UUID GetId() const { return id; }
        inline unsigned int GetRenderId() const { return renderId; }
        inline const std::string& GetPath() { return path; }

        friend class ResourceManager;
    };
}