#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Ecs/Scene.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "../Graphics/HdrCubemap.h"
#include "../Graphics/Armature.h"
#include "../Graphics/Font.h"
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

		Scene prefabScene;
		std::unordered_map<std::string, Material*> importedMaterials;
		std::unordered_map<std::string, Texture*> importedTextures;
		std::vector<Armature> importedArmatures;
		std::vector<Mesh*> importedMeshes;
		std::vector<SkinnedMesh*> importedSkinnedMeshes;
	public:
		void ImportModelFile(const std::string& path);
		Texture* ImportTexture(const std::string& path, bool gammaCorrection = false, bool flip = true, ClampingMode clampingMode = ClampingMode::REPEAT);
		HdrCubemap* ImportCubemap(const std::string& path);
		Font* ImportFont(const std::string& path);

	private:
		bool ContainsMeshes(aiNode* node);

		void ImportAnimation(aiAnimation* animation, const std::string& directory);
		Mesh* ImportMesh(aiNode* node, const aiScene* scene, std::vector<Material*>& materials);
		SkinnedMesh* ImportSkinnedMesh(aiNode* node, const aiScene* scene, std::vector<Material*>& materials);
		void ProcessBones(aiNode* node, const aiScene* scene, Armature& armature, int parentId);
		Submesh* ProcessSubmesh(aiMesh* mesh);
		SkinnedSubmesh* ProcessSkinnedSubmesh(aiMesh* mesh, Armature* armature);
		Material* ImportMaterial(aiMaterial* material, const std::string& directory);

		Entity ImportHierarchy(aiNode* node, const aiScene* scene, const std::string& directory);
		void ImportChildNodes(aiNode* node, const aiScene* scene, Entity parent, const std::string& directory);
	};
}