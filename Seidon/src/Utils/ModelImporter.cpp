#include "ModelImporter.h"

namespace Seidon
{
    ModelImportData ModelImporter::Import(const std::string& path)
	{
        std::string directory;
        ModelImportData res;

        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return res;
        }

        directory = path.substr(0, path.find_last_of('/'));

        for (int i = 0; i < scene->mNumMaterials; i++)
        {
            res.materials.push_back(ProcessMaterial(scene->mMaterials[i], directory));
        }

        ProcessNode(scene->mRootNode, scene, res, aiMatrix4x4(), directory);
        return res;
	}

    void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, ModelImportData& importData, const aiMatrix4x4& transform, const std::string& directory)
    {
        aiMatrix4x4 worldTransform = node->mTransformation * transform;
        importData.localTransforms.push_back(glm::make_mat4x4((float*)&(worldTransform.Transpose())));
        //importData.parents.push_back(i - 1);

        MeshImportData meshData;
        meshData.name = node->mName.C_Str();
        for (int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            SubMeshImportData subMesh = ProcessSubMesh(mesh);

            meshData.subMeshes.push_back(subMesh);
        }

        importData.meshes.push_back(meshData);

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, importData, worldTransform, directory);
        }
    }

    SubMeshImportData ModelImporter::ProcessSubMesh(aiMesh* mesh)
    {
        SubMeshImportData importData;

        importData.name = std::string(mesh->mName.C_Str());
        importData.materialId = mesh->mMaterialIndex;

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

        return importData;
    }

    MaterialImportData ModelImporter::ProcessMaterial(aiMaterial* material, const std::string& directory)
    {
        MaterialImportData importData;

        importData.name = std::string(material->GetName().C_Str());

        aiColor3D color;
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
            importData.tint = glm::vec3(color.r, color.g, color.b);
        else
            importData.tint = glm::vec3(1.0f);
;
        aiString str;
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
            importData.albedoMapPath = directory + "/" + std::string(str.C_Str());
        }

        if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            material->GetTexture(aiTextureType_SHININESS, 0, &str);
            importData.roughnessMapPath = directory + "/" + std::string(str.C_Str());
        }

        if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            material->GetTexture(aiTextureType_NORMALS, 0, &str);
            importData.normalMapPath = directory + "/" + std::string(str.C_Str());
        }

        if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            material->GetTexture(aiTextureType_SPECULAR, 0, &str);
            importData.metallicMapPath = directory + "/" + std::string(str.C_Str());
        }

        if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
        {
            material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &str);
            importData.aoMapPath = directory + "/" + std::string(str.C_Str());
        }
       
        return importData;
    }
}