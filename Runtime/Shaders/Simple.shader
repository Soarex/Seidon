~VERTEX SHADER
#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 3) in vec2 VertexUV;

out vec2 UV;

void main()
{
    UV = VertexUV;
    gl_Position = vec4(vertexPosition, 1.0);
}


~FRAGMENT SHADER
#version 330 core

in vec2 UV;

out vec4 FragColor;

uniform float exposure;
uniform sampler2D hdrMap;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrMap, UV).rgb;

    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);
}