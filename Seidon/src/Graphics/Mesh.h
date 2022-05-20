#pragma once
#include "Vertex.h"
#include "Armature.h"
#include "../Core/UUID.h"

#include <vector>
#include <string>

#include <iostream>
#include <fstream>

namespace Seidon
{
    template <typename T>
    struct BaseSubmesh
    {
        std::string name;
        std::vector<T> vertices;
        std::vector<unsigned int> indices;

        T vertexType;

        BaseSubmesh() = default;
        virtual ~BaseSubmesh() = default;

        BaseSubmesh(const std::vector<T>& vertices, const std::vector<unsigned int>& indices, const std::string& name = "")
            : name(name), vertices(vertices), indices(indices) {}
    };

    template <typename T>
    struct BaseMesh
    {
        UUID id;
        std::string filepath;
        std::string name;
        std::vector<T*> subMeshes;

        T submeshType;

        BaseMesh(UUID id = UUID()) : id(id) {}
        BaseMesh(const std::string& name) : name(name) {};
        virtual ~BaseMesh() = default;

        void Load(std::ifstream& in)
        {
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
                T* submesh = new T();

                size_t size2 = 0;

                in.read((char*)&size2, sizeof(size_t));
                in.read(buffer, size2 * sizeof(char));
                submesh->name = buffer;

                in.read((char*)&size2, sizeof(size_t));

                submesh->vertices.resize(size2);

                for (int j = 0; j < size2; j++)
                    in.read((char*)&submesh->vertices[j], sizeof(decltype(submesh->vertexType)));


                in.read((char*)&size2, sizeof(size_t));

                submesh->indices.resize(size2);
                for (int j = 0; j < size2; j++)
                    in.read((char*)&submesh->indices[j], sizeof(unsigned int));

                subMeshes[i] = submesh;
            }
        }

        //void SaveAsync(const std::string& path);
        void Save(std::ofstream& out)
        {
            out.write((char*)&id, sizeof(id));

            size_t size = name.length() + 1;
            out.write((char*)&size, sizeof(size_t));

            out.write(name.c_str(), size * sizeof(char));

            size = subMeshes.size();
            out.write((char*)&size, sizeof(size_t));

            for (T* submesh : subMeshes)
            {
                size = submesh->name.length() + 1;
                out.write((char*)&size, sizeof(size_t));
                out.write(submesh->name.c_str(), size * sizeof(char));

                size = submesh->vertices.size();
                out.write((char*)&size, sizeof(size_t));

                for (auto& v : submesh->vertices)
                    out.write((char*)&v, sizeof(decltype(submesh->vertexType)));

                size = submesh->indices.size();
                out.write((char*)&size, sizeof(size_t));

                for (unsigned int i : submesh->indices)
                    out.write((char*)&i, sizeof(unsigned int));
            }
        }
    };

    using Submesh = BaseSubmesh<Vertex>;
    using Mesh = BaseMesh<Submesh>;

    using SkinnedSubmesh = BaseSubmesh<SkinnedVertex>;

    struct SkinnedMesh : public BaseMesh<SkinnedSubmesh>
    {
        Armature armature;

        SkinnedMesh(UUID id = UUID()) { this->id = id; }
        SkinnedMesh(const std::string& name) { this->name = name; };

        void Save(std::ofstream& out)
        {
            BaseMesh::Save(out);
            armature.Save(out);
        }

        void Load(std::ifstream& in)
        {
            BaseMesh::Load(in);
            armature.Load(in);
        }
    };
}