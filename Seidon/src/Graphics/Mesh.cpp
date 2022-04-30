#include "Mesh.h"
#include "../Core/Application.h"
#include "../Debug/Debug.h"

#include <iostream>
#include <fstream>

namespace Seidon
{
    void SubMesh::Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& name)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->name = name;
    }

    Mesh::Mesh(UUID id)
        : id(id)
    {

    }

    Mesh::~Mesh()
    {
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

                    subMeshes[i] = submesh;
                }
            }
        );
    }
}