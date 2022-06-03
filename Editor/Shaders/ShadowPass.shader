~VERTEX SHADER
#version 460 core

layout(location = 0) in vec3 vertexPosition;

layout(location = 6) in int objectId;

layout(std430, binding = 0) buffer transforms
{
    mat4 modelMatrices[];
};

uniform mat4 lightSpaceMatrix;


void main()
{
    mat4 modelMatrix = modelMatrices[objectId];

    gl_Position = lightSpaceMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}


~FRAGMENT SHADER
#version 460 core
void main()
{
    // gl_FragDepth = gl_FragCoord.z;
}