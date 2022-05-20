#include "Mesh.h"
/*
#include "../Core/Application.h"
#include "../Debug/Debug.h"

#include <iostream>
#include <fstream>
namespace Seidon
{
    template <typename T>
    void BaseMesh<T>::Save(const std::string& path)
    {
        std::ofstream out(path, std::ios::out | std::ios::binary);

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

    template <typename T>
    void BaseMesh<T>::SaveAsync(const std::string& path)
    {
        Application::Get()->GetWorkManager()->Execute([&]()
            {
                Save(path);
            }
        );
    }

    template <typename T>
    void BaseMesh<T>::Load(const std::string& path)
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
}
*/