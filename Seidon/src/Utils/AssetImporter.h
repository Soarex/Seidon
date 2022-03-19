#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Graphics/Mesh.h"
#include "../Graphics/HdrCubemap.h"
#include "../Graphics/Armature.h"
#include "../Animation/Animation.h"

#include <vector>
#include <unordered_map>
#include <iostream>

namespace Seidon
{
	class AssetImporter
	{
	private:
		Assimp::Importer importer;
		std::unordered_map<std::string, Material*> importedMaterials;
		std::unordered_map<std::string, Texture*> importedTextures;
		std::vector<Armature> importedArmatures;
		std::vector<Mesh*> importedMeshes;
	public:
		void ImportModelFile(const std::string& path);
		Texture* ImportTexture(const std::string& path, bool gammaCorrection = false);
		HdrCubemap* ImportCubemap(const std::string& path);

	private:
		bool ContainsMeshes(aiNode* node);

		void ImportAnimation(aiAnimation* animation, const std::string& directory);
		void ImportMeshes(aiNode* node, const aiScene* scene, const aiMatrix4x4& transform, const std::string& directory);
		void ProcessBones(aiNode* node, const aiScene* scene, Armature& armature, int parentId);
		SubMesh* ProcessSubMesh(aiMesh* mesh, Armature* armature);
		Material* ImportMaterial(aiMaterial* material, const std::string& directory);
	};
}