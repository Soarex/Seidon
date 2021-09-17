~VERTEX SHADER
#version 330 core
layout(location = 0) in vec3 vertexPosition;

out vec3 localPosition;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    localPosition = vertexPosition;
    gl_Position = projectionMatrix * viewMatrix * vec4(localPosition, 1.0);
}

~FRAGMENT SHADER
#version 330 core
in vec3 localPosition;

out vec4 fragmentColor;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(localPosition));
    vec3 color = texture(equirectangularMap, uv).rgb;

    fragmentColor = vec4(color, 1.0);
}