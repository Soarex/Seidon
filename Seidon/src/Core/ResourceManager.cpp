#include "ResourceManager.h"

#include "Audio/Sound.h"

#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Armature.h"
#include "Graphics/HdrCubemap.h"
#include "Graphics/Font.h"
#include "Graphics/QuadMesh.h"
#include "Animation/Animation.h"
#include "Physics/MeshCollider.h"

#include <unordered_set>

#include <iostream>
#include <fstream>

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
        AddAsset("albedo_default", t);

        t = new Texture(2);
        t->Create(1, 1, blue);
        t->path = "normal_default";
        AddAsset("normal_default", t);

        t = new Texture(3);
        t->Create(1, 1, black);
        t->path = "metallic_default";
        AddAsset("metallic_default", t);

        t = new Texture(4);
        t->Create(1, 1, grey);
        t->path = "roughness_default";
        AddAsset("roughness_default", t);

        t = new Texture(5);
        t->Create(1, 1, white);
        t->path = "ao_default";
        AddAsset("ao_default", t);

        Shader* s = new Shader(6);
        s->Load("Shaders/PBR.shader");
        AddAsset("default_shader", s);

        Material* m = new Material(7);
        m->name = "default_material";
        AddAsset("default_material", m);

        Texture t1;
        t1.Create(1, 1, black);
        t1.path = "default_cubemap";
        HdrCubemap* c = new HdrCubemap(UUID(8));
        c->CreateFromEquirectangularMap(&t1);
        AddAsset("default_cubemap", c);

        Mesh* mesh = new Mesh(9);
        mesh->name = "Empty Mesh";
        AddAsset("empty_mesh", mesh);

        SkinnedMesh* skinnedMesh = new SkinnedMesh(10);
        skinnedMesh->name = "Empty Skinned Mesh";
        AddAsset("empty_skinned_mesh", skinnedMesh);

        Animation* animation = new Animation(11);
        animation->name = "Default Animation";
        animation->duration = 0;
        animation->ticksPerSecond = 24;
        AddAsset("default_animation", animation);

        m = new Material(12);
        m->name = "Preetham Sky Material";

        m->shader = LoadAsset<Shader>(std::filesystem::current_path().string() + "\\Shaders\\PreethamSky.sdshader", UUID(), true);
        m->ModifyProperty("Turbidity", 2.0f);
        m->ModifyProperty("Azimuth", 0.0f);
        m->ModifyProperty("Inclination", glm::radians(45.0f));
        m->ModifyProperty("Intensity", 1.0f);
        AddAsset("Preetham Sky Material", m);

        Font* font = new Font(13);
        font->name = "empty_font";
        font->fontAtlas->Create(1, 1, grey);
        AddAsset("empty_font", font);

        s = new Shader(14);
        s->Load("Shaders/PBR-Skinned.sdshader");
        AddAsset("default_skinned_shader", s);

        m = new Material(15);
        m->shader = s;
        m->name = "default_skinned_material";
        AddAsset("default_skinned_material", m);

        MeshCollider* collider = new MeshCollider(16);
        collider->name = "quad_mesh_collider";

        QuadMesh quad;
        collider->CreateFromMesh(&quad);
        AddAsset("quad_mesh_collider", collider);

        Sound* sound = new Sound(16);
        sound->name = "empty_sound";
        AddAsset("empty_sound", sound);
    }

    void ResourceManager::Destroy()
    {
        for (auto [id, asset] : assets)
            delete asset;

        assets.clear();
        nameToAssetId.clear();
        idToAssetPath.clear();
        assetPathToId.clear();
    }

    void ResourceManager::Save(std::ofstream& out)
    {
        size_t size = idToAssetPath.size();
        out.write((char*)&size, sizeof(size_t));

        for (auto& [id, path] : idToAssetPath)
        {
            out.write((char*)&id, sizeof(UUID));

            size_t lenght = path.length() + 1;
            out.write((char*)&lenght, sizeof(size_t));
            out.write(path.c_str(), lenght * sizeof(char));
        }
    }

    void ResourceManager::Load(std::ifstream& in)
    {
        char buffer[500];
        size_t size = 0;

        in.read((char*)&size, sizeof(size_t));
        for (int i = 0; i < size; i++)
        {
            UUID id;
            in.read((char*)&id, sizeof(UUID));

            size_t lenght = 0;
            in.read((char*)&lenght, sizeof(size_t));
            in.read(buffer, lenght * sizeof(char));

            std::string path = std::string(buffer);

            idToAssetPath[id] = path;
            assetPathToId[path] = id;
        }
    }

    std::string ResourceManager::AbsoluteToRelativePath(const std::string& absolutePath) 
    { 
        std::string res = std::filesystem::relative(absolutePath, assetDirectory).string();
        
        if (res == ".") res = "";

        return res; 
    }

    std::string ResourceManager::RelativeToAbsolutePath(const std::string& relativePath) 
    { 
        return assetDirectory + "\\" + relativePath; 
    }

    std::vector<Asset*> ResourceManager::GetAssets()
    {
        std::vector<Asset*> res;

        for (auto& [key, asset] : assets)
            res.push_back(asset);

        return res;
    }

    void ResourceManager::AddAsset(const std::string& name, Asset* asset)
    { 
        assets[asset->id] = asset;
        nameToAssetId[name] = asset->id;
        idToAssetPath[asset->id] = name;
        assetPathToId[name] = asset->id;
    }
}