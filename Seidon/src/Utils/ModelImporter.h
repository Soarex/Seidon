#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include "Graphics/Mesh.h"

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
		std::vector<MeshImportData> meshes;
		std::vector<MaterialImportData> materials;
		std::vector<glm::mat4> localTransforms;
		std::vector<int> parents;
	};

	class ModelImporter
	{
	private:
		Assimp::Importer importer;

	public:
		ModelImportData Import(const std::string& path);

	private:
		void ProcessNode(aiNode* node, const aiScene* scene, ModelImportData& importData, const aiMatrix4x4& transform, const std::string& directory);
		SubMeshImportData ProcessSubMesh(aiMesh* mesh);
		MaterialImportData ProcessMaterial(aiMaterial* material, const std::string& directory);
	};
}