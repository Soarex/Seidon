#include "Shader.h"

#include "Core/Application.h"
#include "Core/WorkManager.h"

#include "../Debug/Debug.h"

namespace Seidon
{
    Shader* Shader::temporaryShader = nullptr;

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