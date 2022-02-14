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
        glm::ivec4 boneIds = { -1, -1, -1, -1 };
        glm::vec4 weights = { 0, 0, 0, 0 };
    };
}