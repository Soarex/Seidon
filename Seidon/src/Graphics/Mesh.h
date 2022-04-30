#pragma once
#include <Glad/glad.h>

#include <vector>
#include <string>

#include "Vertex.h"
#include "Material.h"

namespace Seidon
{
    class SubMesh
    {
    public:
        std::string name;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;    
    public:

        void Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& name = "");
    private:
        friend struct Mesh;
    };

    struct Mesh
    {
        UUID id;
        std::string filepath;
        std::string name;
        std::vector<SubMesh*> subMeshes;

        Mesh(UUID id = UUID());
        Mesh(const std::string& name) : name(name) {};
        ~Mesh();

        void Save(const std::string& path);
        void SaveAsync(const std::string& path);
        void Load(const std::string& path);
        void LoadAsync(const std::string& path);
    };
}