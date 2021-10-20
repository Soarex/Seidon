#include "Shader.h"

#include "Core/Application.h"
#include "Core/WorkManager.h"

namespace Seidon
{
    Shader* Shader::temporaryShader = nullptr;

    Shader::Shader(const std::string& vertexShaderCode, const std::string& fragmentShaderCode)
    {
        CreateFromSource(vertexShaderCode, fragmentShaderCode);
    }

    Shader::~Shader()
    {
        glDeleteProgram(ID);
    }

    void Shader::LoadFromFile(const std::string& path)
    {
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
    }

    void Shader::LoadFromFileAsync(const std::string& path)
    {
        this->path = path;

        if (!temporaryShader)
        {
            temporaryShader = new Shader();
            std::string vertexSource = "#version 330 core\n void main() { gl_Position = vec4(0); } ";
            std::string fragmentSource = "#version 330 core\n void main() { } ";

            temporaryShader->path = "Temp";
            temporaryShader->CreateFromSource(vertexSource, fragmentSource);
        }

        ID = temporaryShader->ID;
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
        const char* vertexCode = vertexShaderCode.c_str();
        const char* fragmentCode = fragmentShaderCode.c_str();

        unsigned int vertexShader, fragmentShader;
        int  success;
        char infoLog[512];

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexCode, NULL);
        glCompileShader(vertexShader);

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }


    void Shader::Use()
    {
        glUseProgram(ID);
    }

    void Shader::SetBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void Shader::SetInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::SetFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetMat3(const std::string& name, const glm::mat3& value) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }
}