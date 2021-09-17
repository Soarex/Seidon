~VERTEX SHADER
#version 330 core
layout(location = 0) in vec3 vertexPosition;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 localPosition;

void main()
{
    localPosition = vertexPosition;

    vec4 clipPosition = projectionMatrix * viewMatrix * vec4(localPosition, 1.0);

    gl_Position = clipPosition.xyww;
}

~FRAGMENT SHADER
#version 330 core
out vec4 fragmentColor;

in vec3 localPosition;

uniform samplerCube environmentMap;

void main()
{
    vec3 environmentColor = texture(environmentMap, localPosition).rgb;

    fragmentColor = vec4(environmentColor, 1.0);
}