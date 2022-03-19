#include "HdrCubemap.h"

#include "../Debug/Debug.h"

namespace Seidon
{
    HdrCubemap::HdrCubemap(UUID id, unsigned int faceSize, unsigned int irradianceMapSize, unsigned int prefilteredMapSize, unsigned int BRDFLookupSize)
        : id(id), faceSize(faceSize), irradianceMapSize(irradianceMapSize), prefilteredMapSize(prefilteredMapSize), BRDFLookupSize(BRDFLookupSize)
    {

    }

    HdrCubemap::HdrCubemap(unsigned int faceSize, unsigned int irradianceMapSize, unsigned int prefilteredMapSize, unsigned int BRDFLookupSize)
        : faceSize(faceSize), irradianceMapSize(irradianceMapSize), prefilteredMapSize(prefilteredMapSize), BRDFLookupSize(BRDFLookupSize)
    {

    }

    HdrCubemap::~HdrCubemap()
    {
        Destroy();
    }

    void HdrCubemap::Destroy()
    {
        SD_ASSERT(initialized, "Cubemap not initialized");

        GL_CHECK(glDeleteTextures(1, &skyboxID));
        GL_CHECK(glDeleteTextures(1, &irradianceMapID));
        GL_CHECK(glDeleteTextures(1, &prefilteredMapID));

        initialized = false;
    }

    void HdrCubemap::Save(const std::string& path)
    {
        SD_ASSERT(initialized, "Cubemap not initialized");

        std::ofstream out(path, std::ios::out | std::ios::binary);
        
        out.write((char*)&id, sizeof(UUID));

        size_t size = filepath.length() + 1;
        out.write((char*)&size, sizeof(size_t));
        out.write(filepath.c_str(), size * sizeof(char));

        SaveCubemap(out);
        SaveIrradianceMap(out);
        SavePrefilteredMap(out);

        float* pixels = new float[(long long)BRDFLookupSize * BRDFLookupSize * 2];

        BRDFLookupMap.Bind(0);
        GL_CHECK(glGetTexImage(GL_TEXTURE_2D, 0, (GLenum)TextureFormat::RED_GREEN, GL_FLOAT, pixels));

        out.write((char*)&BRDFLookupSize, sizeof(unsigned int));
        out.write((char*)pixels, sizeof(float) * BRDFLookupSize * BRDFLookupSize * 2);

        delete[] pixels;
    }

    void HdrCubemap::SaveCubemap(std::ofstream& out)
    {
        int elementsPerPixel = 3;
        float* pixels = new float[(long long)faceSize * faceSize * elementsPerPixel];

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID));

        out.write((char*)&faceSize, sizeof(unsigned int));
        for (unsigned int i = 0; i < 6; ++i)
        {
            GL_CHECK(glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, GL_FLOAT, pixels));
            out.write((char*)pixels, sizeof(float) * faceSize * faceSize * elementsPerPixel);
        }

        delete[] pixels;
    }

    void HdrCubemap::SaveIrradianceMap(std::ofstream& out)
    {
        int elementsPerPixel = 3;
        float* pixels = new float[(long long)irradianceMapSize * irradianceMapSize * elementsPerPixel];

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapID));

        out.write((char*)&irradianceMapSize, sizeof(unsigned int));
        for (unsigned int i = 0; i < 6; ++i)
        {
            GL_CHECK(glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, GL_FLOAT, pixels));
            out.write((char*)pixels, sizeof(float) * irradianceMapSize * irradianceMapSize * elementsPerPixel);
        }

        delete[] pixels;
    }

    void HdrCubemap::SavePrefilteredMap(std::ofstream& out)
    {
        int elementsPerPixel = 3;
        float* pixels = new float[(long long)prefilteredMapSize * prefilteredMapSize * elementsPerPixel];

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMapID));
        
        out.write((char*)&prefilteredMapSize, sizeof(unsigned int));

        for (unsigned int mip = 0; mip < maxMipLevels; mip++)
        {
            unsigned int mipSize = prefilteredMapSize * std::pow(0.5, mip);

            for (unsigned int i = 0; i < 6; ++i)
            {
                GL_CHECK(glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, GL_RGB, GL_FLOAT, pixels));
                out.write((char*)pixels, sizeof(float) * mipSize * mipSize * elementsPerPixel);
            }
        }
        delete[] pixels;
    }

    void HdrCubemap::Load(const std::string& path)
    {
        SD_ASSERT(!initialized, "Cubemap alrady initialized");

        std::ifstream in(path, std::ios::in | std::ios::binary);

        if (!in)
        {
            std::cerr << "Error opening cubemap file: " << path << std::endl;
            return;
        }

        in.read((char*)&id, sizeof(UUID));

        size_t size = 0;
        in.read((char*)&size, sizeof(size_t));

        char buffer[2048];
        in.read(buffer, size * sizeof(char));
        filepath = buffer;

        LoadCubemap(in);
        LoadIrradianceMap(in);
        LoadPrefilteredMap(in);

        in.read((char*)&BRDFLookupSize, sizeof(unsigned int));
        
        float* pixels = new float[(long long)BRDFLookupSize * BRDFLookupSize * 2];
        in.read((char*)pixels, sizeof(float) * BRDFLookupSize * BRDFLookupSize * 2);

        BRDFLookupMap.Create(BRDFLookupSize, BRDFLookupSize, pixels, TextureFormat::RED_GREEN, TextureFormat::FLOAT16_RED_GREEN, ClampingMode::CLAMP);
        delete[] pixels;

        initialized = true;
    }

    void HdrCubemap::LoadCubemap(std::ifstream& in)
    {
        int elementsPerPixel = 3;

        GL_CHECK(glGenTextures(1, &skyboxID));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID));

        in.read((char*)&faceSize, sizeof(unsigned int));
        float* pixels = new float[(long long)faceSize * faceSize * 3];

        for (unsigned int i = 0; i < 6; ++i)
        {
            in.read((char*)pixels, sizeof(float) * faceSize * faceSize * elementsPerPixel);
            GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, faceSize, faceSize, 0, GL_RGB, GL_FLOAT, pixels));
        }

        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

        delete[] pixels;
    }

    
    void HdrCubemap::LoadIrradianceMap(std::ifstream& in)
    {
        int elementsPerPixel = 3;

        GL_CHECK(glGenTextures(1, &irradianceMapID));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapID));

        in.read((char*)&irradianceMapSize, sizeof(unsigned int));
        float* pixels = new float[(long long)irradianceMapSize * irradianceMapSize * 3];

        for (unsigned int i = 0; i < 6; ++i)
        {
            in.read((char*)pixels, sizeof(float) * irradianceMapSize * irradianceMapSize * elementsPerPixel);
            GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, irradianceMapSize, irradianceMapSize, 0, GL_RGB, GL_FLOAT, pixels));
        }

        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        delete[] pixels;
    }

    
    void HdrCubemap::LoadPrefilteredMap(std::ifstream& in)
    {
        int elementsPerPixel = 3;

        GL_CHECK(glGenTextures(1, &prefilteredMapID));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMapID));

        in.read((char*)&prefilteredMapSize, sizeof(unsigned int));
        float* pixels = new float[(long long)prefilteredMapSize * prefilteredMapSize * 3];

        for (unsigned int i = 0; i < 6; i++)
            GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, prefilteredMapSize, prefilteredMapSize, 0, GL_RGB, GL_FLOAT, NULL));

        GL_CHECK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

        for (unsigned int mip = 0; mip < maxMipLevels; mip++)
        {
            unsigned int mipSize = prefilteredMapSize * std::pow(0.5, mip);

            for (unsigned int i = 0; i < 6; ++i)
            {
                in.read((char*)pixels, sizeof(float) * mipSize * mipSize * elementsPerPixel);
                GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, GL_RGB16F, mipSize, mipSize, 0, GL_RGB, GL_FLOAT, pixels));
            }
        }
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        delete[] pixels;
    }

    void HdrCubemap::CreateFromEquirectangularMap(Texture* texture)
    {
        SD_ASSERT(!initialized, "Cubemap already initialized");

        filepath = texture->GetPath();
        ToCubemap(*texture);
        GenerateIrradianceMap();
        GeneratePrefilteredMap();
        GenerateBRDFLookupMap();

        initialized = true;
    }

	void HdrCubemap::LoadFromEquirectangularMap(std::string path)
	{
        SD_ASSERT(!initialized, "Cubemap already initialized");

        filepath = path;
        stbi_set_flip_vertically_on_load(true);
        int width, height, channelCount;
        float* data = stbi_loadf(path.c_str(), &width, &height, &channelCount, 0);

        if (data)
        {
            Texture hdrTexture;

            hdrTexture.Create(width, height, data, TextureFormat::RGB, TextureFormat::FLOAT16, ClampingMode::CLAMP);
            ToCubemap(hdrTexture);
            GenerateIrradianceMap();
            GeneratePrefilteredMap();
            GenerateBRDFLookupMap();

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load HDR image." << std::endl;
        }

        stbi_set_flip_vertically_on_load(false);

        initialized = true;
	}

    void HdrCubemap::BindSkybox(unsigned int slot)
    {
        SD_ASSERT(initialized, "Cubemap not initialized");

        GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID));
    }

    void HdrCubemap::BindIrradianceMap(unsigned int slot)
    {
        SD_ASSERT(initialized, "Cubemap not initialized");

        GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapID));
    }

    void HdrCubemap::BindPrefilteredMap(unsigned int slot)
    {
        SD_ASSERT(initialized, "Cubemap not initialized");

        GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMapID));
    }

    void HdrCubemap::BindBRDFLookupMap(unsigned int slot)
    {
        SD_ASSERT(initialized, "Cubemap not initialized");

        BRDFLookupMap.Bind(slot);
    }

    void HdrCubemap::ToCubemap(Texture& equirectangularMap)
    {
        GL_CHECK(glGenTextures(1, &skyboxID));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID));
        for (unsigned int i = 0; i < 6; ++i)
            GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, faceSize, faceSize, 0, GL_RGB, GL_FLOAT, nullptr));

        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));


        glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 viewMatrices[] =
        {
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        RenderBuffer depthAttachment;
        depthAttachment.Create(faceSize, faceSize, RenderBufferType::DEPTH_STENCIL);

        Framebuffer conversionFramebuffer;
        conversionFramebuffer.Create();

        Shader conversionShader;
        conversionShader.LoadFromFile("Shaders/EquirectangularConversion.shader");
        conversionShader.Use();
        conversionShader.SetInt("equirectangularMap", 0);
        conversionShader.SetMat4("projectionMatrix", projectionMatrix);
        equirectangularMap.Bind();

        GL_CHECK(glViewport(0, 0, faceSize, faceSize));
        conversionFramebuffer.Bind();

        for (unsigned int i = 0; i < 6; ++i)
        {
            conversionShader.SetMat4("viewMatrix", viewMatrices[i]);
            
            GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skyboxID, 0));
            
            conversionFramebuffer.SetDepthStencilRenderBuffer(depthAttachment);

            GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            DrawCaptureCube();
        }
        conversionFramebuffer.Unbind();

        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID));
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
    }

    void HdrCubemap::GenerateIrradianceMap()
    {
        GL_CHECK(glGenTextures(1, &irradianceMapID));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapID));
        for (unsigned int i = 0; i < 6; ++i)
        {
            GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, irradianceMapSize, irradianceMapSize, 0,
                GL_RGB, GL_FLOAT, nullptr));
        }
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 viewMatrices[] =
        {
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        RenderBuffer depthAttachment;
        depthAttachment.Create(irradianceMapSize, irradianceMapSize, RenderBufferType::DEPTH_STENCIL);

        Framebuffer convolutionFramebuffer;
        convolutionFramebuffer.Create();

        Shader convolutionShader;
        convolutionShader.LoadFromFile("Shaders/Convolution.shader");
        convolutionShader.Use();
        convolutionShader.SetInt("environmentMap", 0);
        convolutionShader.SetFloat("faceResolution", faceSize);
        convolutionShader.SetMat4("projectionMatrix", projectionMatrix);
        
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID));

        GL_CHECK(glViewport(0, 0, irradianceMapSize, irradianceMapSize));

        convolutionFramebuffer.Bind();

        for (unsigned int i = 0; i < 6; ++i)
        {
            convolutionShader.SetMat4("viewMatrix", viewMatrices[i]);
            GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMapID, 0));

            convolutionFramebuffer.SetDepthStencilRenderBuffer(depthAttachment);

            GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            DrawCaptureCube();
        }

        convolutionFramebuffer.Unbind();
    }

    void HdrCubemap::GeneratePrefilteredMap()
    {
        GL_CHECK(glGenTextures(1, &prefilteredMapID));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMapID));
        for (unsigned int i = 0; i < 6; ++i)
        {
            GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, prefilteredMapSize, prefilteredMapSize, 0,
                GL_RGB, GL_FLOAT, nullptr));
        }
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GL_CHECK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 viewMatrices[] =
        {
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };


        Framebuffer convolutionFramebuffer;
        convolutionFramebuffer.Create();

        Shader convolutionShader;
        convolutionShader.LoadFromFile("Shaders/PrefilterConvolution.shader");
        convolutionShader.Use();
        convolutionShader.SetInt("environmentMap", 0);
        convolutionShader.SetMat4("projectionMatrix", projectionMatrix);

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID));

        GL_CHECK(glViewport(0, 0, prefilteredMapSize, prefilteredMapSize));

        convolutionFramebuffer.Bind();
        
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
        {
            unsigned int mipWidth = prefilteredMapSize * std::pow(0.5, mip);
            unsigned int mipHeight = prefilteredMapSize * std::pow(0.5, mip);

            RenderBuffer depthAttachment;
            depthAttachment.Create(mipWidth, mipHeight, RenderBufferType::DEPTH_STENCIL);
            convolutionFramebuffer.SetDepthStencilRenderBuffer(depthAttachment);

            GL_CHECK(glViewport(0, 0, mipWidth, mipHeight));

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            convolutionShader.SetFloat("roughness", roughness);

            for (unsigned int i = 0; i < 6; ++i)
            {
                convolutionShader.SetMat4("viewMatrix", viewMatrices[i]);
                GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilteredMapID, mip));
                GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

                DrawCaptureCube();
            }
        }

        convolutionFramebuffer.Unbind();
    }

    void HdrCubemap::GenerateBRDFLookupMap()
    {
        BRDFLookupMap.Create(BRDFLookupSize, BRDFLookupSize, (float*)NULL, TextureFormat::RED_GREEN, TextureFormat::FLOAT16_RED_GREEN, ClampingMode::CLAMP);
        RenderBuffer depthAttachment;
        depthAttachment.Create(BRDFLookupSize, BRDFLookupSize, RenderBufferType::DEPTH_STENCIL);

        Framebuffer convolutionFramebuffer;
        convolutionFramebuffer.Create();
        convolutionFramebuffer.SetColorTexture(BRDFLookupMap);
        convolutionFramebuffer.SetDepthStencilRenderBuffer(depthAttachment);
        convolutionFramebuffer.Bind();

        Shader convolutionShader;
        convolutionShader.LoadFromFile("Shaders/BRDFConvolution.shader");
        convolutionShader.Use();
        
        GL_CHECK(glViewport(0, 0, BRDFLookupSize, BRDFLookupSize));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        DrawCaptureQuad();
        convolutionFramebuffer.Unbind();
    }

    void HdrCubemap::DrawCaptureCube()
    {
        const float cubeVertices[] = 
        {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };
        
        unsigned int cubeVAO = 0;
        unsigned int cubeVBO = 0;
        GL_CHECK(glGenVertexArrays(1, &cubeVAO));
        GL_CHECK(glGenBuffers(1, &cubeVBO));
        
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, cubeVBO));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW));
        
        GL_CHECK(glBindVertexArray(cubeVAO));
        GL_CHECK(glEnableVertexAttribArray(0));
        GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GL_CHECK(glBindVertexArray(0));

        GL_CHECK(glBindVertexArray(cubeVAO));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 36));
        GL_CHECK(glBindVertexArray(0));

        GL_CHECK(glDeleteBuffers(1, &cubeVBO));
        GL_CHECK(glDeleteVertexArrays(1, &cubeVAO));
    }

    void HdrCubemap::DrawCaptureQuad()
    {
        unsigned int quadVAO, quadVBO;

        float quadVertices[] = 
        {
            // positions            // texture Coords
            -1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,     1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
        };
        
        GL_CHECK(glGenVertexArrays(1, &quadVAO));
        GL_CHECK(glGenBuffers(1, &quadVBO));
        GL_CHECK(glBindVertexArray(quadVAO));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, quadVBO));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW));
        GL_CHECK(glEnableVertexAttribArray(0));
        GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
        GL_CHECK(glEnableVertexAttribArray(1));
        GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));

        GL_CHECK(glBindVertexArray(quadVAO));
        GL_CHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
        GL_CHECK(glBindVertexArray(0));
        GL_CHECK(glDeleteBuffers(1, &quadVBO));
        GL_CHECK(glDeleteVertexArrays(1, &quadVAO));
    }

}