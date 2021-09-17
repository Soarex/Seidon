~VERTEX SHADER
#version 330 core
layout(location = 0) in vec3 vertexPosition;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = lightSpaceMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}


~FRAGMENT SHADER
#version 330 core

void main()
{
    // gl_FragDepth = gl_FragCoord.z;
}