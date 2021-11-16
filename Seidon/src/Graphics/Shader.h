#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Seidon
{
    class Shader
    {
    private:
        std::string path;
        unsigned int ID;

        static Shader* temporaryShader;
    public:
        Shader() = default;
        Shader(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
        ~Shader();
        void CreateFromSource(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);

        void LoadFromFile(const std::string& path);
        void LoadFromFileAsync(const std::string& path);

        void Use();

        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetInts(const std::string& name, int* values, int count) const;
        void SetFloat(const std::string& name, float value) const;
        void SetMat3(const std::string& name, const glm::mat3& value) const;
        void SetMat4(const std::string& name, const glm::mat4& value) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;

    };
}