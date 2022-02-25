#include "Animation.h"

#include <iostream>
#include <fstream>

namespace Seidon
{
    void Animation::Save(const std::string& path)
    {
        std::ofstream out(path, std::ios::out | std::ios::binary);

        out.write((char*)&id, sizeof(id));

        size_t size = name.length() + 1;
        out.write((char*)&size, sizeof(size_t));

        out.write(name.c_str(), size * sizeof(char));

        out.write((char*)&duration, sizeof(duration));
        out.write((char*)&ticksPerSecond, sizeof(ticksPerSecond));

        size = channels.size();
        out.write((char*)&size, sizeof(size_t));

        for (AnimationChannel& channel : channels)
        {
            size = channel.boneName.length() + 1;
            out.write((char*)&size, sizeof(size_t));
            out.write(channel.boneName.c_str(), size * sizeof(char));

            out.write((char*)&channel.boneId, sizeof(channel.boneId));

            size = channel.positionKeys.size();
            out.write((char*)&size, sizeof(size_t));
            for (PositionKey& key : channel.positionKeys)
            {
                out.write((char*)&key.time, sizeof(key.time));
                out.write((char*)&key.value, sizeof(key.value));
            }

            size = channel.rotationKeys.size();
            out.write((char*)&size, sizeof(size_t));
            for (RotationKey& key : channel.rotationKeys)
            {
                out.write((char*)&key.time, sizeof(key.time));
                out.write((char*)&key.value, sizeof(key.value));
            }

            size = channel.scalingKeys.size();
            out.write((char*)&size, sizeof(size_t));
            for (ScalingKey& key : channel.scalingKeys)
            {
                out.write((char*)&key.time, sizeof(key.time));
                out.write((char*)&key.value, sizeof(key.value));
            }
        }
    }

    void Animation::Load(const std::string& path)
    {
        std::ifstream in(path, std::ios::in | std::ios::binary);

        if (!in)
        {
            std::cerr << "Error opening animation file: " << path << std::endl;
            return;
        }

        char buffer[2048];
        in.read((char*)&id, sizeof(id));

        size_t size = 0;
        in.read((char*)&size, sizeof(size_t));

        in.read(buffer, size * sizeof(char));
        name = buffer;

        in.read((char*)&duration, sizeof(duration));
        in.read((char*)&ticksPerSecond, sizeof(ticksPerSecond));

        in.read((char*)&size, sizeof(size_t));
        channels.resize(size);
        for (int i = 0; i < size; i++)
        {
            AnimationChannel channel;

            size_t size2 = 0;

            in.read((char*)&size2, sizeof(size_t));
            in.read(buffer, size2 * sizeof(char));
            channel.boneName = buffer;

            in.read((char*)&channel.boneId, sizeof(channel.boneId));

            in.read((char*)&size2, sizeof(size_t));
            channel.positionKeys.resize(size2);
            for (int j = 0; j < size2; j++)
            {
                PositionKey key;
                in.read((char*)&key.time, sizeof(key.time));
                in.read((char*)&key.value, sizeof(key.value));

                channel.positionKeys[j] = key;
            }

            in.read((char*)&size2, sizeof(size_t));
            channel.rotationKeys.resize(size2);
            for (int j = 0; j < size2; j++)
            {
                RotationKey key;
                in.read((char*)&key.time, sizeof(key.time));
                in.read((char*)&key.value, sizeof(key.value));

                channel.rotationKeys[j] = key;
            }

            in.read((char*)&size2, sizeof(size_t));
            channel.scalingKeys.resize(size2);
            for (int j = 0; j < size2; j++)
            {
                ScalingKey key;
                in.read((char*)&key.time, sizeof(key.time));
                in.read((char*)&key.value, sizeof(key.value));

                channel.scalingKeys[j] = key;
            }

            channels[i] = channel;
        }
    }
}