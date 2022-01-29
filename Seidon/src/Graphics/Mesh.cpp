#include "Mesh.h"
#include "../Core/Application.h"
#include "../Debug/Debug.h"

#include <iostream>

namespace Seidon
{
    SubMesh::SubMesh()
    {

    }

    SubMesh::~SubMesh()
    {
        Destroy();
    }

    void SubMesh::Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& name)
    {
        SD_ASSERT(!initialized, "Submesh already initialized");

        this->vertices = vertices;
        this->indices = indices;
        this->name = name;

        SetupMesh();

        initialized = true;
    }

    void SubMesh::Destroy()
    {
        SD_ASSERT(initialized, "Submesh not initialized");

        GL_CHECK(glDeleteBuffers(1, &VBO));
        GL_CHECK(glDeleteBuffers(1, &EBO));
        GL_CHECK(glDeleteVertexArrays(1, &VAO));

        initialized = false;
    }

    void SubMesh::SetupMesh()
    {
        GL_CHECK(glGenVertexArrays(1, &VAO));
        GL_CHECK(glGenBuffers(1, &VBO));
        GL_CHECK(glGenBuffers(1, &EBO));

        GL_CHECK(glBindVertexArray(VAO));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, VBO));

        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW));

        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
        GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
            &indices[0], GL_STATIC_DRAW));

        // vertex positions
        GL_CHECK(glEnableVertexAttribArray(0));
        GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));

        // vertex normals
        GL_CHECK(glEnableVertexAttribArray(1));
        GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)));

        // vertex tangent
        GL_CHECK(glEnableVertexAttribArray(2));
        GL_CHECK(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent)));

        // vertex texture coords
        GL_CHECK(glEnableVertexAttribArray(3));
        GL_CHECK(glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords)));

        // vertex bone ids
        GL_CHECK(glEnableVertexAttribArray(3));
        GL_CHECK(glVertexAttribPointer(4, 1, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneIds)));

        // vertex weights
        GL_CHECK(glEnableVertexAttribArray(3));
        GL_CHECK(glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights)));

        GL_CHECK(glBindVertexArray(0));

        initialized = true;
    }

    unsigned int SubMesh::GetVAO() 
    {
        SD_ASSERT(initialized, "Submesh not initialized");
        return VAO; 
    };

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

    void Mesh::SaveAsync(const std::string& path)
    {
        Application::Get()->GetWorkManager()->Execute([&]()
            {
                Save(path);
            }
        );
    }

    void Mesh::Load(const std::string& path)
    {
        std::ifstream in(path, std::ios::in | std::ios::binary);

        if (!in)
        {
            std::cerr << "Error opening mesh file: " << path << std::endl;
            return;
        }

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

    void Mesh::LoadAsync(const std::string& path)
    {
        Application::Get()->GetWorkManager()->Execute([&]()
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

                    Application::Get()->GetWorkManager()->ExecuteOnMainThread([=]()
                        {
                            submesh->SetupMesh();
                        }
                    );

                    subMeshes[i] = submesh;
                }
            }
        );
    }
}