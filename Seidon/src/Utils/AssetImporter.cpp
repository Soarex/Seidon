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

        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;

        directory = path.substr(0, path.find_last_of('\\'));

        for (int i = 0; i < scene->mNumMaterials; i++)
            ImportMaterial(scene->mMaterials[i], directory);

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
            armature.Save(directory + "\\" + armature.name + ".sdarm");

            Application::Get()->GetResourceManager()->RegisterArmature(&armature, directory + "\\" + armature.name + ".sdarm");

            importedArmatures.push_back(armature);
        }

        for(aiNode* meshRootNode : meshRootNodes)
            ImportMeshes(meshRootNode, scene, aiMatrix4x4(), directory);
       
        for (int i = 0; i < scene->mNumAnimations; i++)
            ImportAnimation(scene->mAnimations[i], directory);

        for (auto& m : importedMeshes)
        {
            for (auto& s : m->subMeshes)
                for (auto& v : s->vertices)
                {
                    float weightSum = v.weights[0] + v.weights[1] + v.weights[2] + v.weights[3];

                    if (weightSum > 0)
                        v.weights /= weightSum;
                }
            
            m->Save(directory + "\\" + m->name + ".sdmesh");
            Application::Get()->GetResourceManager()->RegisterMesh(m, directory + "\\" + m->name + ".sdmesh");
            delete m;
        }

        for (auto& [name, texture] : importedTextures)
            delete texture;

        for (auto& [name, material] : importedMaterials)
            delete material;

        importedMaterials.clear();
        importedTextures.clear();
        importedArmatures.clear();
        importedMeshes.clear();
	} 

    bool AssetImporter::ContainsMeshes(aiNode* node)
    {
        if (node->mNumMeshes > 0) return true;

        for (int i = 0; i < node->mNumChildren; i++)
            if (ContainsMeshes(node->mChildren[i])) return true;
        
        return false;
    }

    void AssetImporter::ImportAnimation(aiAnimation* animation, const std::string& directory)
    {
        Animation anim;
        anim.name = animation->mName.C_Str();
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

        std::vector<bool> foundIds;
        foundIds.resize(armature->bones.size());

        for (AnimationChannel& channel : anim.channels)
            foundIds[channel.boneId] = true;

        for (int i = 0; i < foundIds.size(); i++)
            if (!foundIds[i])
            {
                AnimationChannel c;
                c.boneId = i;
                c.boneName = "";

                c.positionKeys.push_back(PositionKey());
                c.rotationKeys.push_back(RotationKey());
                c.scalingKeys.push_back(ScalingKey());

                anim.channels.push_back(c);
            }

        std::sort(anim.channels.begin(), anim.channels.end(), [](AnimationChannel& a, AnimationChannel& b) { return a.boneId < b.boneId; });
        anim.Save(directory + "\\" + anim.name + ".sdanim");

        Application::Get()->GetResourceManager()->RegisterAnimation(&anim, directory + "\\" + anim.name + ".sdanim");
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

    void AssetImporter::ImportMeshes(aiNode* node, const aiScene* scene, const aiMatrix4x4& transform, const std::string& directory)
    {
        aiMatrix4x4 worldTransform = node->mTransformation * transform;
        glm::mat4 glmTransform = glm::make_mat4x4((float*)&(worldTransform.Transpose()));
        //importData.parents.push_back(i - 1);

        if (node->mNumMeshes > 0)
        {
            Mesh* m = new Mesh();
            m->name = node->mName.C_Str();

            std::vector<Material*> materials;
            materials.reserve(node->mNumMeshes);

            Armature* armature = nullptr;
            for (int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                
                if (mesh->HasBones())
                {
                    std::string rootBoneName = mesh->mBones[0]->mName.C_Str();
                    for (Armature& a : importedArmatures)
                        if (a.bones[0].name == rootBoneName || a.bones[1].name == rootBoneName)
                            armature = &a;
                }

                SubMesh* submesh = ProcessSubMesh(mesh, armature);

                m->subMeshes.push_back(submesh);

                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                aiString materialName;
                material->Get(AI_MATKEY_NAME, materialName);
                
                materials.push_back(importedMaterials[materialName.C_Str()]);
            }

            importedMeshes.push_back(m);
            
            Entity e = prefabMaker.CreateEntity(m->name);
            e.GetComponent<TransformComponent>().SetFromMatrix(glmTransform);

            if (!armature)
            {
                RenderComponent& r = e.AddComponent<RenderComponent>();
                r.mesh = m;
                r.materials = materials;
            }
            else
            {
                SkinnedRenderComponent& r = e.AddComponent<SkinnedRenderComponent>();
                r.mesh = m;
                r.materials = materials;
                r.armature = armature;
            }
            
            Prefab p;
            p.MakeFromEntity(e);
            p.Save("Assets\\" + m->name + ".sdpref");
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ImportMeshes(node->mChildren[i], scene, worldTransform, directory);
    }

    SubMesh* AssetImporter::ProcessSubMesh(aiMesh* mesh, Armature* armature)
    {
        SubMesh* submesh = new SubMesh();

        std::vector<Vertex> vertices;
        vertices.reserve(mesh->mNumVertices);
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

            vertices.push_back(vertex);
        }

        std::vector<unsigned int> indices;
        indices.reserve(mesh->mNumFaces);
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        submesh->Create(vertices, indices, std::string(mesh->mName.C_Str()));
        if (!armature) return submesh;

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
            
                for (int k = 0; k < Vertex::MAX_BONES_PER_VERTEX; k++)
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

    Material* AssetImporter::ImportMaterial(aiMaterial* material, const std::string& directory)
    {
        aiString name;
        material->Get(AI_MATKEY_NAME, name);
        if (importedMaterials.count(name.C_Str()) > 0) return importedMaterials[material->GetName().C_Str()];

        ResourceManager& resourceManager = *Application::Get()->GetResourceManager();

        Material* m = new Material();

        m->name = std::string(name.C_Str());
        m->shader = resourceManager.GetShader("default_shader");

        byte* ptr = m->data;

        aiColor3D tint;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, tint);
        *((glm::vec3*)ptr) = glm::vec3(tint.r, tint.g, tint.b);
        ptr += sizeof(glm::vec3);

        aiString str;
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            material->GetTexture(aiTextureType_DIFFUSE, 0, &str);

            *((Texture**)ptr) = ImportTexture(directory + "\\" + std::string(str.C_Str()), true);
            ptr += sizeof(Texture*);
        }
        else
        {
            *((Texture**)ptr) = resourceManager.GetTexture("albedo_default");
            ptr += sizeof(Texture*);
        }

        if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            material->GetTexture(aiTextureType_NORMALS, 0, &str);

            *((Texture**)ptr) = ImportTexture(directory + "\\" + std::string(str.C_Str()));
            ptr += sizeof(Texture*);
        }
        else
        {
            *((Texture**)ptr) = resourceManager.GetTexture("normal_default");
            ptr += sizeof(Texture*);
        }

        if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            material->GetTexture(aiTextureType_SHININESS, 0, &str);
            
            //*((float*)ptr) = 1.0f;
            //ptr += sizeof(float);

            *((Texture**)ptr) = ImportTexture(directory + "\\" + std::string(str.C_Str()));
            ptr += sizeof(Texture*);
        }
        else
        {
           // *((float*)ptr) = 0.0f;
           // ptr += sizeof(float);

            *((Texture**)ptr) = resourceManager.GetTexture("roughness_default");
            ptr += sizeof(Texture*);
        }


        if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            material->GetTexture(aiTextureType_SPECULAR, 0, &str);
            
            //*((float*)ptr) = 1.0f;
            //ptr += sizeof(float);

            *((Texture**)ptr) = ImportTexture(directory + "\\" + std::string(str.C_Str()));
            ptr += sizeof(Texture*);
        }
        else
        {
            //*((float*)ptr) = 0.0f;
            //ptr += sizeof(float);

            *((Texture**)ptr) = resourceManager.GetTexture("metallic_default");
            ptr += sizeof(Texture*);
        }

        if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
        {
            material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &str);
             
            *((Texture**)ptr) = ImportTexture(directory + "\\" + std::string(str.C_Str()));
            ptr += sizeof(Texture*);
        }
        else
        {
            *((Texture**)ptr) = resourceManager.GetTexture("ao_default");
            ptr += sizeof(Texture*);
        }

        m->Save(directory + "\\" + m->name + ".sdmat");

        Application::Get()->GetResourceManager()->RegisterMaterial(m, directory + "\\" + m->name + ".sdmat");

        importedMaterials[m->name] = m;
        return m;
    }

    Texture* AssetImporter::ImportTexture(const std::string& path, bool gammaCorrection)
    {
        if (importedTextures.count(path) > 0) return importedTextures[path];

        Texture* t = new Texture();
        t->Import(path, gammaCorrection);
        t->path = ChangeSuffix(path, ".sdtex");
        t->Save(t->path);

        Application::Get()->GetResourceManager()->RegisterTexture(t, t->path);

        importedTextures[path] = t;
        return t;
    }

    HdrCubemap* AssetImporter::ImportCubemap(const std::string& path)
    {
        HdrCubemap* c = new HdrCubemap();
        c->LoadFromEquirectangularMap(path);
        c->filepath = ChangeSuffix(path, ".sdhdr");
        c->Save(c->filepath);

        Application::Get()->GetResourceManager()->RegisterCubemap(c, c->filepath);

        return c;
    }
}