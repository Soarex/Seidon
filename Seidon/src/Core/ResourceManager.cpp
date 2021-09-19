#include "ResourceManager.h"

namespace Seidon
{
    std::map<std::string, Texture*>		ResourceManager::textures;
    std::map<std::string, Material*>	ResourceManager::materials;
    std::map<std::string, Mesh*>		ResourceManager::meshes;
    std::map<std::string, Shader*>		ResourceManager::shaders;

    void ResourceManager::Init()
    {
        stbi_set_flip_vertically_on_load(true);

        unsigned char white[] = { 255, 255, 255};
        unsigned char grey[] = { 255 / 2, 255 / 2, 255 / 2};
        unsigned char black[] = { 0, 0, 0};
        unsigned char blue[] = { 128, 128, 255};

        textures["albedo_default"] = new Texture();
        textures["albedo_default"]->Create(1, 1, white);
        textures["albedo_default"]->path = "albedo_default";

        textures["normal_default"] = new Texture();
        textures["normal_default"]->Create(1, 1, blue);
        textures["normal_default"]->path = "normal_default";

        textures["metallic_default"] = new Texture();
        textures["metallic_default"]->Create(1, 1, black);
        textures["metallic_default"]->path = "metallic_default";

        textures["roughness_default"] = new Texture();
        textures["roughness_default"]->Create(1, 1, grey);
        textures["roughness_default"]->path = "roughness_default";

        textures["ao_default"] = new Texture();
        textures["ao_default"]->Create(1, 1, white);
        textures["ao_default"]->path = "ao_default";

        materials["default_material"] = new Material();
        materials["default_material"]->name = "default_material";
        materials["default_material"]->tint = glm::vec3(1.0f);
        materials["default_material"]->albedo = textures["albedo_default"];
        materials["default_material"]->normal = textures["normal_default"];
        materials["default_material"]->metallic = textures["metallic_default"];
        materials["default_material"]->roughness = textures["roughness_default"];
        materials["default_material"]->ao = textures["ao_default"];

        Mesh* mesh = new Mesh();
        mesh->name = "Empty Mesh";
        meshes["Empty Mesh"] = mesh;
    }

    void ResourceManager::Destroy()
    {
        for (auto [name, texture] : textures)
            delete texture;
        textures.clear();

        for (auto [name, material] : materials)
            delete material;
        materials.clear();

        for (auto [name, mesh] : meshes)
            delete mesh;
        meshes.clear();

        for (auto [name, shaders] : shaders)
            delete shaders;
        shaders.clear();
    }

    Texture* ResourceManager::LoadTexture(const std::string& path, bool gammaCorrection)
    {
        if (textures.count(path) > 0) return textures[path];

        textures[path] = new Texture();
        textures[path]->LoadFromFileAsync(path, gammaCorrection);

        return textures[path];
    }

    Shader* ResourceManager::LoadShader(const std::string& path)
    {
        if (shaders.count(path) > 0) return shaders[path];
        
        shaders[path] = new Shader();
        shaders[path]->LoadFromFileAsync(path);

        return shaders[path];
    }

    Mesh* ResourceManager::CreateMesh(MeshImportData& importData)
    {
        Mesh* mesh = new Mesh();
        mesh->name = importData.name;

        for (auto& m : importData.subMeshes)
        {
            SubMesh* subMesh = new SubMesh();
            subMesh->Create(m.vertices, m.indices, GetMaterial("default_material"), m.name);
      
            mesh->subMeshes.push_back(subMesh);
        }

        meshes[importData.name] = mesh;

        return mesh;
    }

    Material* ResourceManager::CreateMaterial(MaterialImportData& importData)
    {
        Material* material = new Material();
        material->name = importData.name;
        material->tint = importData.tint;
        material->albedo = LoadTexture(importData.albedoMapPath == "" ? "albedo_default" : importData.albedoMapPath);
        material->normal = LoadTexture(importData.normalMapPath == "" ? "normal_default" : importData.normalMapPath);
        material->roughness = LoadTexture(importData.roughnessMapPath == "" ? "roughness_default" : importData.roughnessMapPath);
        material->metallic = LoadTexture(importData.metallicMapPath == "" ? "metallic_default" : importData.metallicMapPath);
        material->ao = LoadTexture(importData.aoMapPath == "" ? "ao_default" : importData.aoMapPath);

        materials[importData.name] = material;

        return material;
    }

    std::vector<Mesh*> ResourceManager::CreateFromImportData(ModelImportData& importData)
    {
        std::vector<Mesh*> res;

        std::vector<Material*> materials;
        for (auto materialImportData : importData.materials)
            materials.push_back(CreateMaterial(materialImportData));

        int i = 0;
        for (auto& meshImportData : importData.meshes)
        {
            Mesh* mesh = CreateMesh(meshImportData);

            for (int j = 0; j < meshImportData.subMeshes.size(); j++)
                mesh->subMeshes[j]->material = materials[meshImportData.subMeshes[j].materialId];


            res.push_back(mesh);
        }

        return res;
    }

    std::vector<std::string> ResourceManager::GetTextureKeys()
    {
        std::vector<std::string> keys;

        for (auto& [key, texture] : textures)
            keys.push_back(key);

        return keys;
    }

    std::vector<std::string> ResourceManager::GetMeshKeys()
    {
        std::vector<std::string> keys;

        for (auto& [key, mesh] : meshes)
            keys.push_back(key);

        return keys;
    }

    std::vector<std::string> ResourceManager::GetShaderKeys()
    {
        std::vector<std::string> keys;

        for (auto& [key, shader] : shaders)
            keys.push_back(key);

        return keys;
    }

    std::vector<std::string> ResourceManager::GetMaterialKeys()
    {
        std::vector<std::string> keys;

        for (auto& [key, material] : materials)
            keys.push_back(key);

        return keys;
    }
}