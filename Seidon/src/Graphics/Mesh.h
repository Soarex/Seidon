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

    private:
        unsigned int VBO, EBO, VAO;

    public:
        SubMesh();
        ~SubMesh();

        void Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& name = "");

        unsigned int GetVAO() { return VAO; };

    private:
        void SetupMesh();

        friend struct Mesh;
    };

    struct Mesh
    {
        UUID id;
        std::string filepath;
        std::string name;
        std::vector<SubMesh*> subMeshes;

        Mesh() = default;
        Mesh(const std::string& name) : name(name) {};
        ~Mesh();

        void Load(const std::string& path);
        void Save(const std::string& path);
    };
}