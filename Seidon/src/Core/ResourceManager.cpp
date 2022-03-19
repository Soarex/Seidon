#include "ResourceManager.h"

#include <unordered_set>
#include <yaml-cpp/yaml.h>

namespace Seidon
{
    void ResourceManager::Init()
    {
        stbi_set_flip_vertically_on_load(true);

        unsigned char white[] = { 255, 255, 255};
        unsigned char grey[] = { 255 / 2, 255 / 2, 255 / 2};
        unsigned char black[] = { 0, 0, 0};
        unsigned char blue[] = { 128, 128, 255};

        Texture* t = new Texture(1);
        t->Create(1, 1, white);
        t->path = "albedo_default";
        AddTexture("albedo_default", t);

        t = new Texture(2);
        t->Create(1, 1, blue);
        t->path = "normal_default";
        AddTexture("normal_default", t);

        t = new Texture(3);
        t->Create(1, 1, black);
        t->path = "metallic_default";
        AddTexture("metallic_default", t);

        t = new Texture(4);
        t->Create(1, 1, grey);
        t->path = "roughness_default";
        AddTexture("roughness_default", t);

        t = new Texture(5);
        t->Create(1, 1, white);
        t->path = "ao_default";
        AddTexture("ao_default", t);

        Material* m = new Material(6);
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
        HdrCubemap* c = new HdrCubemap(UUID(7));
        c->CreateFromEquirectangularMap(&t1);
        AddCubemap("default_cubemap", c);

        Mesh* mesh = new Mesh(8);
        mesh->name = "Empty Mesh";
        AddMesh("empty_mesh", mesh);

        Armature* armature = new Armature(9);
        armature->name = "Default Armature";
        AddArmature("default_armature", armature);

        Animation* animation = new Animation(10);
        animation->name = "Default Animation";
        animation->duration = 0;
        animation->ticksPerSecond = 24;
        AddAnimation("default_animation", animation);
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

        for (auto [id, armature] : armatures)
            delete armature;
        armatures.clear();
        nameToArmatureId.clear();
        idToArmaturePath.clear();

        for (auto [id, animation] : animations)
            delete animation;
        animations.clear();
        nameToAnimationId.clear();
        idToAnimationPath.clear();
    }

    void ResourceManager::SaveText(const std::string& path)
    {
        YAML::Emitter out;
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

        out << YAML::Key << "Armatures" << YAML::BeginSeq;
        for (auto& [id, path] : idToArmaturePath)
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Id" << YAML::Value << id;
            out << YAML::Key << "Path" << YAML::Value << path;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        out << YAML::Key << "Animations" << YAML::BeginSeq;
        for (auto& [id, path] : idToAnimationPath)
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

        std::ofstream outStream(path);
        outStream << out.c_str();
    }

    void ResourceManager::LoadText(const std::string& path)
    {
        //Destroy();
        //Init();
        YAML::Node data;

        try
        {
            data = YAML::LoadFile(path);
        }
        catch (YAML::ParserException e)
        {
            std::cout << e.msg << std::endl;
            return;
        }

        YAML::Node resources = data["Resources"];

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

        for (YAML::Node armature : resources["Armatures"])
            idToArmaturePath[armature["Id"].as<uint64_t>()] = armature["Path"].as<std::string>();

        for (YAML::Node animation : resources["Animations"])
            idToAnimationPath[animation["Id"].as<uint64_t>()] = animation["Path"].as<std::string>();
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

    Armature* ResourceManager::LoadArmature(const std::string& path)
    {
        Armature* a = new Armature();
        a->Load(path);

        AddArmature(path, a);

        idToArmaturePath[a->id] = path;
        return a;
    }

    Animation* ResourceManager::LoadAnimation(const std::string& path)
    {
        Animation* a = new Animation();
        a->Load(path);

        AddAnimation(path, a);

        idToAnimationPath[a->id] = path;
        return a;
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

    Armature* ResourceManager::LoadArmature(UUID id)
    {
        std::string& path = idToArmaturePath[id];

        Armature* a = new Armature();
        a->Load(path);

        AddArmature(path, a);

        return a;
    }

    Animation* ResourceManager::LoadAnimation(UUID id)
    {
        std::string& path = idToAnimationPath[id];

        Animation* a = new Animation();
        a->Load(path);

        AddAnimation(path, a);

        return a;
    }

    void ResourceManager::RegisterShader(Shader* shader, const std::string& path)
    {
        idToShaderPath[shader->id] = path;
    }

    void ResourceManager::RegisterMesh(Mesh* mesh, const std::string& path)
    {
        idToMeshPath[mesh->id] = path;
    }

    void ResourceManager::RegisterMaterial(Material* material, const std::string& path)
    {
        idToMaterialPath[material->id] = path;
    }

    void ResourceManager::RegisterTexture(Texture* texture, const std::string& path)
    {
        idToTexturePath[texture->id] = path;
    }

    void ResourceManager::RegisterCubemap(HdrCubemap* cubemap, const std::string& path)
    {
        idToCubemapPath[cubemap->id] = path;
    }

    void ResourceManager::RegisterArmature(Armature* armature, const std::string& path)
    {
        idToArmaturePath[armature->id] = path;
    }

    void ResourceManager::RegisterAnimation(Animation* animation, const std::string& path)
    {
        idToAnimationPath[animation->id] = path;
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

    Armature* ResourceManager::GetOrLoadArmature(const std::string& name)
    {
        if (nameToArmatureId.count(name) > 0) return armatures[nameToArmatureId[name]];

        return LoadArmature(name);
    }

    Animation* ResourceManager::GetOrLoadAnimation(const std::string& name)
    {
        if (nameToAnimationId.count(name) > 0) return animations[nameToAnimationId[name]];

        return LoadAnimation(name);
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

    Armature* ResourceManager::GetOrLoadArmature(UUID id)
    {
        if (armatures.count(id) > 0) return armatures[id];

        return LoadArmature(id);
    }

    Animation* ResourceManager::GetOrLoadAnimation(UUID id)
    {
        if (animations.count(id) > 0) return animations[id];

        return LoadAnimation(id);
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

    std::vector<Armature*>	ResourceManager::GetArmatures()
    {
        std::vector<Armature*> res;

        for (auto& [key, armature] : armatures)
            res.push_back(armature);

        return res;
    }

    std::vector<Animation*>	ResourceManager::GetAnimations()
    {
        std::vector<Animation*> res;

        for (auto& [key, animation] : animations)
            res.push_back(animation);

        return res;
    }

    void ResourceManager::AddTexture(const std::string& name, Texture* texture)
    { 
        textures[texture->GetId()] = texture; 
        nameToTextureId[name] = texture->GetId(); 
        idToTexturePath[texture->GetId()] = texture->path; 
    }

    void ResourceManager::AddMesh(const std::string& name, Mesh* mesh)
    { 
        meshes[mesh->id] = mesh; 
        nameToMeshId[name] = mesh->id; 
        idToMeshPath[mesh->id] = name;
    }

    void ResourceManager::AddShader(const std::string& name, Shader* shader) 
    { 
        shaders[shader->GetId()] = shader; 
        nameToShaderId[name] = shader->GetId(); 
        idToShaderPath[shader->GetId()] = name;
    }

    void ResourceManager::AddMaterial(const std::string& name, Material* material)
    { 
        materials[material->id] = material; 
        nameToMaterialId[name] = material->id; 
        idToMaterialPath[material->id] = name;
    }

    void ResourceManager::AddCubemap(const std::string& name, HdrCubemap* cubemap)
    { 
        cubemaps[cubemap->GetId()] = cubemap; 
        nameToCubemapId[name] = cubemap->GetId(); 
        idToCubemapPath[cubemap->GetId()] = name;
    }

    void ResourceManager::AddAnimation(const std::string& name, Animation* animation)
    { 
        animations[animation->id] = animation; 
        nameToAnimationId[name] = animation->id; 
        idToAnimationPath[animation->id] = name;
    }

    void ResourceManager::AddArmature(const std::string& name, Armature* armature)
    { 
        armatures[armature->id] = armature; 
        nameToArmatureId[name] = armature->id; 
        idToArmaturePath[armature->id] = name;
    }
}