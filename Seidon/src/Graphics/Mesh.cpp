#include "Mesh.h"
#include <iostream>

namespace Seidon
{
    SubMesh::SubMesh()
    {

    }

    SubMesh::~SubMesh()
    {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void SubMesh::Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& name)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->name = name;

        SetupMesh();
    }

    void SubMesh::SetupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
            &indices[0], GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        // vertex tangent
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        // vertex texture coords
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

        glBindVertexArray(0);
    }

    Mesh::~Mesh()
    {
        for (SubMesh* m : subMeshes)
            delete m;
    }

    void Mesh::Save(const std::string& path)
    {
        std::ofstream out(path, std::ios::out | std::ios::binary);

        out.write((char*)&id, sizeof(id));

        size_t size = name.length() + 1;
        out.write((char*)&size, sizeof(size_t));

        out.write(name.c_str(), size * sizeof(char));

        size = subMeshes.size();
        out.write((char*)&size, sizeof(size_t));

        for (SubMesh* submesh : subMeshes)
        {
            size = submesh->name.length() + 1;
            out.write((char*)&size, sizeof(size_t));
            out.write(submesh->name.c_str(), size * sizeof(char));

            size = submesh->vertices.size();
            out.write((char*)&size, sizeof(size_t));

            for (Vertex& v : submesh->vertices)
                out.write((char*)&v, sizeof(Vertex));

            size = submesh->indices.size();
            out.write((char*)&size, sizeof(size_t));

            for (unsigned int i : submesh->indices)
                out.write((char*)&i, sizeof(unsigned int));
        }
    }

    void Mesh::Load(const std::string& path)
    {
        std::ifstream in(path, std::ios::in | std::ios::binary);

        char buffer[2048];
        in.read((char*)&id, sizeof(id));

        size_t size = 0;
        in.read((char*)&size, sizeof(size_t));

        in.read(buffer, size * sizeof(char));
        name = buffer;

        in.read((char*)&size, sizeof(size_t));
        subMeshes.resize(size);
        for (int i = 0; i < size; i++)
        {
            SubMesh* submesh = new SubMesh();

            size_t size2 = 0;

            in.read((char*)&size2, sizeof(size_t));
            in.read(buffer, size2 * sizeof(char));
            submesh->name = buffer;

            in.read((char*)&size2, sizeof(size_t));

            submesh->vertices.resize(size2);

            for (int j = 0; j < size2; j++)
                in.read((char*)&submesh->vertices[j], sizeof(Vertex));


            in.read((char*)&size2, sizeof(size_t));

            submesh->indices.resize(size2);
            for (int j = 0; j < size2; j++)
                in.read((char*)&submesh->indices[j], sizeof(unsigned int));

            submesh->SetupMesh();
            subMeshes[i] = submesh;
        }
    }
}