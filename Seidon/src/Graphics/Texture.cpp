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
        Destroy();
    }

    void Texture::Bind(unsigned int slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, renderId);
    }

    void Texture::Create(int width, int height, unsigned char* rgbData, TextureFormat sourceFormat, TextureFormat internalFormat,
        ClampingMode clampingMode, const glm::vec3& borderColor)
    {
        this->width = width;
        this->height = height;
        this->format = sourceFormat;

        glGenTextures(1, &renderId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderId);

        glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)internalFormat, width, height, 0, (GLenum)sourceFormat, GL_UNSIGNED_BYTE, rgbData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)clampingMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)clampingMode);
        float color[] = { borderColor.x, borderColor.y, borderColor.z, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    }

    void Texture::Create(int width, int height, float* rgbData, TextureFormat sourceFormat, TextureFormat internalFormat,
        ClampingMode clampingMode, const glm::vec3& borderColor)
    {
        this->width = width;
        this->height = height;
        this->format = sourceFormat;

        glGenTextures(1, &renderId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderId);

        glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)internalFormat, width, height, 0, (GLenum)sourceFormat, GL_FLOAT, rgbData);
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

        Byte* pixels = new Byte[(long long)width * height * elementsPerPixel];
        
        Bind(0);
        glGetTexImage(GL_TEXTURE_2D, 0, (GLenum)format, GL_UNSIGNED_BYTE, pixels);

        std::ofstream out(path, std::ios::out | std::ios::binary);

        out.write((char*)&id, sizeof(UUID));

        size_t size = this->path.length() + 1;
        out.write((char*)&size, sizeof(size_t));
        out.write(this->path.c_str(), size * sizeof(char));

        out.write((char*)&gammaCorrected, sizeof(bool));

        out.write((char*)&width, sizeof(unsigned int));
        out.write((char*)&height, sizeof(unsigned int));
        out.write((char*)&format, sizeof(TextureFormat));

        out.write((char*)pixels, sizeof(Byte) * width * height * elementsPerPixel);

        delete[] pixels;
    }

    void Texture::SaveAsync(const std::string& path)
    {
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

        Byte* pixels = new Byte[(long long)width * height * elementsPerPixel];

        Bind(0);
        glGetTexImage(GL_TEXTURE_2D, 0, (GLenum)format, GL_UNSIGNED_BYTE, pixels);
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

                out.write((char*)pixels, sizeof(Byte) * width * height * elementsPerPixel);

                delete[] pixels;
            }
        );
    }

    void Texture::Load(const std::string& path)
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

        Byte* pixels = new Byte[(long long)width * height * elementsPerPixel];

        in.read((char*)pixels, sizeof(Byte) * width * height * elementsPerPixel);


        TextureFormat internalFormat = format;

        if (gammaCorrected && format == TextureFormat::RGB) internalFormat = TextureFormat::SRGB;
        if (gammaCorrected && format == TextureFormat::RGBA) internalFormat = TextureFormat::SRGBA;

        Create(width, height, pixels, format, internalFormat);

        delete[] pixels;
    }

    void Texture::LoadAsync(const std::string& path)
    {
        renderId = Application::Get()->GetResourceManager()->GetTexture("albedo_default")->GetRenderId();
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

                Byte* pixels = new Byte[(long long)width * height * elementsPerPixel];

                in.read((char*)pixels, sizeof(Byte) * width * height * elementsPerPixel);


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

    void Texture::Import(const std::string& path, bool gammaCorrection)
    {
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
            Create(width, height, data, sourceFormat, internalFormat);
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
}