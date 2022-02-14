#include "ModelImporter.h"
#include <filesystem>
#include <unordered_map>

namespace Seidon
{
    ModelImportData ModelImporter::Import(const std::string& path)
	{
        std::string directory;
        ModelImportData res;
        res.filepath = path;

        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return res;
        }

        directory = path.substr(0, path.find_last_of('\\'));

        for (int i = 0; i < scene->mNumMaterials; i++)
        {
            res.materials.push_back(ProcessMaterial(scene->mMaterials[i], directory));
        }

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
            res.armatures.push_back(armature);
        }

        for(aiNode* meshRootNode : meshRootNodes)
            ProcessMeshes(meshRootNode, scene, res, aiMatrix4x4(), directory);
       
        ProcessAnimations(scene, res);

        return res;
	} 

    bool ModelImporter::ContainsMeshes(aiNode* node)
    {
        if (node->mNumMeshes > 0) return true;

        for (int i = 0; i < node->mNumChildren; i++)
            if (ContainsMeshes(node->mChildren[i])) return true;
        
        return false;
    }

    void ModelImporter::ProcessAnimations(const aiScene* scene, ModelImportData& importData)
    {
        for (int i = 0; i < scene->mNumAnimations; i++)
        {
            Animation animation;
            animation.name = scene->mAnimations[i]->mName.C_Str();
            animation.duration = scene->mAnimations[i]->mDuration;
            animation.ticksPerSecond = scene->mAnimations[i]->mTicksPerSecond;
            animation.sceneTransform = glm::make_mat4x4((float*)&(scene->mRootNode->mTransformation.Transpose()));

            animation.channels.reserve(scene->mAnimations[i]->mNumChannels);

            for (int j = 0; j < scene->mAnimations[i]->mNumChannels; j++)
            {
                AnimationChannel channel;
                channel.boneName = scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str();

                for (Armature& armature : importData.armatures)
                    for (BoneData& bone : armature.bones)
                        if (channel.boneName == bone.name)
                            channel.boneId = bone.id;

                channel.positionKeys.reserve(scene->mAnimations[i]->mChannels[j]->mNumPositionKeys);

                for (int k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumPositionKeys; k++)
                {
                    PositionKey key;
                    key.time = scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mTime;
                    key.value.x = scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.x;
                    key.value.y = scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.y;
                    key.value.z = scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.z;

                    channel.positionKeys.push_back(key);
                }


                channel.rotationKeys.reserve(scene->mAnimations[i]->mChannels[j]->mNumRotationKeys);

                for (int k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumRotationKeys; k++)
                {
                    RotationKey key;
                    key.time = scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mTime;

                    key.value.x = scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.x;
                    key.value.y = scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.y;
                    key.value.z = scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.z;
                    key.value.w = scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.w;

                    channel.rotationKeys.push_back(key);
                }


                channel.scalingKeys.reserve(scene->mAnimations[i]->mChannels[j]->mNumScalingKeys);

                for (int k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumScalingKeys; k++)
                {
                    ScalingKey key;
                    key.time = scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mTime;
                    key.value.x = scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.x;
                    key.value.y = scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.y;
                    key.value.z = scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.z;

                    channel.scalingKeys.push_back(key);
                }

                animation.channels.push_back(channel);
            }

            std::vector<bool> foundIds;
            foundIds.resize(100);

            for (AnimationChannel& channel : animation.channels)
                foundIds[channel.boneId] = true;

            for(int i = 0; i < foundIds.size(); i++)
                if (!foundIds[i])
                {
                    AnimationChannel c;
                    c.boneId = i;
                    c.boneName = "";

                    c.positionKeys.push_back(PositionKey());
                    c.rotationKeys.push_back(RotationKey());
                    c.scalingKeys.push_back(ScalingKey());

                    animation.channels.push_back(c);
                }

            std::sort(animation.channels.begin(), animation.channels.end(), [](AnimationChannel& a, AnimationChannel& b) { return a.boneId <= b.boneId; });
            importData.animations.push_back(animation);
        }
    }

    void ModelImporter::ProcessBones(aiNode* node, const aiScene* scene, Armature& armature, int parentId)
    {
        BoneData bone;
        bone.name = node->mName.C_Str();
        
        //if (bone.name.compare(bone.name.size() - 3, 3, "End") != 0)
        //{
            bone.id = armature.bones.size();
            bone.parentId = parentId;
            bone.inverseBindPoseMatrix = glm::identity<glm::mat4>();

            armature.bones.push_back(bone);
        //}
        
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessBones(node->mChildren[i], scene, armature, bone.id);
    }

    void ModelImporter::ProcessMeshes(aiNode* node, const aiScene* scene, ModelImportData& importData, const aiMatrix4x4& transform, const std::string& directory)
    {
        aiMatrix4x4 worldTransform = node->mTransformation * transform;
        importData.localTransforms.push_back(glm::make_mat4x4((float*)&(worldTransform.Transpose())));
        //importData.parents.push_back(i - 1);

        if (node->mNumMeshes > 0)
        {
            MeshImportData meshData;
            meshData.name = node->mName.C_Str();

            for (int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

                Armature* armature = nullptr;
                if (mesh->HasBones())
                {
                    std::string rootBoneName = mesh->mBones[0]->mName.C_Str();
                    for (Armature& a : importData.armatures)
                        if (a.bones[0].name == rootBoneName || a.bones[1].name == rootBoneName)
                            armature = &a;
                }

                SubMeshImportData subMesh = ProcessSubMesh(mesh, armature);

                meshData.subMeshes.push_back(subMesh);
            }

            importData.meshes.push_back(meshData);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessMeshes(node->mChildren[i], scene, importData, worldTransform, directory);
    }

    SubMeshImportData ModelImporter::ProcessSubMesh(aiMesh* mesh, Armature* armature)
    {
        SubMeshImportData importData;

        importData.name = std::string(mesh->mName.C_Str());
        importData.materialId = mesh->mMaterialIndex;

        importData.vertices.reserve(mesh->mNumVertices);
        
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

            importData.vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                importData.indices.push_back(face.mIndices[j]);
        }

        if (!armature) return importData;

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
                    if (importData.vertices[weight.mVertexId].boneIds[k] != -1) continue;

                    importData.vertices[weight.mVertexId].weights[k] = weight.mWeight;
                    importData.vertices[weight.mVertexId].boneIds[k] = boneNameToIndex[boneName];

                    break;
                }
            }
        }


        return importData;
    }

    MaterialImportData ModelImporter::ProcessMaterial(aiMaterial* material, const std::string& directory)
    {
        MaterialImportData importData;

        importData.name = std::string(material->GetName().C_Str());

        importData.tint = glm::vec3(1.0f);
;
        aiString str;
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
            importData.albedoMapPath = directory + "\\" + std::string(str.C_Str());
        }

        if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            material->GetTexture(aiTextureType_SHININESS, 0, &str);
            importData.roughnessMapPath = directory + "\\" + std::string(str.C_Str());
        }

        if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            material->GetTexture(aiTextureType_NORMALS, 0, &str);
            importData.normalMapPath = directory + "\\" + std::string(str.C_Str());
        }

        if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            material->GetTexture(aiTextureType_SPECULAR, 0, &str);
            importData.metallicMapPath = directory + "\\" + std::string(str.C_Str());
        }

        if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
        {
            material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &str);
            importData.aoMapPath = directory + "\\" + std::string(str.C_Str());
        }
       
        return importData;
    }
}