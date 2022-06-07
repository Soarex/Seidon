#include "Texture.h"

#include "Core/Application.h"
#include "Core/WorkManager.h"

#include "../Debug/Debug.h"

#include <iostream>
#include <fstream>

namespace Seidon
{
    Texture* Texture::temporaryTexture = nullptr;
    
    Texture::Texture(UUID id)
    {
        path = "";
        this->id = id;
    }

    Texture::~Texture()
    {
        Destroy();
    }

    void Texture::Bind(unsigned int slot) const
    {
        SD_ASSERT(initialized, "Texture not initialized");

        GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, renderId));
    }

    void Texture::Create(int width, int height, unsigned char* rgbData, TextureFormat sourceFormat, TextureFormat internalFormat,
        ClampingMode clampingMode, const glm::vec3& borderColor, bool anisotropicFiltering)
    {
        SD_ASSERT(!initialized, "Texture already initialized");

        this->width = width;
        this->height = height;
        this->format = sourceFormat;
        this->clampingMode = clampingMode;

        GL_CHECK(glGenTextures(1, &renderId));
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, renderId));

        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)internalFormat, width, height, 0, (GLenum)sourceFormat, GL_UNSIGNED_BYTE, rgbData));
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, anisotropicFiltering ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)clampingMode));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)clampingMode));

        float color[] = { borderColor.x, borderColor.y, borderColor.z, 1.0f };
        GL_CHECK(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color));

        renderHandle = GL_CHECK(glGetTextureHandleARB(renderId));

        initialized = true;
    }

    void Texture::Create(int width, int height, int* rgbData, TextureFormat sourceFormat, TextureFormat internalFormat,
        ClampingMode clampingMode, const glm::vec3& borderColor)
    {
        SD_ASSERT(!initialized, "Texture already initialized");

        this->width = width;
        this->height = height;
        this->format = sourceFormat;
        this->clampingMode = clampingMode;

        GL_CHECK(glGenTextures(1, &renderId));
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, renderId));

        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)internalFormat, width, height, 0, (GLenum)sourceFormat, GL_INT, rgbData));

        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)clampingMode));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)clampingMode));

        float color[] = { borderColor.x, borderColor.y, borderColor.z, 1.0f };
        GL_CHECK(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color));

        //renderHandle = GL_CHECK(glGetTextureHandleARB(renderId));

        initialized = true;
    }

    void Texture::Create(int width, int height, float* rgbData, TextureFormat sourceFormat, TextureFormat internalFormat,
        ClampingMode clampingMode, const glm::vec3& borderColor)
    {
        SD_ASSERT(!initialized, "Texture already initialized");

        this->width = width;
        this->height = height;
        this->format = sourceFormat;
        this->clampingMode = clampingMode;

        GL_CHECK(glGenTextures(1, &renderId));
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, renderId));
   
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)internalFormat, width, height, 0, (GLenum)sourceFormat, GL_FLOAT, rgbData));
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
  
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
      
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)clampingMode));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)clampingMode));

        float color[] = { borderColor.x, borderColor.y, borderColor.z, 1.0f };
        GL_CHECK(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color));

        //renderHandle = GL_CHECK(glGetTextureHandleARB(renderId));
        initialized = true;
    }

    void Texture::SetClampingMode(ClampingMode clampingMode)
    {
        Bind();
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)clampingMode));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)clampingMode));

        this->clampingMode = clampingMode;
    }

    void Texture::Destroy()
    {
        SD_ASSERT(initialized, "Texture not initialized");

        GL_CHECK(glDeleteTextures(1, &renderId));

        initialized = false;
    }

    void Texture::Save(std::ofstream& out)
    {
        SD_ASSERT(initialized, "Texture not initialized");

        int elementsPerPixel = 0;

        switch (format)
        {
        case TextureFormat::RED:
            elementsPerPixel = 1;
            break;

        case TextureFormat::RED_GREEN:
            elementsPerPixel = 2;
            break;

        case TextureFormat::RGB: case TextureFormat::SRGB:
            elementsPerPixel = 3;
            break;

        case TextureFormat::RGBA: case TextureFormat::SRGBA:
            elementsPerPixel = 4;
            break;
        }

        byte* pixels = new byte[(long long)width * height * elementsPerPixel];

        Bind(0);
        GL_CHECK(glGetTexImage(GL_TEXTURE_2D, 0, (GLenum)format, GL_UNSIGNED_BYTE, pixels));

        out.write((char*)&id, sizeof(UUID));

        size_t size = this->path.length() + 1;
        out.write((char*)&size, sizeof(size_t));
        out.write(this->path.c_str(), size * sizeof(char));

        out.write((char*)&gammaCorrected, sizeof(bool));

        out.write((char*)&width, sizeof(unsigned int));
        out.write((char*)&height, sizeof(unsigned int));
        out.write((char*)&format, sizeof(TextureFormat));
        out.write((char*)&clampingMode, sizeof(ClampingMode));


        out.write((char*)pixels, sizeof(byte) * width * height * elementsPerPixel);

        delete[] pixels;
    }

    void Texture::SaveAsync(const std::string& path)
    {
        SD_ASSERT(initialized, "Texture not initialized");

        int elementsPerPixel = 0;

        switch (format)
        {
        case TextureFormat::RED:
            elementsPerPixel = 1;
            break;

        case TextureFormat::RED_GREEN:
            elementsPerPixel = 2;
            break;

        case TextureFormat::RGB: case TextureFormat::SRGB:
            elementsPerPixel = 3;
            break;

        case TextureFormat::RGBA: case TextureFormat::SRGBA:
            elementsPerPixel = 4;
            break;
        }

        byte* pixels = new byte[(long long)width * height * elementsPerPixel];

        Bind(0);
        GL_CHECK(glGetTexImage(GL_TEXTURE_2D, 0, (GLenum)format, GL_UNSIGNED_BYTE, pixels));
        Application::Get()->GetWorkManager()->Execute([&]()
            {
                std::ofstream out(path, std::ios::out | std::ios::binary);

                out.write((char*)&id, sizeof(UUID));

                size_t size = this->path.length() + 1;
                out.write((char*)&size, sizeof(size_t));
                out.write(this->path.c_str(), size * sizeof(char));

                out.write((char*)&gammaCorrected, sizeof(bool));

                out.write((char*)&width, sizeof(unsigned int));
                out.write((char*)&height, sizeof(unsigned int));
                out.write((char*)&format, sizeof(TextureFormat));

                out.write((char*)pixels, sizeof(byte) * width * height * elementsPerPixel);

                delete[] pixels;
            }
        );
    }

    void Texture::Load(std::ifstream& in)
    {
        SD_ASSERT(!initialized, "Texture already initialized");

        if (!in)
        {
            std::cerr << "Error opening texture file: " << path << std::endl;
            return;
        }

        unsigned int width, height;
        in.read((char*)&id, sizeof(UUID));

        size_t size = 0;
        in.read((char*)&size, sizeof(size_t));

        char buffer[2048];
        in.read(buffer, size * sizeof(char));
        this->path = buffer;

        in.read((char*)&gammaCorrected, sizeof(bool));

        in.read((char*)&width, sizeof(unsigned int));
        in.read((char*)&height, sizeof(unsigned int));

        TextureFormat format;
        in.read((char*)&format, sizeof(TextureFormat));

        in.read((char*)&clampingMode, sizeof(ClampingMode));

        int elementsPerPixel = 0;

        switch (format)
        {
        case TextureFormat::RED:
            elementsPerPixel = 1;
            break;

        case TextureFormat::RED_GREEN:
            elementsPerPixel = 2;
            break;

        case TextureFormat::RGB: case TextureFormat::SRGB:
            elementsPerPixel = 3;
            break;

        case TextureFormat::RGBA: case TextureFormat::SRGBA:
            elementsPerPixel = 4;
            break;
        }

        byte* pixels = new byte[(long long)width * height * elementsPerPixel];

        in.read((char*)pixels, sizeof(byte) * width * height * elementsPerPixel);


        TextureFormat internalFormat = format;

        if (gammaCorrected && format == TextureFormat::RGB) internalFormat = TextureFormat::SRGB;
        if (gammaCorrected && format == TextureFormat::RGBA) internalFormat = TextureFormat::SRGBA;

        Create(width, height, pixels, format, internalFormat, clampingMode);

        delete[] pixels;
    }

    void Texture::LoadAsync(const std::string& path)
    {
        SD_ASSERT(!initialized, "Texture already initialized");

        renderId = Application::Get()->GetResourceManager()->GetAsset<Texture>("albedo_default")->GetRenderId();
        Application::Get()->GetWorkManager()->Execute([&]()
            {
                std::ifstream in(path, std::ios::in | std::ios::binary);

                unsigned int width, height;
                in.read((char*)&id, sizeof(UUID));

                size_t size = 0;
                in.read((char*)&size, sizeof(size_t));

                char buffer[2048];
                in.read(buffer, size * sizeof(char));
                this->path = buffer;

                in.read((char*)&gammaCorrected, sizeof(bool));

                in.read((char*)&width, sizeof(unsigned int));
                in.read((char*)&height, sizeof(unsigned int));

                TextureFormat format;
                in.read((char*)&format, sizeof(TextureFormat));

                int elementsPerPixel = 0;

                switch (format)
                {
                case TextureFormat::RED:
                    elementsPerPixel = 1;
                    break;

                case TextureFormat::RED_GREEN:
                    elementsPerPixel = 2;
                    break;

                case TextureFormat::RGB: case TextureFormat::SRGB:
                    elementsPerPixel = 3;
                    break;

                case TextureFormat::RGBA: case TextureFormat::SRGBA:
                    elementsPerPixel = 4;
                    break;
                }

                byte* pixels = new byte[(long long)width * height * elementsPerPixel];

                in.read((char*)pixels, sizeof(byte) * width * height * elementsPerPixel);


                TextureFormat internalFormat = format;

                if (gammaCorrected && format == TextureFormat::RGB) internalFormat = TextureFormat::SRGB;
                if (gammaCorrected && format == TextureFormat::RGBA) internalFormat = TextureFormat::SRGBA;

                Application::Get()->GetWorkManager()->ExecuteOnMainThread([=]()
                    {
                        Create(width, height, pixels, format, internalFormat);
                        delete[] pixels;
                    }
                );
            }
        );
    }

    bool Texture::Import(const std::string& path, bool gammaCorrection, ClampingMode clampingMode)
    {
        SD_ASSERT(!initialized, "Texture already initialized");

        this->path = path;
        gammaCorrected = gammaCorrection;
        int width, height, channelCount;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channelCount, 0);

        TextureFormat internalFormat = TextureFormat::RGB;
        if (channelCount == 1)
            internalFormat = TextureFormat::RED;
        else if (channelCount == 3)
            internalFormat = gammaCorrection ? TextureFormat::SRGB : TextureFormat::RGB;
        else if (channelCount == 4)
            internalFormat = gammaCorrection ? TextureFormat::SRGBA : TextureFormat::RGBA;

        TextureFormat sourceFormat = TextureFormat::RGB;
        if (channelCount == 1)
            sourceFormat = TextureFormat::RED;
        else if (channelCount == 3)
            sourceFormat = TextureFormat::RGB;
        else if (channelCount == 4)
            sourceFormat = TextureFormat::RGBA;
        
        if (data)
        {
            Create(width, height, data, sourceFormat, internalFormat, clampingMode);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load texture " << path << std::endl;
            return false;
        }

        return true;
    }

    void Texture::ImportAsync(const std::string& path, bool gammaCorrection)
    {
        SD_ASSERT(!initialized, "Texture already initialized");

        this->path = path;
        if (!temporaryTexture)
        {
            temporaryTexture = new Texture();
            unsigned char white[] = { 255, 255, 255 };

            temporaryTexture->path = "Temp";
            temporaryTexture->Create(1, 1, white);
        }

        renderId = temporaryTexture->renderId;
        Application::Get()->GetWorkManager()->Execute([this, path, gammaCorrection]()
            {
                int width, height, channelCount;
                gammaCorrected = gammaCorrection;
              
                unsigned char* data = stbi_load(path.c_str(), &width, &height, &channelCount, 0);

                TextureFormat internalFormat = TextureFormat::RGB;
                if (channelCount == 1)
                    internalFormat = TextureFormat::RED;
                else if (channelCount == 3)
                    internalFormat = gammaCorrection ? TextureFormat::SRGB : TextureFormat::RGB;
                else if (channelCount == 4)
                    internalFormat = gammaCorrection ? TextureFormat::SRGBA : TextureFormat::RGBA;

                TextureFormat sourceFormat = TextureFormat::RGB;
                if (channelCount == 1)
                    sourceFormat = TextureFormat::RED;
                else if (channelCount == 3)
                    sourceFormat = TextureFormat::RGB;
                else if (channelCount == 4)
                    sourceFormat = TextureFormat::RGBA;

                if (data)
                {
                    Application::Get()->GetWorkManager()->ExecuteOnMainThread([this, width, height, sourceFormat, internalFormat, data]()
                        {
                            Create(width, height, data, sourceFormat, internalFormat);
                            stbi_image_free(data);
                        }
                    );
                }
                else
                {
                    std::cout << "Failed to load texture " << path << std::endl;
                }
            }
        );
    }
   
    void Texture::MakeResident()
    {
        if (!isResident)
        {
            GL_CHECK(glMakeTextureHandleResidentARB(renderHandle));
            isResident = true;
        }
    }

    void Texture::MakeNonResident()
    {
        GL_CHECK(glMakeTextureHandleNonResidentARB(renderHandle));
    }
}