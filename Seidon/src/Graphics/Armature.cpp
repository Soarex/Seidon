#include "Armature.h"

#include <iostream>
#include <fstream>

namespace Seidon
{
    Armature::Armature(UUID id)
        : id(id)
    {

    }

	void Armature::Save(const std::string& path)
	{
        std::ofstream out(path, std::ios::out | std::ios::binary);

        out.write((char*)&id, sizeof(id));

        size_t size = name.length() + 1;
        out.write((char*)&size, sizeof(size_t));

        out.write(name.c_str(), size * sizeof(char));

        size = bones.size();
        out.write((char*)&size, sizeof(size_t));

        for (BoneData& bone : bones)
        {
            size = bone.name.length() + 1;
            out.write((char*)&size, sizeof(size_t));
            out.write(bone.name.c_str(), size * sizeof(char));

            out.write((char*)&bone.id, sizeof(bone.id));
            out.write((char*)&bone.parentId, sizeof(bone.parentId));
            out.write((char*)&bone.inverseBindPoseMatrix, sizeof(bone.inverseBindPoseMatrix));
        }
	}

    void Armature::Load(const std::string& path)
    {
        std::ifstream in(path, std::ios::in | std::ios::binary);

        if (!in)
        {
            std::cerr << "Error opening armature file: " << path << std::endl;
            return;
        }

        char buffer[2048];
        in.read((char*)&id, sizeof(id));

        size_t size = 0;
        in.read((char*)&size, sizeof(size_t));

        in.read(buffer, size * sizeof(char));
        name = buffer;

        in.read((char*)&size, sizeof(size_t));
        bones.resize(size);
        for (int i = 0; i < size; i++)
        {
            BoneData bone;

            size_t size2 = 0;

            in.read((char*)&size2, sizeof(size_t));
            in.read(buffer, size2 * sizeof(char));
            bone.name = buffer;

            in.read((char*)&bone.id, sizeof(bone.id));
            in.read((char*)&bone.parentId, sizeof(bone.parentId));
            in.read((char*)&bone.inverseBindPoseMatrix, sizeof(bone.inverseBindPoseMatrix));

            bones[i] = bone;
        }
    }
}