#pragma once
#include "../Core/UUID.h"


#include <glm/glm.hpp>
#include <string>

namespace Seidon
{
    struct MetaType;

    class Shader
    {
    private:
        bool initialized = false;

        UUID id;
        std::string path;
        unsigned int renderId;

        MetaType* bufferLayout;

        static Shader* temporaryShader;
    public:
        Shader(UUID id = UUID());
        Shader(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
        ~Shader();

        inline UUID GetId() { return id; }
        inline const std::string& GetPath() { return path; }

        void CreateFromSource(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
        void Destroy();

        void LoadFromFile(const std::string& path);
        void LoadFromFileAsync(const std::string& path);

        void Use();

        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetInts(const std::string& name, int* values, int count) const;
        void SetFloat(const std::string& name, float value) const;
        void SetDouble(const std::string& name, double value) const;
        void SetMat3(const std::string& name, const glm::mat3& value) const;
        void SetMat4(const std::string& name, const glm::mat4& value) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;

        MetaType* GetBufferLayout() { return bufferLayout; }

    private:
        void ReadLayout(std::ifstream& stream);

        friend class ResourceManager;
    };
}