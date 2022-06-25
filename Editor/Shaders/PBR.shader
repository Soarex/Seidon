~VERTEX SHADER

~BEGIN LAYOUT
Tint : VECTOR3_COLOR
Albedo : TEXTURE
Normal : TEXTURE
Roughness : TEXTURE
Metallic : TEXTURE
Ambient Occlusion : TEXTURE
~END LAYOUT

#version 450 core
#extension GL_ARB_bindless_texture : require

#define MAX_CASCADE_COUNT 4

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexTangent;
layout(location = 3) in vec2 vertexUV;

layout(location = 6) in uint objectId;

out VS_OUT
{
    vec3 worldSpaceFragmentPosition;
    vec3 normal;
    vec2 UV;
    vec4 lightSpaceFragmentPositions[MAX_CASCADE_COUNT];
    float viewSpaceZ;
    mat3 TBN;

    flat uint objectId;
} vs_out;

layout(std430, binding = 0) buffer transforms
{
    mat4 modelMatrices[];
};

struct CameraData
{
    vec3 position;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

uniform CameraData camera;

uniform mat4 lightSpaceMatrices[MAX_CASCADE_COUNT];

void main()
{
    mat4 modelMatrix = modelMatrices[objectId];
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    vec3 T = normalize(vec3(modelMatrix * vec4(vertexTangent, 0.0)));
    vec3 N = normalize(vec3(modelMatrix * vec4(vertexNormal, 0.0)));

    T = normalize(T - dot(T, N) * N);

    vec3 B = cross(N, T);

    vs_out.worldSpaceFragmentPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
    vs_out.normal = normalMatrix * vertexNormal;
    vs_out.UV = vertexUV;

    vec3 bias = vs_out.normal * 0.3;
    for(int i = 0; i < MAX_CASCADE_COUNT; i++)
        vs_out.lightSpaceFragmentPositions[i] = lightSpaceMatrices[i] * vec4(vs_out.worldSpaceFragmentPosition + bias, 1.0);
    
    vs_out.viewSpaceZ = (camera.viewMatrix * vec4(vs_out.worldSpaceFragmentPosition, 1)).z;

    vs_out.TBN = mat3(T, B, N);

    vs_out.objectId = objectId;

    gl_Position = camera.projectionMatrix * camera.viewMatrix * vec4(vs_out.worldSpaceFragmentPosition, 1.0);
}


~FRAGMENT SHADER
#version 450 core
#extension GL_ARB_bindless_texture : require

#define MAX_CASCADE_COUNT 4

layout(location = 0) out vec4 fragmentColor;
layout(location = 1) out int entityOutput;

in VS_OUT
{
    vec3 worldSpaceFragmentPosition;
    vec3 normal;
    vec2 UV;
    vec4 lightSpaceFragmentPositions[MAX_CASCADE_COUNT];
    float viewSpaceZ;
    mat3 TBN;

    flat uint objectId;
} fs_in;

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
};

struct Material
{
    vec3 tint;
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D roughnessMap;
    sampler2D metallicMap;
    sampler2D aoMap;
};

struct CameraData
{
    vec3 position;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

struct IBLData
{
    samplerCube irradianceMap;
    samplerCube prefilterMap; 
    sampler2D   BRDFLookupMap;
};

struct ShadowMappingData
{
    sampler2D shadowMaps[MAX_CASCADE_COUNT];
    float cascadeFarPlaneDistances[MAX_CASCADE_COUNT + 1];
    int cascadeCount;
};

uniform DirectionalLight directionalLight;
uniform CameraData camera;
uniform IBLData iblData;
uniform ShadowMappingData shadowMappingData;

layout(std430, binding = 1) buffer textureBuffer
{
    Material materials[];
};

layout(std430, binding = 2) buffer ids
{
    int entityIds[];
};

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySchlickGGX(float NdotV, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float shadowCalculation();

void main()
{
    vec3 tint = materials[fs_in.objectId].tint;
    vec3 albedo = texture(materials[fs_in.objectId].albedoMap, fs_in.UV).rgb * tint;
    float metallic = texture(materials[fs_in.objectId].metallicMap, fs_in.UV).r;
    float roughness = texture(materials[fs_in.objectId].roughnessMap, fs_in.UV).r;
    float ao = texture(materials[fs_in.objectId].aoMap, fs_in.UV).r;
    vec3 normal = texture(materials[fs_in.objectId].normalMap, fs_in.UV).rgb;
    normal = normal * 2.0 - 1.0;
    vec3 N = normalize(fs_in.TBN * normal);

    //vec3 N = normalize(fs_in.normal);
    vec3 V = normalize(camera.position - fs_in.worldSpaceFragmentPosition);
    vec3 R = reflect(-V, N);

    // Direct Light
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

    // Ambient Light (IBL)
    F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    kS = F;
    kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(iblData.irradianceMap, N).rgb;
    vec3 ambientDiffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 5.0;
    vec3 prefilteredColor = textureLod(iblData.prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF = texture(iblData.BRDFLookupMap, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 ambientSpecular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 ambient = (kD * ambientDiffuse + ambientSpecular) * ao;

    float shadow = shadowCalculation();

    vec3 color = ambient + (1 - shadow) * Lo;
    fragmentColor = vec4(color, 1.0f);
    entityOutput = entityIds[fs_in.objectId];
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

float shadowCalculation()
{
    int cascadeIndex = shadowMappingData.cascadeCount - 1;
    for (int i = 0; i < shadowMappingData.cascadeCount - 1; ++i)
        if (abs(fs_in.viewSpaceZ) < shadowMappingData.cascadeFarPlaneDistances[i])
        {
            cascadeIndex = i;
            break;
        }


    vec3 projectedCoordinates = fs_in.lightSpaceFragmentPositions[cascadeIndex].xyz / fs_in.lightSpaceFragmentPositions[cascadeIndex].w;
    projectedCoordinates = projectedCoordinates * 0.5 + 0.5;

    float closestDepth = texture(shadowMappingData.shadowMaps[cascadeIndex], projectedCoordinates.xy).r;
    float currentDepth = projectedCoordinates.z;

    if (currentDepth > 1.0)
        return 0.0;


    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMappingData.shadowMaps[cascadeIndex], 0);
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMappingData.shadowMaps[cascadeIndex], projectedCoordinates.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth  > pcfDepth ? 1.0 : 0.0;
        }

    shadow /= 9.0;

    return shadow;
}