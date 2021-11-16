#include "HdrCubemap.h"

namespace Seidon
{
    HdrCubemap::HdrCubemap(unsigned int faceSize, unsigned int irradianceMapSize, unsigned int prefilteredMapSize, unsigned int BRDFLookupSize)
        : faceSize(faceSize), irradianceMapSize(irradianceMapSize), prefilteredMapSize(prefilteredMapSize), BRDFLookupSize(BRDFLookupSize)
    {

    }

    void HdrCubemap::CreateFromEquirectangularMap(Texture* texture)
    {
        filepath = texture->GetPath();
        ToCubemap(*texture);
        GenerateIrradianceMap();
        GeneratePrefilteredMap();
        GenerateBRDFLookupMap();
    }

	void HdrCubemap::LoadFromEquirectangularMap(std::string path)
	{
        filepath = path;
        stbi_set_flip_vertically_on_load(true);
        int width, height, channelCount;
        float* data = stbi_loadf(path.c_str(), &width, &height, &channelCount, 0);

        if (data)
        {
            Texture hdrTexture;

            hdrTexture.Create(width, height, data, TextureFormat::FLOAT16, TextureFormat::RGB, ClampingMode::CLAMP);
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
	}

    void HdrCubemap::BindSkybox(unsigned int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);
    }

    void HdrCubemap::BindIrradianceMap(unsigned int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapID);
    }

    void HdrCubemap::BindPrefilteredMap(unsigned int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMapID);
    }

    void HdrCubemap::BindBRDFLookupMap(unsigned int slot)
    {
        BRDFLookupMap.Bind(slot);
    }

    void HdrCubemap::ToCubemap(Texture& equirectangularMap)
    {
        glGenTextures(1, &skyboxID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);
        for (unsigned int i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, faceSize, faceSize, 0, GL_RGB, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


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

        conversionFramebuffer.SetDepthStencilRenderBuffer(depthAttachment);

        Shader conversionShader;
        conversionShader.LoadFromFile("Shaders/EquirectangularConversion.shader");
        conversionShader.Use();
        conversionShader.SetInt("equirectangularMap", 0);
        conversionShader.SetMat4("projectionMatrix", projectionMatrix);
        equirectangularMap.Bind();

        glViewport(0, 0, faceSize, faceSize);
        conversionFramebuffer.Bind();

        for (unsigned int i = 0; i < 6; ++i)
        {
            conversionShader.SetMat4("viewMatrix", viewMatrices[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skyboxID, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            DrawCaptureCube();
        }
        conversionFramebuffer.Unbind();

        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    void HdrCubemap::GenerateIrradianceMap()
    {
        glGenTextures(1, &irradianceMapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapID);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, irradianceMapSize, irradianceMapSize, 0,
                GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
        convolutionFramebuffer.SetDepthStencilRenderBuffer(depthAttachment);

        Shader convolutionShader;
        convolutionShader.LoadFromFile("Shaders/Convolution.shader");
        convolutionShader.Use();
        convolutionShader.SetInt("environmentMap", 0);
        convolutionShader.SetFloat("faceResolution", faceSize);
        convolutionShader.SetMat4("projectionMatrix", projectionMatrix);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);

        glViewport(0, 0, irradianceMapSize, irradianceMapSize);

        convolutionFramebuffer.Bind();

        for (unsigned int i = 0; i < 6; ++i)
        {
            convolutionShader.SetMat4("viewMatrix", viewMatrices[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMapID, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            DrawCaptureCube();
        }

        convolutionFramebuffer.Unbind();
    }

    void HdrCubemap::GeneratePrefilteredMap()
    {
        glGenTextures(1, &prefilteredMapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMapID);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, prefilteredMapSize, prefilteredMapSize, 0,
                GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);

        glViewport(0, 0, prefilteredMapSize, prefilteredMapSize);

        convolutionFramebuffer.Bind();
        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
        {
            unsigned int mipWidth = prefilteredMapSize * std::pow(0.5, mip);
            unsigned int mipHeight = prefilteredMapSize * std::pow(0.5, mip);

            RenderBuffer depthAttachment;
            depthAttachment.Create(mipWidth, mipHeight, RenderBufferType::DEPTH_STENCIL);
            convolutionFramebuffer.SetDepthStencilRenderBuffer(depthAttachment);

            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            convolutionShader.SetFloat("roughness", roughness);

            for (unsigned int i = 0; i < 6; ++i)
            {
                convolutionShader.SetMat4("viewMatrix", viewMatrices[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilteredMapID, mip);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                DrawCaptureCube();
            }
        }

        convolutionFramebuffer.Unbind();
    }

    void HdrCubemap::GenerateBRDFLookupMap()
    {
        BRDFLookupMap.Create(BRDFLookupSize, BRDFLookupSize, (float*)NULL, TextureFormat::FLOAT16_RED_GREEN, TextureFormat::RED_GREEN, ClampingMode::CLAMP);
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
        
        glViewport(0, 0, BRDFLookupSize, BRDFLookupSize);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDeleteBuffers(1, &cubeVBO);
        glDeleteVertexArrays(1, &cubeVAO);
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
        
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glDeleteBuffers(1, &quadVBO);
        glDeleteVertexArrays(1, &quadVAO);
    }

}