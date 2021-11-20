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
        glGenTextures(1, &renderId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderId);

        glTexImage2D(GL_TEXTURE_2D, 0, (int)startFormat, width, height, 0, (int)endFormat, GL_UNSIGNED_BYTE, rgbData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int)clampingMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int)clampingMode);
        float color[] = { borderColor.x, borderColor.y, borderColor.z, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    }

    void Texture::Create(int width, int height, float* rgbData, TextureFormat startFormat, TextureFormat endFormat,
        ClampingMode clampingMode, const glm::vec3& borderColor)
    {
        glGenTextures(1, &renderId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderId);

        glTexImage2D(GL_TEXTURE_2D, 0, (int)startFormat, width, height, 0, (int)endFormat, GL_FLOAT, rgbData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int)clampingMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int)clampingMode);
        float color[] = { borderColor.x, borderColor.y, borderColor.z, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    }

    void Texture::Destroy()
    {
        glDeleteTextures(1, &renderId);
    }

    void Texture::LoadFromFile(const std::string& path, bool gammaCorrection)
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

    void Texture::LoadFromFileAsync(const std::string& path, bool gammaCorrection)
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