#pragma once
#include "../Core/UUID.h"
#include "../Core/Asset.h"

#include <glm/glm.hpp>
#include <string>

namespace Seidon
{
    struct MetaType;

    class Shader : public Asset
    {
    public:
        Shader(UUID id = UUID());
        Shader(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
        ~Shader();

        inline UUID GetId() { return id; }

        void CreateFromSource(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
        void Destroy();

        using Asset::Save;
        using Asset::Load;

        void Save(std::ofstream& out) override;
        void Load(std::ifstream& in) override;

        void Load(const std::string& path) override;
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
        bool initialized = false;

        unsigned int renderId;

        MetaType* bufferLayout;

        static Shader* temporaryShader;
    private:
        void ReadLayout(std::ifstream& stream);

        friend class ResourceManager;
    };
}