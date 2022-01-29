#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace Seidon
{
    struct Vertex {
        static constexpr int MAX_BONES_PER_VERTEX = 4;

        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 texCoords;
        std::vector<int> boneIds = { -1, -1, -1, -1 };
        std::vector<int> weights = { 0, 0, 0, 0 };
    };
}