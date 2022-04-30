#include "Shader.h"

#include "Core/Application.h"
#include "Core/WorkManager.h"

#include "../Reflection/Reflection.h"
#include "../Utils/StringUtils.h"
#include "../Debug/Debug.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <fstream>
#include <sstream>
#include <iostream>

namespace Seidon
{
    Shader* Shader::temporaryShader = nullptr;

    Shader::Shader(UUID id)
        : id(id) 
    {
        bufferLayout = new MetaType();
    }

    Shader::Shader(const std::string& vertexShaderCode, const std::string& fragmentShaderCode)
    {
        CreateFromSource(vertexShaderCode, fragmentShaderCode);
    }

    Shader::~Shader()
    {
        Destroy();
    }

    void Shader::Destroy()
    {
        SD_ASSERT(initialized, "Shader not initialized");

        GL_CHECK(glDeleteProgram(renderId));
    }

    void Shader::ReadLayout(std::ifstream& stream)
    {
        char buffer[512];
        bool endFound = false;
        int offset = 0;

        while (stream.getline(buffer, 512))
        {
            if (buffer[0] == '~')
            {
                if (strcmp(buffer, "~END LAYOUT") == 0)
                {
                    endFound = true;
                    break;
                }
                continue;
            }

            char* split = strchr(buffer, ':');

            if (!split)
            {
                std::cerr << "Shader buffer layout syntax error: Illformed attribute" << std::endl;
                return;
            }

            *split = '\0';

            char* namePtr = buffer;
            char* typePtr = split + 1;

            std::string typeString = RemoveLeadingAndEndingSpaces(typePtr);

            MemberData member;
            member.name = RemoveLeadingAndEndingSpaces(namePtr);
            member.type = MetaType::StringToType(typeString);

            switch (member.type)
            {
            case Types::FLOAT: case Types::FLOAT_NORMALIZED:
                member.size = sizeof(float);
                member.offset = offset;
                offset += member.size;
                break;

            case Types::VECTOR3_COLOR:
                member.size = sizeof(glm::vec3);
                member.offset = offset;
                offset += member.size;
                break;

            case Types::TEXTURE:
                member.size = sizeof(uint64_t);
                member.offset = offset;
                offset += member.size;
                break;

            default:
                std::cerr << "Shader buffer layout syntax error: " << typeString << " is not a type" << std::endl;
                return;
            }

            bufferLayout->members.push_back(member);
        }

        if (!endFound)
        {
            std::cerr << "Shader buffer layout syntax error: End not found" << std::endl;
            return;
        }
    }

    void Shader::LoadFromFile(const std::string& path)
    {
        SD_ASSERT(!initialized, "Shader already initialized");

        this->path = path;
        std::stringstream vertexStream;
        std::stringstream fragmentStream;
        std::ifstream shaderFile;

        try
        {
            char buffer[512];
            std::stringstream* currentStream = nullptr;

            shaderFile.open(path);
            if (!shaderFile.is_open())
            {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_OPENED " << path << std::endl;
                return;
            }

            while (shaderFile.getline(buffer, 512)) 
            {
                if (buffer[0] == '~') 
                {
                    if (strcmp(buffer, "~VERTEX SHADER") == 0) currentStream = &vertexStream;
                    if (strcmp(buffer, "~FRAGMENT SHADER") == 0) currentStream = &fragmentStream;
                    if (strcmp(buffer, "~BEGIN LAYOUT") == 0) ReadLayout(shaderFile);
                    continue;
                }

                (*currentStream) << buffer << "\n";
            }

            shaderFile.close();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ " << path << std::endl;
        }

        std::string vShaderCode = vertexStream.str();
        std::string fShaderCode = fragmentStream.str();

        CreateFromSource(vShaderCode, fShaderCode);

        initialized = true;
    }

    void Shader::LoadFromFileAsync(const std::string& path)
    {
        SD_ASSERT(!initialized, "Shader already initialized");

        this->path = path;

        if (!temporaryShader)
        {
            temporaryShader = new Shader();
            std::string vertexSource = "#version 330 core\n void main() { gl_Position = vec4(0); } ";
            std::string fragmentSource = "#version 330 core\n void main() { } ";

            temporaryShader->path = "Temp";
            temporaryShader->CreateFromSource(vertexSource, fragmentSource);
        }

        renderId = temporaryShader->renderId;
        Application::Get()->GetWorkManager()->Execute([this, path]()
            {
                std::stringstream vertexStream;
                std::stringstream fragmentStream;
                std::ifstream shaderFile;

                try
                {
                    char buffer[512];
                    std::stringstream* currentStream = nullptr;

                    shaderFile.open(path);
                    if (!shaderFile.is_open())
                    {
                        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_OPENED " << path << std::endl;
                        return;
                    }

                    while (shaderFile.getline(buffer, 512)) {
                        if (buffer[0] == '~') {
                            if (strcmp(buffer, "~VERTEX SHADER") == 0) currentStream = &vertexStream;
                            if (strcmp(buffer, "~FRAGMENT SHADER") == 0) currentStream = &fragmentStream;
                            continue;
                        }

                        (*currentStream) << buffer << "\n";
                    }

                    shaderFile.close();
                }
                catch (std::ifstream::failure e)
                {
                    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ " << path << std::endl;
                }

                std::string vShaderCode = vertexStream.str();
                std::string fShaderCode = fragmentStream.str();

                
                Application::Get()->GetWorkManager()->ExecuteOnMainThread([this, vShaderCode, fShaderCode]()
                    {
                        CreateFromSource(vShaderCode, fShaderCode);
                    }
                );
            }
        );
    }

    void Shader::CreateFromSource(const std::string& vertexShaderCode, const std::string& fragmentShaderCode)
    {
        SD_ASSERT(!initialized, "Shader already initialized");

        const char* vertexCode = vertexShaderCode.c_str();
        const char* fragmentCode = fragmentShaderCode.c_str();

        unsigned int vertexShader, fragmentShader;
        int  success;
        char infoLog[512];

        vertexShader = GL_CHECK(glCreateShader(GL_VERTEX_SHADER));
        GL_CHECK(glShaderSource(vertexShader, 1, &vertexCode, NULL));
        GL_CHECK(glCompileShader(vertexShader));

        GL_CHECK(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success));
        if (!success)
        {
            GL_CHECK(glGetShaderInfoLog(vertexShader, 512, NULL, infoLog));
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            return;
        }

        fragmentShader = GL_CHECK(glCreateShader(GL_FRAGMENT_SHADER));
        GL_CHECK(glShaderSource(fragmentShader, 1, &fragmentCode, NULL));
        GL_CHECK(glCompileShader(fragmentShader));

        GL_CHECK(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success));

        if (!success)
        {
            GL_CHECK(glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog));
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            return;
        }

        renderId = GL_CHECK(glCreateProgram());
        GL_CHECK(glAttachShader(renderId, vertexShader));
        GL_CHECK(glAttachShader(renderId, fragmentShader));
        GL_CHECK(glLinkProgram(renderId));

        GL_CHECK(glGetProgramiv(renderId, GL_LINK_STATUS, &success));

        if (!success) 
        {
            GL_CHECK(glGetProgramInfoLog(renderId, 512, NULL, infoLog));
            std::cout << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
        }

        GL_CHECK(glDeleteShader(vertexShader));
        GL_CHECK(glDeleteShader(fragmentShader));

        initialized = true;

        Use();
        SetInt("iblData.irradianceMap", 5);
        SetInt("iblData.prefilterMap", 6);
        SetInt("iblData.BRDFLookupMap", 7);

        int shadowSamplers[] = { 8, 9, 10, 11 };
        SetInts("shadowMappingData.shadowMaps", shadowSamplers, 4);

    }


    void Shader::Use()
    {
        SD_ASSERT(initialized, "Shader not initialized");

        GL_CHECK(glUseProgram(renderId));
    }

    void Shader::SetBool(const std::string& name, bool value) const
    {
        SD_ASSERT(initialized, "Shader not initialized");

        GL_CHECK(glUniform1i(glGetUniformLocation(renderId, name.c_str()), (int)value));
    }

    void Shader::SetInt(const std::string& name, int value) const
    {
        SD_ASSERT(initialized, "Shader not initialized");

        GL_CHECK(glUniform1i(glGetUniformLocation(renderId, name.c_str()), value));
    }

    void Shader::SetInts(const std::string& name, int* values, int count) const
    {
        SD_ASSERT(initialized, "Shader not initialized");

        GL_CHECK(glUniform1iv(glGetUniformLocation(renderId, name.c_str()), count, values));
    }

    void Shader::SetFloat(const std::string& name, float value) const
    {
        SD_ASSERT(initialized, "Shader not initialized");

        GL_CHECK(glUniform1f(glGetUniformLocation(renderId, name.c_str()), value));
    }

    void Shader::SetDouble(const std::string& name, double value) const
    {
        SD_ASSERT(initialized, "Shader not initialized");

        GL_CHECK(glUniform1d(glGetUniformLocation(renderId, name.c_str()), value));
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
    {
        SD_ASSERT(initialized, "Shader not initialized");

        GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(renderId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)));
    }

    void Shader::SetMat3(const std::string& name, const glm::mat3& value) const
    {
        SD_ASSERT(initialized, "Shader not initialized");

        GL_CHECK(glUniformMatrix3fv(glGetUniformLocation(renderId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)));
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
    {
        SD_ASSERT(initialized, "Shader not initialized");

        GL_CHECK(glUniform3fv(glGetUniformLocation(renderId, name.c_str()), 1, glm::value_ptr(value)));
    }
}