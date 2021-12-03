#include "ResourceManager.h"

#include <unordered_set>

namespace Seidon
{
    void ResourceManager::Init()
    {
        stbi_set_flip_vertically_on_load(true);

        unsigned char white[] = { 255, 255, 255};
        unsigned char grey[] = { 255 / 2, 255 / 2, 255 / 2};
        unsigned char black[] = { 0, 0, 0};
        unsigned char blue[] = { 128, 128, 255};

        Texture* t = new Texture();
        t->Create(1, 1, white);
        t->path = "albedo_default";
        AddTexture("albedo_default", t);

        t = new Texture();
        t->Create(1, 1, blue);
        t->path = "normal_default";
        AddTexture("normal_default", t);

        t = new Texture();
        t->Create(1, 1, black);
        t->path = "metallic_default";
        AddTexture("metallic_default", t);

        t = new Texture();
        t->Create(1, 1, grey);
        t->path = "roughness_default";
        AddTexture("roughness_default", t);

        t = new Texture();
        t->Create(1, 1, white);
        t->path = "ao_default";
        AddTexture("ao_default", t);

        Material* m = new Material();
        m->name = "default_material";
        m->tint = glm::vec3(1.0f);
        m->albedo = GetTexture("albedo_default");
        m->normal = GetTexture("normal_default");
        m->metallic = GetTexture("metallic_default");
        m->roughness = GetTexture("roughness_default");
        m->ao = GetTexture("ao_default");
        AddMaterial("default_material", m);

        Texture t1;
        t1.Create(1, 1, black);
        t1.path = "default_cubemap";
        HdrCubemap* c = new HdrCubemap();
        c->CreateFromEquirectangularMap(&t1);
        AddCubemap("default_cubemap", c);

        Mesh* mesh = new Mesh();
        mesh->name = "Empty Mesh";
        AddMesh("empty_mesh", mesh);
    }

    void ResourceManager::Destroy()
    {
        for (auto [id, texture] : textures)
            delete texture;
        textures.clear();
        nameToTextureId.clear();

        for (auto [id, material] : materials)
            delete material;
        materials.clear();
        nameToMaterialId.clear();

        for (auto [id, mesh] : meshes)
            delete mesh;
        meshes.clear();
        nameToMeshId.clear();

        for (auto [id, shader] : shaders)
            delete shader;
        shaders.clear();
        nameToShaderId.clear();

        for (auto [id, cubemap] : cubemaps)
            delete cubemap;
        cubemaps.clear();
        nameToCubemapId.clear();
    }

    void ResourceManager::SaveText(YAML::Emitter& out)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "Resources";

        out << YAML::BeginMap;
        
        out << YAML::Key << "Textures" << YAML::BeginSeq;
        for (auto& [id, path] : idToTexturePath)
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Id" << YAML::Value << id;
            out << YAML::Key << "Path" << YAML::Value << path;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        out << YAML::Key << "Meshes" << YAML::BeginSeq;
        for (auto& [id, path] : idToMeshPath)
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Id" << YAML::Value << id;
            out << YAML::Key << "Path" << YAML::Value << path;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        out << YAML::Key << "Materials" << YAML::BeginSeq;
        for (auto& [id, path] : idToMaterialPath)
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Id" << YAML::Value << id;
            out << YAML::Key << "Path" << YAML::Value << path;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        out << YAML::Key << "Cubemaps" << YAML::BeginSeq;
        for (auto& [id, path] : idToCubemapPath)
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Id" << YAML::Value << id;
            out << YAML::Key << "Path" << YAML::Value << path;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        out << YAML::Key << "Shaders" << YAML::BeginSeq;
        for (auto& [id, path] : idToShaderPath)
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Id" << YAML::Value << id;
            out << YAML::Key << "Path" << YAML::Value << path;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        out << YAML::EndMap;
        out << YAML::EndMap;
    }

    void ResourceManager::LoadText(YAML::Node& node)
    {
        YAML::Node resources = node["Resources"];

        for (YAML::Node textures : resources["Textures"])
        {
            idToTexturePath[resources["Id"].as<uint64_t>()] = resources["Path"].as<std::string>();
            LoadTexture(resources["Path"].as<std::string>(), false, resources["Id"].as<uint64_t>());
        }

        for (YAML::Node textures : resources["Meshes"])
            idToMeshPath[resources["Id"].as<uint64_t>()] = resources["Path"].as<std::string>();

        for (YAML::Node textures : resources["Materials"])
            idToMaterialPath[resources["Id"].as<uint64_t>()] = resources["Path"].as<std::string>();

        for (YAML::Node textures : resources["Shaders"])
            idToShaderPath[resources["Id"].as<uint64_t>()] = resources["Path"].as<std::string>();

        for (YAML::Node textures : resources["Cubemaps"])
            idToCubemapPath[resources["Id"].as<uint64_t>()] = resources["Path"].as<std::string>();
    }

    Texture* ResourceManager::LoadTexture(const std::string& path, bool gammaCorrection, UUID id)
    {
        if (nameToTextureId.count(path) > 0) return GetTexture(path);

        Texture* t = new Texture();
        t->id = id;
        t->LoadFromFileAsync(path, gammaCorrection);

        AddTexture(path, t);

        idToTexturePath[t->GetId()] = path;
        return t;
    }

    Shader* ResourceManager::LoadShader(const std::string& path, UUID id)
    {
        if (nameToShaderId.count(path) > 0) return GetShader(path);
        
        Shader* s = new Shader();
        s->id = id;
        s->LoadFromFileAsync(path);

        AddShader(path, s);

        idToShaderPath[s->GetId()] = path;
        return s;
    }

    HdrCubemap* ResourceManager::LoadCubemap(const std::string& path, UUID id)
    {
        if (nameToCubemapId.count(path) > 0) return GetCubemap(path);

        HdrCubemap* c = new HdrCubemap();
        c->LoadFromEquirectangularMap(path);
        c->id = id;

        AddCubemap(path, c);

        idToCubemapPath[c->GetId()] = path;
        return c;
    }

    Mesh* ResourceManager::CreateMesh(const MeshImportData& importData, UUID id)
    {
        Mesh* mesh = new Mesh();
        mesh->name = importData.name;
        mesh->id = id;

        for (auto& m : importData.subMeshes)
        {
            SubMesh* subMesh = new SubMesh();
            subMesh->Create(m.vertices, m.indices, m.name);
      
            mesh->subMeshes.push_back(subMesh);
        }

        AddMesh(importData.name, mesh);
        
        return mesh;
    }

    Material* ResourceManager::CreateMaterial(const MaterialImportData& importData, UUID id)
    {
        if (nameToMaterialId.count(importData.name) > 0) return GetMaterial(importData.name);

        Material* material = new Material();
        material->id = id;
        material->name = importData.name;
        material->tint = importData.tint;
        material->albedo = LoadTexture(importData.albedoMapPath == "" ? "albedo_default" : importData.albedoMapPath);
        material->normal = LoadTexture(importData.normalMapPath == "" ? "normal_default" : importData.normalMapPath);
        material->roughness = LoadTexture(importData.roughnessMapPath == "" ? "roughness_default" : importData.roughnessMapPath);
        material->metallic = LoadTexture(importData.metallicMapPath == "" ? "metallic_default" : importData.metallicMapPath);
        material->ao = LoadTexture(importData.aoMapPath == "" ? "ao_default" : importData.aoMapPath);

        AddMaterial(importData.name, material);
        return material;
    }

    const ModelFileInfo& ResourceManager::LoadModelFile(const std::string& path)
    {
        if (loadedModelFiles.count(path) > 0) return loadedModelFiles[path];

        ModelImporter importer;
        return CreateFromImportData(importer.Import(path));
    }

    ModelFileInfo ResourceManager::CreateFromImportData(const ModelImportData& importData)
    {
        ModelFileInfo res;
        res.filePath = importData.filepath;

        std::vector<Material*> modelMaterials;
        for (auto materialImportData : importData.materials)
        {
            Material* m = CreateMaterial(materialImportData);
            idToMaterialPath[m->id] = importData.filepath;
            modelMaterials.push_back(m);
        }
        
        int i = 0;
        for (auto& meshImportData : importData.meshes)
        {
            Mesh* mesh = CreateMesh(meshImportData);
            mesh->filepath = importData.filepath;
            idToMeshPath[mesh->id] = importData.filepath;

            std::vector<Material*> materials;
            for (int j = 0; j < meshImportData.subMeshes.size(); j++)
                materials.push_back(modelMaterials[meshImportData.subMeshes[j].materialId]);


            res.content.push_back({ mesh, materials });
        }

        loadedModelFiles[importData.filepath] = res;
        return res;
    }

    std::vector<std::string> ResourceManager::GetTextureNames()
    {
        std::vector<std::string> keys;

        for (auto& [key, texture] : nameToTextureId)
            keys.push_back(key);

        return keys;
    }

    std::vector<std::string> ResourceManager::GetMeshNames()
    {
        std::vector<std::string> keys;

        for (auto& [key, mesh] : nameToMeshId)
            keys.push_back(key);

        return keys;
    }

    std::vector<std::string> ResourceManager::GetShaderNames()
    {
        std::vector<std::string> keys;

        for (auto& [key, shader] : nameToShaderId)
            keys.push_back(key);

        return keys;
    }

    std::vector<std::string> ResourceManager::GetMaterialNames()
    {
        std::vector<std::string> keys;

        for (auto& [key, material] : nameToMaterialId)
            keys.push_back(key);

        return keys;
    }

    std::vector<std::string> ResourceManager::GetCubemapNames()
    {
        std::vector<std::string> keys;

        for (auto& [key, cubemap] : nameToCubemapId)
            keys.push_back(key);

        return keys;
    }

    std::vector<Mesh*> ResourceManager::GetModelFileMeshes(const std::string& name)
    {
        std::vector<Mesh*> res;

        for (const auto& [mesh, materials] : loadedModelFiles[name].content)
            res.push_back(mesh);

        return res;
    }

    std::vector<Material*> ResourceManager::GetModelFileMaterials(const std::string& name)
    {
        std::unordered_set<Material*> set;
        std::vector<Material*> res;

        for (const auto& [mesh, materials] : loadedModelFiles[name].content)
            for(const auto& material : materials)
                set.insert(material);

        for (const auto& material : set)
            res.push_back(material);

        return res;
    }
}