#include "AssetImporter.h"

#include "../Core/Application.h"
#include "../Ecs/Prefab.h"
#include "StringUtils.h"

#include <filesystem>
#include <unordered_map>

namespace Seidon
{
    void AssetImporter::ImportModelFile(const std::string& path)
	{
        std::string directory;
        std::string relativeDirectory;

        std::string absolutePath = Application::Get()->GetResourceManager()->RelativeToAbsolutePath(path);

        const aiScene* scene = importer.ReadFile(absolutePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;

        directory = absolutePath.substr(0, absolutePath.find_last_of('\\'));
        relativeDirectory = Application::Get()->GetResourceManager()->AbsoluteToRelativePath(directory);

        std::cout << directory << " " << relativeDirectory << std::endl;

        for (int i = 0; i < scene->mNumMaterials; i++)
            ImportMaterial(scene->mMaterials[i], relativeDirectory);
        
        std::vector<aiNode*> meshRootNodes;
        std::vector<aiNode*> armatureRootNodes;

        for (int i = 0; i < scene->mRootNode->mNumChildren; i++)
        {
            if (ContainsMeshes(scene->mRootNode->mChildren[i]))
                meshRootNodes.push_back(scene->mRootNode->mChildren[i]);
            else
                armatureRootNodes.push_back(scene->mRootNode->mChildren[i]);
        }

        for (aiNode* armatureRootNode : armatureRootNodes)
        {
            Armature armature;
            armature.name = armatureRootNode->mName.C_Str();
            ProcessBones(armatureRootNode, scene, armature, -1);
            importedArmatures.push_back(armature);
        }

        for (int i = 0; i < scene->mNumAnimations; i++)
            ImportAnimation(scene->mAnimations[i], directory);

        for (aiNode* meshRootNode : meshRootNodes)
        {
            Entity e = ImportHierarchy(meshRootNode, scene, directory);

            Prefab p;
            p.MakeFromEntity(e);
            p.Save(directory + "\\" + e.GetName() + ".sdpref");
        }

        for (auto& m : importedMeshes)
        {
            m->Save(directory + "\\" + m->name + ".sdmesh");

            Application::Get()->GetResourceManager()->RegisterAsset(m, relativeDirectory + "\\" + m->name + ".sdmesh");

            MeshCollider* collider = new MeshCollider();

            if (!collider->CreateFromMesh(m)) continue;

            collider->name = m->name;

            collider->Save(directory + "\\" + m->name + ".sdcoll");
            Application::Get()->GetResourceManager()->RegisterAsset(collider, relativeDirectory + "\\" + m->name + ".sdcoll");

            delete collider;
            delete m;
        }

        for (auto& m : importedSkinnedMeshes)
        {
            for (auto& s : m->subMeshes)
                for (auto& v : s->vertices)
                {
                    float weightSum = v.weights[0] + v.weights[1] + v.weights[2] + v.weights[3];

                    if (weightSum > 0)
                        v.weights /= weightSum;
                }
            
            m->Save(directory + "\\" + m->name + ".sdskmesh");
   
            Application::Get()->GetResourceManager()->RegisterAsset(m, relativeDirectory + "\\" + m->name + ".sdskmesh");
            delete m;
        }

        for (auto& [name, material] : importedMaterials)
        {
            material->Save(directory + "\\" + material->name + ".sdmat");
            delete material;
        }

        for (auto& [name, texture] : importedTextures)
            delete texture;


        importedMaterials.clear();
        importedTextures.clear();
        importedArmatures.clear();
        importedMeshes.clear();
        importedSkinnedMeshes.clear();

        prefabScene.Destroy();
	} 

    bool AssetImporter::ContainsMeshes(aiNode* node)
    {
        if (node->mNumMeshes > 0) return true;

        for (int i = 0; i < node->mNumChildren; i++)
            if (ContainsMeshes(node->mChildren[i])) return true;
        
        return false;
    }

    Entity AssetImporter::ImportHierarchy(aiNode* node, const aiScene* scene, const std::string& directory)
    {
        Entity e = prefabScene.CreateEntity();

        glm::mat4 glmTransform = glm::make_mat4x4((float*)&(node->mTransformation.Transpose()));
        e.GetComponent<TransformComponent>().SetFromMatrix(glmTransform);

        e.GetComponent<NameComponent>().name = node->mName.C_Str();

        if (node->mNumMeshes > 0)
        {
            bool isSkinned = false;

            for (int i = 0; i < node->mNumMeshes; i++)
                if (scene->mMeshes[node->mMeshes[i]]->HasBones()) isSkinned = true;

            if (isSkinned)
            {
                SkinnedRenderComponent& r = e.AddComponent<SkinnedRenderComponent>();

                SkinnedMesh* mesh = ImportSkinnedMesh(node, scene, r.materials);
                r.SetMesh(mesh);
                importedSkinnedMeshes.push_back(mesh);
            }
            else
            {
                RenderComponent& r = e.AddComponent<RenderComponent>();

                Mesh* mesh = ImportMesh(node, scene, r.materials);
                r.SetMesh(mesh);
                importedMeshes.push_back(mesh);
            }
        }
        
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ImportChildNodes(node->mChildren[i], scene, e, directory);

        return e;
    }

    void AssetImporter::ImportChildNodes(aiNode* node, const aiScene* scene, Entity parent, const std::string& directory)
    {
        Entity e = prefabScene.CreateEntity();

        glm::mat4 glmTransform = glm::make_mat4x4((float*)&(node->mTransformation.Transpose()));
        e.GetComponent<TransformComponent>().SetFromMatrix(glmTransform);

        e.SetParent(parent);
        e.GetComponent<NameComponent>().name = node->mName.C_Str();

        if (node->mNumMeshes > 0)
        {
            bool isSkinned = false;

            for (int i = 0; i < node->mNumMeshes; i++)
                if (scene->mMeshes[node->mMeshes[i]]->HasBones()) isSkinned = true;

            if (isSkinned)
            {
                SkinnedRenderComponent& r = e.AddComponent<SkinnedRenderComponent>();

                SkinnedMesh* mesh = ImportSkinnedMesh(node, scene, r.materials);
                r.SetMesh(mesh);
                importedSkinnedMeshes.push_back(mesh);
            }
            else
            {
                RenderComponent& r = e.AddComponent<RenderComponent>();

                Mesh* mesh = ImportMesh(node, scene, r.materials);
                r.SetMesh(mesh);
                importedMeshes.push_back(mesh);
            }
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ImportChildNodes(node->mChildren[i], scene, e, directory);
    }

    Mesh* AssetImporter::ImportMesh(aiNode* node, const aiScene* scene, std::vector<Material*>& materials)
    {
        Mesh* m = new Mesh();
        m->name = node->mName.C_Str();

        materials.reserve(node->mNumMeshes);

        for (int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            Submesh* submesh = ProcessSubmesh(mesh);

            m->subMeshes.push_back(submesh);

            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            aiString materialName;
            material->Get(AI_MATKEY_NAME, materialName);

            materials.push_back(importedMaterials[materialName.C_Str()]);
        }

        return m;
    }

    SkinnedMesh* AssetImporter::ImportSkinnedMesh(aiNode* node, const aiScene* scene, std::vector<Material*>& materials)
    {
        SkinnedMesh* m = new SkinnedMesh();
        m->name = node->mName.C_Str();

        materials.reserve(node->mNumMeshes);
        
        Armature* armature = nullptr;
        for (int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            std::string rootBoneName = mesh->mBones[0]->mName.C_Str();
            for (Armature& a : importedArmatures)
            {
                //if (mesh->mNumBones != a.bones.size()) continue;

                if (a.bones[0].name == rootBoneName || a.bones[1].name == rootBoneName)
                    armature = &a;
            }

            if (!armature)
            {
                std::cerr << "Import error: Submesh armature not found" << std::endl;
                return nullptr;
            }

            SkinnedSubmesh* submesh = ProcessSkinnedSubmesh(mesh, armature);
            m->subMeshes.push_back(submesh);
            
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            aiString materialName;
            material->Get(AI_MATKEY_NAME, materialName);

            importedMaterials[materialName.C_Str()]->shader = Application::Get()->GetResourceManager()->GetAsset<Shader>("default_skinned_shader");
            materials.push_back(importedMaterials[materialName.C_Str()]);
            
        }

        m->armature = *armature;
        return m;
    }

    Submesh* AssetImporter::ProcessSubmesh(aiMesh* mesh)
    {
        Submesh* submesh = new Submesh();

        submesh->vertices.reserve(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            glm::vec3 temp;

            temp.x = mesh->mVertices[i].x;
            temp.y = mesh->mVertices[i].y;
            temp.z = mesh->mVertices[i].z;
            vertex.position = temp;

            if (mesh->HasNormals())
            {
                temp.x = mesh->mNormals[i].x;
                temp.y = mesh->mNormals[i].y;
                temp.z = mesh->mNormals[i].z;
                vertex.normal = temp;

                temp.x = mesh->mTangents[i].x;
                temp.y = mesh->mTangents[i].y;
                temp.z = mesh->mTangents[i].z;
                vertex.tangent = temp;
            }

            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoords = vec;
            }
            else
                vertex.texCoords = glm::vec2(0.0f, 0.0f);

            submesh->vertices.push_back(vertex);
        }

        submesh->indices.reserve(mesh->mNumFaces);
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                submesh->indices.push_back(face.mIndices[j]);
        }

        submesh->name = mesh->mName.C_Str();

        return submesh;
    }

    SkinnedSubmesh* AssetImporter::ProcessSkinnedSubmesh(aiMesh* mesh, Armature* armature)
    {
        SkinnedSubmesh* submesh = new SkinnedSubmesh();

        submesh->vertices.reserve(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            SkinnedVertex vertex;

            glm::vec3 temp;

            temp.x = mesh->mVertices[i].x;
            temp.y = mesh->mVertices[i].y;
            temp.z = mesh->mVertices[i].z;
            vertex.position = temp;

            if (mesh->HasNormals())
            {
                temp.x = mesh->mNormals[i].x;
                temp.y = mesh->mNormals[i].y;
                temp.z = mesh->mNormals[i].z;
                vertex.normal = temp;

                temp.x = mesh->mTangents[i].x;
                temp.y = mesh->mTangents[i].y;
                temp.z = mesh->mTangents[i].z;
                vertex.tangent = temp;
            }

            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoords = vec;
            }
            else
                vertex.texCoords = glm::vec2(0.0f, 0.0f);

            submesh->vertices.push_back(vertex);
        }

        submesh->indices.reserve(mesh->mNumFaces);
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                submesh->indices.push_back(face.mIndices[j]);
        }

        submesh->name = mesh->mName.C_Str();
        //if (!armature) return submesh;

        std::unordered_map<std::string, int> boneNameToIndex;

        for (int i = 0; i < mesh->mNumBones; i++)
        {
            aiBone* bone = mesh->mBones[i];

            std::string boneName = bone->mName.C_Str();

            if (boneNameToIndex.count(boneName) == 0)
            {
                int i = 0;
                for (BoneData& data : armature->bones)
                {
                    if (data.name == boneName)
                    {
                        data.inverseBindPoseMatrix = glm::make_mat4x4((float*)&(bone->mOffsetMatrix.Transpose()));

                        boneNameToIndex[boneName] = i;
                    }

                    i++;
                }
            }

            for (int j = 0; j < bone->mNumWeights; j++)
            {
                aiVertexWeight& weight = bone->mWeights[j];

                for (int k = 0; k < SkinnedVertex::MAX_BONES_PER_VERTEX; k++)
                {
                    if (submesh->vertices[weight.mVertexId].weights[k] != 0) continue;

                    submesh->vertices[weight.mVertexId].weights[k] = weight.mWeight;
                    submesh->vertices[weight.mVertexId].boneIds[k] = boneNameToIndex[boneName];

                    break;
                }
            }
        }

        return submesh;
    }
    
    void AssetImporter::ImportAnimation(aiAnimation* animation, const std::string& directory)
    {
        Animation anim;
        anim.name = animation->mName.C_Str();

        std::replace(anim.name.begin(), anim.name.end(), '|', '-');

        anim.duration = animation->mDuration;
        anim.ticksPerSecond = animation->mTicksPerSecond;

        anim.channels.reserve(animation->mNumChannels);

        Armature* armature = nullptr;

        for (int j = 0; j < animation->mNumChannels; j++)
        {
            AnimationChannel channel;
            channel.boneName = animation->mChannels[j]->mNodeName.C_Str();

            if (!armature)
            {
                for (Armature& a : importedArmatures)
                    for (BoneData& bone : a.bones)
                        if (channel.boneName == bone.name)
                            armature = &a;

                if (!armature)
                {
                    std::cerr << "Error importing animation " << anim.name << ": Couldn't find matching armature" << std::endl;
                    return;
                }
            }

            for (BoneData& bone : armature->bones)
                if (channel.boneName == bone.name)
                    channel.boneId = bone.id;

            channel.positionKeys.reserve(animation->mChannels[j]->mNumPositionKeys);

            for (int k = 0; k < animation->mChannels[j]->mNumPositionKeys; k++)
            {
                PositionKey key;
                key.time = animation->mChannels[j]->mPositionKeys[k].mTime;
                key.value.x = animation->mChannels[j]->mPositionKeys[k].mValue.x;
                key.value.y = animation->mChannels[j]->mPositionKeys[k].mValue.y;
                key.value.z = animation->mChannels[j]->mPositionKeys[k].mValue.z;

                channel.positionKeys.push_back(key);
            }


            channel.rotationKeys.reserve(animation->mChannels[j]->mNumRotationKeys);

            for (int k = 0; k < animation->mChannels[j]->mNumRotationKeys; k++)
            {
                RotationKey key;
                key.time = animation->mChannels[j]->mRotationKeys[k].mTime;

                key.value.x = animation->mChannels[j]->mRotationKeys[k].mValue.x;
                key.value.y = animation->mChannels[j]->mRotationKeys[k].mValue.y;
                key.value.z = animation->mChannels[j]->mRotationKeys[k].mValue.z;
                key.value.w = animation->mChannels[j]->mRotationKeys[k].mValue.w;

                channel.rotationKeys.push_back(key);
            }


            channel.scalingKeys.reserve(animation->mChannels[j]->mNumScalingKeys);

            for (int k = 0; k < animation->mChannels[j]->mNumScalingKeys; k++)
            {
                ScalingKey key;
                key.time = animation->mChannels[j]->mScalingKeys[k].mTime;
                key.value.x = animation->mChannels[j]->mScalingKeys[k].mValue.x;
                key.value.y = animation->mChannels[j]->mScalingKeys[k].mValue.y;
                key.value.z = animation->mChannels[j]->mScalingKeys[k].mValue.z;

                channel.scalingKeys.push_back(key);
            }

            anim.channels.push_back(channel);
        }

        std::sort(anim.channels.begin(), anim.channels.end(), [](AnimationChannel& a, AnimationChannel& b) { return a.boneId < b.boneId; });
        anim.Save(directory + "\\" + anim.name + ".sdanim");

        std::string relativeDirectory = Application::Get()->GetResourceManager()->AbsoluteToRelativePath(directory);
        Application::Get()->GetResourceManager()->RegisterAsset(&anim, relativeDirectory + "\\" + anim.name + ".sdanim");
    }
    
    void AssetImporter::ProcessBones(aiNode* node, const aiScene* scene, Armature& armature, int parentId)
    {
        BoneData bone;
        bone.name = node->mName.C_Str();
        
        if (bone.name.compare(bone.name.size() - 3, 3, "End") != 0)
        {
            bone.id = armature.bones.size();
            bone.parentId = parentId;
            bone.inverseBindPoseMatrix = glm::identity<glm::mat4>();

            armature.bones.push_back(bone);
        }
        
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessBones(node->mChildren[i], scene, armature, bone.id);
    }

    Material* AssetImporter::ImportMaterial(aiMaterial* material, const std::string& directory)
    {
        aiString name;
        material->Get(AI_MATKEY_NAME, name);
        if (importedMaterials.count(name.C_Str()) > 0) return importedMaterials[material->GetName().C_Str()];

        ResourceManager& resourceManager = *Application::Get()->GetResourceManager();

        Material* m = new Material();

        m->name = std::string(name.C_Str());
        m->shader = resourceManager.GetAsset<Shader>("default_shader");

        MetaType& layout = *m->shader->GetBufferLayout();
        byte* ptr = m->data;

        aiColor3D tint;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, tint);

        layout.ModifyMember<glm::vec2>("UV Scale", m->data, glm::vec2(1));
        layout.ModifyMember<glm::vec3>("Tint", m->data, glm::vec3(tint.r, tint.g, tint.b));

        aiString str;
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            material->GetTexture(aiTextureType_DIFFUSE, 0, &str);

            Texture* t = ImportTexture(directory + "\\" + std::string(str.C_Str()), true);

            if (t)
                layout.ModifyMember<Texture*>("Albedo", m->data, t);
            else
                layout.ModifyMember<Texture*>("Albedo", m->data, resourceManager.GetAsset<Texture>("albedo_default"));
        }
        else
            layout.ModifyMember<Texture*>("Albedo", m->data, resourceManager.GetAsset<Texture>("albedo_default"));


        if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            material->GetTexture(aiTextureType_NORMALS, 0, &str);

            Texture* t = ImportTexture(directory + "\\" + std::string(str.C_Str()), false);

            if (t)
                layout.ModifyMember<Texture*>("Normal", m->data, t);
            else
                layout.ModifyMember<Texture*>("Normal", m->data, resourceManager.GetAsset<Texture>("normal_default"));
        }
        else
            layout.ModifyMember<Texture*>("Normal", m->data, resourceManager.GetAsset<Texture>("normal_default"));

        if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            material->GetTexture(aiTextureType_SHININESS, 0, &str);

            Texture* t = ImportTexture(directory + "\\" + std::string(str.C_Str()), false);

            if (t)
                layout.ModifyMember<Texture*>("Roughness", m->data, t);
            else
                layout.ModifyMember<Texture*>("Roughness", m->data, resourceManager.GetAsset<Texture>("roughness_default"));
        }
        else
            layout.ModifyMember<Texture*>("Roughness", m->data, resourceManager.GetAsset<Texture>("roughness_default"));


        if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            material->GetTexture(aiTextureType_SPECULAR, 0, &str);

            Texture* t = ImportTexture(directory + "\\" + std::string(str.C_Str()), false);

            if (t)
                layout.ModifyMember<Texture*>("Metallic", m->data, t);
            else
                layout.ModifyMember<Texture*>("Metallic", m->data, resourceManager.GetAsset<Texture>("metallic_default"));
        }
        else
            layout.ModifyMember<Texture*>("Metallic", m->data, resourceManager.GetAsset<Texture>("metallic_default"));

        if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
        {
            material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &str);
             
            Texture* t = ImportTexture(directory + "\\" + std::string(str.C_Str()), false);

            if (t)
                layout.ModifyMember<Texture*>("Ambient Occlusion", m->data, t);
            else
                layout.ModifyMember<Texture*>("Ambient Occlusion", m->data, resourceManager.GetAsset<Texture>("ao_default"));
        }
        else
            layout.ModifyMember<Texture*>("Ambient Occlusion", m->data, resourceManager.GetAsset<Texture>("ao_default"));

        Application::Get()->GetResourceManager()->RegisterAsset(m, directory + "\\" + m->name + ".sdmat");

        importedMaterials[m->name] = m;
        return m;
    }

    Texture* AssetImporter::ImportTexture(const std::string& path, bool gammaCorrection, bool flip, ClampingMode clampingMode)
    {
        if (importedTextures.count(path) > 0) return importedTextures[path];

        Texture* t = new Texture();

        std::string absolutePath = Application::Get()->GetResourceManager()->RelativeToAbsolutePath(path);

        if (!t->Import(absolutePath, gammaCorrection, flip, clampingMode))
        {
            //delete t;
            return nullptr;
        }

        t->path = ChangeSuffix(path, ".sdtex");

        t->Save(ChangeSuffix(absolutePath, ".sdtex"));

        Application::Get()->GetResourceManager()->RegisterAsset(t, t->path);

        importedTextures[path] = t;
        return t;
    }

    HdrCubemap* AssetImporter::ImportCubemap(const std::string& path)
    {
        HdrCubemap* c = new HdrCubemap();
        std::string absolutePath = Application::Get()->GetResourceManager()->RelativeToAbsolutePath(path);

        c->LoadFromEquirectangularMap(absolutePath);

        c->name = ChangeSuffix(path, ".sdhdr");
        c->Save(ChangeSuffix(absolutePath, ".sdhdr"));

        Application::Get()->GetResourceManager()->RegisterAsset(c, c->name);

        return c;
    }

    Font* AssetImporter::ImportFont(const std::string& path)
    {
        Font* f = new Font();
        std::string absolutePath = Application::Get()->GetResourceManager()->RelativeToAbsolutePath(path);

        f->Import(absolutePath);

        f->name = ChangeSuffix(path, ".sdfont");

        f->Save(ChangeSuffix(absolutePath, ".sdfont"));

        Application::Get()->GetResourceManager()->RegisterAsset(f, f->GetName());
        return f;
    }
}