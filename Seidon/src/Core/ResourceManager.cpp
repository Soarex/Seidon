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
        idToTexturePath.clear();

        for (auto [id, material] : materials)
            delete material;
        materials.clear();
        nameToMaterialId.clear();
        idToMaterialPath.clear();

        for (auto [id, mesh] : meshes)
            delete mesh;
        meshes.clear();
        nameToMeshId.clear();
        idToMeshPath.clear();

        for (auto [id, shader] : shaders)
            delete shader;
        shaders.clear();
        nameToShaderId.clear();
        idToShaderPath.clear();

        for (auto [id, cubemap] : cubemaps)
            delete cubemap;
        cubemaps.clear();
        nameToCubemapId.clear();
        idToCubemapPath.clear();
    }

    void ResourceManager::SaveText(YAML::Emitter& out)
    {
        idToTexturePath.clear();
        for (auto& [id, texture] : textures)
        {
            std::string path = "Assets/Imported/" + std::to_string(id) + ".sdtex";
            texture->Save(path);
            idToTexturePath[id] = path;
        }

        idToMeshPath.clear();
        for (auto& [id, mesh] : meshes)
        {
            std::string path = "Assets/Imported/" + std::to_string(id) + ".sdmesh";
            mesh->Save(path);
            idToMeshPath[id] = path;
        }

        idToMaterialPath.clear();
        for (auto& [id, material] : materials)
        {
            std::string path = "Assets/Imported/" + std::to_string(id) + ".sdmat";
            material->Save(path);
            idToMaterialPath[id] = path;
        }

        idToCubemapPath.clear();
        for (auto& [id, cubemap] : cubemaps)
        {
            std::string path = "Assets/Imported/" + std::to_string(id) + ".sdhdr";
            cubemap->Save(path);
            idToCubemapPath[id] = path;
        }

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
        Destroy();
        Init();

        YAML::Node resources = node["Resources"];

        for (YAML::Node texture : resources["Textures"])
            idToTexturePath[texture["Id"].as<uint64_t>()] = texture["Path"].as<std::string>();

        for (YAML::Node mesh : resources["Meshes"])
            idToMeshPath[mesh["Id"].as<uint64_t>()] = mesh["Path"].as<std::string>();

        for (YAML::Node material : resources["Materials"])
            idToMaterialPath[material["Id"].as<uint64_t>()] = material["Path"].as<std::string>();

        for (YAML::Node shader : resources["Shaders"])
            idToShaderPath[shader["Id"].as<uint64_t>()] = shader["Path"].as<std::string>();

        for (YAML::Node cubemap : resources["Cubemaps"])
            idToCubemapPath[cubemap["Id"].as<uint64_t>()] = cubemap["Path"].as<std::string>();
    }


    Shader* ResourceManager::LoadShader(const std::string& path, UUID id)
    {   
        Shader* s = new Shader();
        s->id = id;
        s->LoadFromFileAsync(path);

        AddShader(path, s);

        idToShaderPath[s->GetId()] = path;
        return s;
    }

    Mesh* ResourceManager::LoadMesh(const std::string& path)
    {
        Mesh* m = new Mesh();
        m->Load(path);

        AddMesh(path, m);

        idToMeshPath[m->id] = path;
        return m;
    }

    Material* ResourceManager::LoadMaterial(const std::string& path)
    {
        Material* m = new Material();
        m->Load(path);

        AddMaterial(path, m);

        idToMaterialPath[m->id] = path;
        return m;
    }

    Texture* ResourceManager::LoadTexture(const std::string& path)
    {
        Texture* t = new Texture();
        t->Load(path);

        AddTexture(path, t);

        idToTexturePath[t->GetId()] = path;
        return t;
    }

    HdrCubemap* ResourceManager::LoadCubemap(const std::string& path)
    {
        HdrCubemap* c = new HdrCubemap();
        c->Load(path);

        AddCubemap(path, c);

        idToCubemapPath[c->GetId()] = path;
        return c;
    }

    Shader* ResourceManager::LoadShader(UUID id)
    {
        std::string& path = idToShaderPath[id];

        Shader* s = new Shader();
        s->id = id;
        s->LoadFromFileAsync(path);

        AddShader(path, s);

        return s;
    }

    Mesh* ResourceManager::LoadMesh(UUID id)
    {
        std::string& path = idToMeshPath[id];

        Mesh* m = new Mesh();
        m->Load(path);

        AddMesh(path, m);

        return m;
    }

    Material* ResourceManager::LoadMaterial(UUID id)
    {
        std::string& path = idToMaterialPath[id];

        Material* m = new Material();
        m->Load(path);

        AddMaterial(path, m);

        return m;
    }

    Texture* ResourceManager::LoadTexture(UUID id)
    {
        std::string& path = idToTexturePath[id];

        Texture* t = new Texture();
        t->Load(path);

        AddTexture(path, t);

        return t;
    }

    HdrCubemap* ResourceManager::LoadCubemap(UUID id)
    {
        std::string& path = idToCubemapPath[id];

        HdrCubemap* c = new HdrCubemap();
        c->Load(path);

        AddCubemap(path, c);

        return c;
    }

    Texture* ResourceManager::ImportTexture(const std::string& path, bool gammaCorrection, UUID id)
    {
        Texture* t = new Texture();
        t->id = id;
        t->ImportAsync(path, gammaCorrection);

        AddTexture(path, t);

        idToTexturePath[t->GetId()] = path;
        return t;
    }

    HdrCubemap* ResourceManager::ImportCubemap(const std::string& path, UUID id)
    {
        HdrCubemap* c = new HdrCubemap();
        c->LoadFromEquirectangularMap(path);
        c->id = id;

        AddCubemap(path, c);

        idToCubemapPath[c->GetId()] = path;
        return c;
    }

    Texture* ResourceManager::GetOrImportTexture(const std::string& path, bool gammaCorrection, UUID id)
    {
        if (nameToTextureId.count(path) > 0) return GetTexture(path);

        return ImportTexture(path, gammaCorrection, id);
    }

    HdrCubemap* ResourceManager::GetOrImportCubemap(const std::string& path, UUID id)
    {
        if (nameToCubemapId.count(path) > 0) return GetCubemap(path);
        
        return ImportCubemap(path, id);
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
        material->albedo = GetOrImportTexture(importData.albedoMapPath == "" ? "albedo_default" : importData.albedoMapPath, true);
        material->normal = GetOrImportTexture(importData.normalMapPath == "" ? "normal_default" : importData.normalMapPath);
        material->roughness = GetOrImportTexture(importData.roughnessMapPath == "" ? "roughness_default" : importData.roughnessMapPath);
        material->metallic = GetOrImportTexture(importData.metallicMapPath == "" ? "metallic_default" : importData.metallicMapPath);
        material->ao = GetOrImportTexture(importData.aoMapPath == "" ? "ao_default" : importData.aoMapPath);

        AddMaterial(importData.name, material);
        return material;
    }

    const ModelFileInfo& ResourceManager::ImportModelFile(const std::string& path)
    {
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

        return res;
    }

    Texture* ResourceManager::GetOrLoadTexture(const std::string& name)
    {
        if (nameToTextureId.count(name) > 0) return textures[nameToTextureId[name]];

        return LoadTexture(name);
    }

    Mesh* ResourceManager::GetOrLoadMesh(const std::string& name)
    {
        if (nameToMeshId.count(name) > 0) return meshes[nameToMeshId[name]];

        return LoadMesh(name);
    }

    Shader* ResourceManager::GetOrLoadShader(const std::string& name)
    {
        if (nameToShaderId.count(name) > 0) return shaders[nameToShaderId[name]];

        return LoadShader(name);
    }

    Material* ResourceManager::GetOrLoadMaterial(const std::string& name)
    {
        if (nameToMaterialId.count(name) > 0) return materials[nameToMaterialId[name]];

        return LoadMaterial(name);
    }

    HdrCubemap* ResourceManager::GetOrLoadCubemap(const std::string& name)
    {
        if (nameToCubemapId.count(name) > 0) return cubemaps[nameToCubemapId[name]];

        return LoadCubemap(name);
    }

    Texture* ResourceManager::GetOrLoadTexture(UUID id)
    {
        if (textures.count(id) > 0) return textures[id];

        return LoadTexture(id);
    }

    Mesh* ResourceManager::GetOrLoadMesh(UUID id)
    {
        if (meshes.count(id) > 0) return meshes[id];

        return LoadMesh(id);
    }

    Shader* ResourceManager::GetOrLoadShader(UUID id)
    {
        if (shaders.count(id) > 0) return shaders[id];

        return LoadShader(id);
    }

    Material* ResourceManager::GetOrLoadMaterial(UUID id)
    {
        if (materials.count(id) > 0) return materials[id];

        return LoadMaterial(id);
    }

    HdrCubemap* ResourceManager::GetOrLoadCubemap(UUID id)
    {
        if (cubemaps.count(id) > 0) return cubemaps[id];

        return LoadCubemap(id);
    }


    std::vector<Texture*> ResourceManager::GetTextures()
    {
        std::vector<Texture*> res;

        for (auto& [key, texture] : textures)
            res.push_back(texture);

        return res;
    }

    std::vector<Mesh*> ResourceManager::GetMeshes()
    {
        std::vector<Mesh*> res;

        for (auto& [key, mesh] : meshes)
            res.push_back(mesh);

        return res;
    }

    std::vector<Shader*> ResourceManager::GetShaders()
    {
        std::vector<Shader*> res;

        for (auto& [key, shader] : shaders)
            res.push_back(shader);

        return res;
    }

    std::vector<Material*> ResourceManager::GetMaterials()
    {
        std::vector<Material*> res;

        for (auto& [key, material] : materials)
            res.push_back(material);

        return res;
    }

    std::vector<HdrCubemap*> ResourceManager::GetCubemaps()
    {
        std::vector<HdrCubemap*> res;

        for (auto& [key, cubemap] : cubemaps)
            res.push_back(cubemap);

        return res;
    }
}