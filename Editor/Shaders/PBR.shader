~VERTEX SHADER
#version 330 core
#define MAX_CASCADE_COUNT 8
#define MAX_BONE_COUNT 100
#define MAX_BONE_INFLUENCE 4

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexTangent;
layout(location = 3) in vec2 vertexUV;
layout(location = 4) in ivec4 boneIds;
layout(location = 5) in vec4 boneWeights;

out VS_OUT
{
    vec3 worldSpaceFragmentPosition;
    vec3 normal;
    vec2 UV;
    vec4 lightSpaceFragmentPositions[MAX_CASCADE_COUNT];
    float viewSpaceZ;
    mat3 TBN;
} vs_out;

out vec4 acolor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrices[MAX_CASCADE_COUNT];

uniform mat4 boneMatrices[MAX_BONE_COUNT];

void main()
{
    mat4 boneTransformMatrix = mat4(1.0);

    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        if (boneIds[i] > -1)
            boneTransformMatrix += boneMatrices[boneIds[i]] * boneWeights[i];

    //acolor = vec4(boneIds[0] / 20.0, boneIds[1] / 20.0, boneIds[2] / 20.0, 1);
    //acolor = vec4(boneWeights[0], boneWeights[1], boneWeights[2], 1);

    vec3 T = normalize(vec3(modelMatrix * vec4(vertexTangent, 0.0)));
    vec3 N = normalize(vec3(modelMatrix * vec4(vertexNormal, 0.0)));

    T = normalize(T - dot(T, N) * N);

    vec3 B = cross(N, T);

    vs_out.worldSpaceFragmentPosition = vec3(modelMatrix * boneTransformMatrix * vec4(vertexPosition, 1.0));
    vs_out.normal = normalMatrix * vertexNormal;
    vs_out.UV = vertexUV;

    for(int i = 0; i < MAX_CASCADE_COUNT; i++)
        vs_out.lightSpaceFragmentPositions[i] = lightSpaceMatrices[i] * vec4(vs_out.worldSpaceFragmentPosition, 1.0);
    
    vs_out.viewSpaceZ = (viewMatrix * vec4(vs_out.worldSpaceFragmentPosition, 1)).z;

    vs_out.TBN = mat3(T, B, N);

    gl_Position = projectionMatrix * viewMatrix * vec4(vs_out.worldSpaceFragmentPosition, 1.0);
}


~FRAGMENT SHADER
#version 330 core
#define MAX_CASCADE_COUNT 8

layout(location = 0) out vec4 fragmentColor;

in VS_OUT
{
    vec3 worldSpaceFragmentPosition;
    vec3 normal;
    vec2 UV;
    vec4 lightSpaceFragmentPositions[MAX_CASCADE_COUNT];
    float viewSpaceZ;
    mat3 TBN;
} fs_in;

in vec4 acolor;

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
};

uniform vec3 cameraPosition;
uniform mat4 viewMatrix;

uniform DirectionalLight directionalLight;

uniform vec3 tint;
uniform sampler2D albedoMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D aoMap;

uniform sampler2D shadowMaps[MAX_CASCADE_COUNT];
uniform float cascadeFarPlaneDistances[MAX_CASCADE_COUNT + 1];
uniform int cascadeCount;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   BRDFLookupMap;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySchlickGGX(float NdotV, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float shadowCalculation(float NdotL);

void main()
{
    vec3 normal = texture(normalMap, fs_in.UV).rgb;
    normal = normal * 2.0 - 1.0;
    vec3 N = normalize(fs_in.TBN * normal);

    vec3 albedo = texture(albedoMap, fs_in.UV).rgb * tint;
    float metallic = texture(metallicMap, fs_in.UV).r;
    float roughness = texture(roughnessMap, fs_in.UV).r;
    float ao = texture(aoMap, fs_in.UV).r;

    //vec3 N = normalize(fs_in.normal);
    vec3 V = normalize(cameraPosition - fs_in.worldSpaceFragmentPosition);
    vec3 R = reflect(-V, N);

    //Direct Light
    vec3 L = normalize(directionalLight.direction);
    vec3 H = normalize(V + L);

    vec3 radiance = directionalLight.color;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    float NDF = distributionGGX(N, H, roughness);
    float G = geometrySmith(N, V, L, roughness);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = vec3(0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    //Ambient Light (IBL)
    F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    kS = F;
    kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 ambientDiffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF = texture(BRDFLookupMap, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 ambientSpecular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 ambient = (kD * ambientDiffuse + ambientSpecular) * ao;

    float shadow = shadowCalculation(NdotL);
    vec3 color = ambient + (1 - shadow) * Lo;
    
    fragmentColor = vec4(color, 1.0f);
    //fragmentColor = acolor;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float shadowCalculation(float NdotL)
{
    int cascadeIndex = cascadeCount - 1;
    for (int i = 0; i < cascadeCount - 1; ++i)
        if (abs(fs_in.viewSpaceZ) < cascadeFarPlaneDistances[i])
        {
            cascadeIndex = i;
            break;
        }

    vec3 projectedCoordinates = fs_in.lightSpaceFragmentPositions[cascadeIndex].xyz / fs_in.lightSpaceFragmentPositions[cascadeIndex].w;
    projectedCoordinates = projectedCoordinates * 0.5 + 0.5;

    float closestDepth = texture(shadowMaps[cascadeIndex], projectedCoordinates.xy).r;
    float currentDepth = projectedCoordinates.z;

    if (currentDepth > 1.0)
        return 0.0;

    float bias = max(0.05 * (1.0 - NdotL), 0.005);
    bias *= 1 / (cascadeFarPlaneDistances[cascadeIndex] * 0.5f);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMaps[cascadeIndex], 0);
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMaps[cascadeIndex], projectedCoordinates.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }

    shadow /= 9.0;

    return shadow;
}