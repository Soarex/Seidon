#include "Texture.h"

#include "Core/Application.h"
#include "Core/WorkManager.h"

namespace Seidon
{
    Texture* Texture::temporaryTexture = nullptr;
    Texture::Texture()
    {
        path = "";
    }
    Texture::~Texture()
    {
        glDeleteTextures(1, &renderId);
    }

    void Texture::Bind(unsigned int slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, renderId);
    }

    void Texture::Create(int width, int height, unsigned char* rgbData, TextureFormat startFormat, TextureFormat endFormat,
        ClampingMode clampingMode, const glm::vec3& borderColor)
    {
        this->width = width;
        this->height = height;
        this->format = endFormat;

        glGenTextures(1, &renderId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderId);

        glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)startFormat, width, height, 0, (GLenum)endFormat, GL_UNSIGNED_BYTE, rgbData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)clampingMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)clampingMode);
        float color[] = { borderColor.x, borderColor.y, borderColor.z, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    }

    void Texture::Create(int width, int height, float* rgbData, TextureFormat startFormat, TextureFormat endFormat,
        ClampingMode clampingMode, const glm::vec3& borderColor)
    {
        this->width = width;
        this->height = height;
        this->format = endFormat;

        glGenTextures(1, &renderId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderId);

        glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)startFormat, width, height, 0, (GLenum)endFormat, GL_FLOAT, rgbData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)clampingMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)clampingMode);
        float color[] = { borderColor.x, borderColor.y, borderColor.z, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

    }

    void Texture::Destroy()
    {
        glDeleteTextures(1, &renderId);
    }

    void Texture::Save(const std::string& path)
    {
        int elementsPerPixel = 0;

        switch (format)
        {
        case TextureFormat::RGB:
            elementsPerPixel = 3;
            break;

        case TextureFormat::RGBA:
            elementsPerPixel = 4;
            break;
        }

        Byte* pixels = new Byte[(long long)width * height * elementsPerPixel];
        
        Bind(0);
        glGetTexImage(GL_TEXTURE_2D, 0, (GLenum)format, GL_UNSIGNED_BYTE, pixels);

        std::ofstream out(path, std::ios::out | std::ios::binary);

        out.write((char*)&id, sizeof(UUID));
        out.write((char*)&width, sizeof(unsigned int));
        out.write((char*)&height, sizeof(unsigned int));
        out.write((char*)&format, sizeof(TextureFormat));

        out.write((char*)pixels, sizeof(Byte) * width * height * elementsPerPixel);

        delete[] pixels;
    }

    void Texture::Load(const std::string& path)
    {
        std::ifstream in(path, std::ios::in | std::ios::binary);

        unsigned int width, height;
        in.read((char*)&id, sizeof(UUID));
        in.read((char*)&width, sizeof(unsigned int));
        in.read((char*)&height, sizeof(unsigned int));

        TextureFormat format;
        in.read((char*)&format, sizeof(TextureFormat));

        int elementsPerPixel = 0;

        switch (format)
        {
        case TextureFormat::RGB:
            elementsPerPixel = 3;
            break;

        case TextureFormat::RGBA:
            elementsPerPixel = 4;
            break;
        }

        Byte* pixels = new Byte[(long long)width * height * elementsPerPixel];

        in.read((char*)pixels, sizeof(Byte) * width * height * elementsPerPixel);

        Create(width, height, pixels, format, format);

        delete[] pixels;
    }

    void Texture::Import(const std::string& path, bool gammaCorrection)
    {
        this->path = path;
        int width, height, channelCount;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channelCount, 0);

        TextureFormat sourceFormat = TextureFormat::RGB;
        if (channelCount == 1)
            sourceFormat = TextureFormat::RED;
        else if (channelCount == 3)
            sourceFormat = gammaCorrection ? TextureFormat::SRGB : TextureFormat::RGB;
        else if (channelCount == 4)
            sourceFormat = gammaCorrection ? TextureFormat::SRGBA : TextureFormat::RGBA;

        TextureFormat storageFormat = TextureFormat::RGB;
        if (channelCount == 1)
            storageFormat = TextureFormat::RED;
        else if (channelCount == 3)
            storageFormat = TextureFormat::RGB;
        else if (channelCount == 4)
            storageFormat = TextureFormat::RGBA;
        
        if (data)
        {
            Create(width, height, data, sourceFormat, storageFormat);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load texture " << path << std::endl;
        }
    }

    void Texture::ImportAsync(const std::string& path, bool gammaCorrection)
    {
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
              
                unsigned char* data = stbi_load(path.c_str(), &width, &height, &channelCount, 0);

                TextureFormat sourceFormat = TextureFormat::RGB;
                if (channelCount == 1)
                    sourceFormat = TextureFormat::RED;
                else if (channelCount == 3)
                    sourceFormat = gammaCorrection ? TextureFormat::SRGB : TextureFormat::RGB;
                else if (channelCount == 4)
                    sourceFormat = gammaCorrection ? TextureFormat::SRGBA : TextureFormat::RGBA;

                TextureFormat storageFormat = TextureFormat::RGB;
                if (channelCount == 1)
                    storageFormat = TextureFormat::RED;
                else if (channelCount == 3)
                    storageFormat = TextureFormat::RGB;
                else if (channelCount == 4)
                    storageFormat = TextureFormat::RGBA;

                if (data)
                {
                    Application::Get()->GetWorkManager()->ExecuteOnMainThread([this, width, height, sourceFormat, storageFormat, data]()
                        {
                            Create(width, height, data, sourceFormat, storageFormat);
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
}