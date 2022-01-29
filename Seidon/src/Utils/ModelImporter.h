#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Graphics/Mesh.h"
#include "../Graphics/Armature.h"
#include "../Animation/Animation.h"

#include <vector>
#include <iostream>

namespace Seidon
{
	struct MaterialImportData
	{
		std::string name;
		glm::vec3 tint;
		std::string albedoMapPath;
		std::string roughnessMapPath;
		std::string normalMapPath;
		std::string metallicMapPath;
		std::string aoMapPath;
	};

	struct SubMeshImportData
	{
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		int materialId;
	};

	struct MeshImportData
	{
		std::string name;
		std::vector<SubMeshImportData> subMeshes;
	};

	struct ModelImportData
	{
		std::string filepath;
		std::vector<MeshImportData> meshes;
		std::vector<MaterialImportData> materials;
		std::vector<glm::mat4> localTransforms;
		std::vector<Armature> armatures;
		std::vector<Animation> animations;
		std::vector<int> parents;
	};

	class ModelImporter
	{
	private:
		Assimp::Importer importer;

	public:
		ModelImportData Import(const std::string& path);

	private:
		bool ContainsMeshes(aiNode* node);

		void ProcessMeshes(aiNode* node, const aiScene* scene, ModelImportData& importData, const aiMatrix4x4& transform, const std::string& directory);
		void ProcessBones(aiNode* node, const aiScene* scene, Armature& importData, int parentId);
		SubMeshImportData ProcessSubMesh(aiMesh* mesh, Armature* armature);
		MaterialImportData ProcessMaterial(aiMaterial* material, const std::string& directory);
	};
}