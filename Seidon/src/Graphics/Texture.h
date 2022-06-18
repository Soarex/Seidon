#pragma once
#include "../Core/UUID.h"
#include "../Core/Asset.h"

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
        RED_INTEGER = GL_RED_INTEGER,
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
        INT32 = GL_R32I,
    };

    enum class ClampingMode
    {
        CLAMP = GL_CLAMP_TO_EDGE,
        REPEAT = GL_REPEAT,
        BORDER = GL_CLAMP_TO_BORDER
    };

    class Texture : public Asset
    {
    public:
        std::string path;
    
    public:
        Texture(UUID id = UUID());
        ~Texture();

        void Create(int width, int height, unsigned char* rgbData, TextureFormat sourceFormat = TextureFormat::RGB, TextureFormat internalFormat = TextureFormat::RGB,
            ClampingMode clampingMode = ClampingMode::CLAMP, const glm::vec3& borderColor = glm::vec3(1.0f), bool anisotropicFiltering = true);

        void Create(int width, int height, int* rgbData, TextureFormat sourceFormat = TextureFormat::RGB, TextureFormat internalFormat = TextureFormat::RGB,
            ClampingMode clampingMode = ClampingMode::CLAMP, const glm::vec3& borderColor = glm::vec3(1.0f));

        void Create(int width, int height, float* rgbData, TextureFormat sourceFormat = TextureFormat::RGB, TextureFormat internalFormat = TextureFormat::RGB,
            ClampingMode clampingMode = ClampingMode::CLAMP, const glm::vec3& = glm::vec3(1.0f));

        void SetClampingMode(ClampingMode clampingMode);

        using Asset::Save;
        using Asset::Load;

        void Save(std::ofstream& out) override;
        void SaveAsync(const std::string& path);

        void Load(std::ifstream& in) override;
        void LoadAsync(const std::string& path);

        bool Import(const std::string& path, bool gammaCorrection = true, bool flip = true, ClampingMode clampingMode = ClampingMode::CLAMP);
        void ImportAsync(const std::string& path, bool gammaCorrection = true);

        void Bind(unsigned int slot = 0) const;
        void MakeResident();
        void MakeNonResident();

        void Destroy();

        inline UUID GetId() const { return id; }
        inline unsigned int GetRenderId() const { return renderId; }
        inline uint64_t GetRenderHandle() const { return renderHandle; }
        inline const std::string& GetPath() { return path; }

        inline unsigned int GetWidth() { return width; }
        inline unsigned int GetHeight() { return height; }

    private:
        bool initialized = false;

        unsigned int renderId;
        uint64_t renderHandle;

        unsigned int width, height;
        TextureFormat format;
        ClampingMode clampingMode;
        bool gammaCorrected = false;
        bool isResident = false;

        static Texture* temporaryTexture;

        friend class ResourceManager;
    };
}