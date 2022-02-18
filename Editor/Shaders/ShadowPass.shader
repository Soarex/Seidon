~VERTEX SHADER
#version 330 core
#define MAX_BONE_COUNT 100
#define MAX_BONE_INFLUENCE 4

layout(location = 0) in vec3 vertexPosition;
layout(location = 4) in ivec4 boneIds;
layout(location = 5) in vec4 boneWeights;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;

uniform mat4 boneMatrices[MAX_BONE_COUNT];

void main()
{
    mat4 boneTransformMatrix = mat4(0);

    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        boneTransformMatrix += boneMatrices[boneIds[i]] * boneWeights[i];

    if (boneTransformMatrix == mat4(0))
        boneTransformMatrix = mat4(1);

    gl_Position = lightSpaceMatrix * modelMatrix * boneTransformMatrix * vec4(vertexPosition, 1.0);
}


~FRAGMENT SHADER
#version 330 core
void main()
{
    // gl_FragDepth = gl_FragCoord.z;
}