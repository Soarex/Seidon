#pragma once
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/HdrCubemap.h"

#include "Utils/ModelImporter.h"

#include <map>
#include <utility>

namespace Seidon
{
	struct ModelFileInfo
	{
		std::string filePath;
		std::vector<std::pair<Mesh*, std::vector<Material*>>> content;
	};

	class ResourceManager
	{
	private:
		std::map<std::string, Texture*>		textures;
		std::map<std::string, HdrCubemap*>	cubemaps;
		std::map<std::string, Material*>	materials;
		std::map<std::string, Mesh*>		meshes;
		std::map<std::string, Shader*>		shaders;

		std::map<std::string, ModelFileInfo> loadedModelFiles;
	public:
		void Init();
		void Destroy();
		
		Texture* LoadTexture(const std::string& path, bool gammaCorrection = false);
		Shader* LoadShader(const std::string& path);
		HdrCubemap* LoadCubemap(const std::string& path);
		const ModelFileInfo& LoadModelFile(const std::string& path);

		Mesh* CreateMesh(const MeshImportData& importData);
		Material* CreateMaterial(const MaterialImportData& importData);
		ModelFileInfo CreateFromImportData(const ModelImportData& importData);

		inline Texture*	GetTexture(const std::string& name) { return textures.at(name); }
		inline Mesh*	GetMesh(const std::string& name) { return meshes.at(name); }
		inline Shader*	GetShader(const std::string& name) { return shaders.at(name); }
		inline Material* GetMaterial(const std::string& name) { return materials.at(name); }
		inline HdrCubemap* GetCubemap(const std::string& name) { return cubemaps.at(name); }

		std::vector<Mesh*> GetModelFileMeshes(const std::string& name);
		std::vector<Material*> GetModelFileMaterials(const std::string& name);

		inline void AddTexture(const std::string& name, Texture* texture) { textures[name] = texture; }
		inline void AddMesh(const std::string& name, Mesh* mesh) { meshes[name] = mesh; }
		inline void AddShader(const std::string& name, Shader* shader) { shaders[name] = shader; }
		inline void AddMaterial(const std::string& name, Material* material) { materials[name] = material; }
		inline void AddCubemap(const std::string& name, HdrCubemap* cubemap) { cubemaps[name] = cubemap; }

		inline bool IsTextureLoaded(const std::string& name) { return textures.count(name) > 0; }
		inline bool IsCubemapLoaded(const std::string& name) { return cubemaps.count(name) > 0; }
		inline bool IsMeshLoaded(const std::string& name) { return meshes.count(name) > 0; }
		inline bool IsShaderLoaded(const std::string& name) { return shaders.count(name) > 0; }
		inline bool IsMaterialLoaded(const std::string& name) { return materials.count(name) > 0; }
		inline bool IsModelLoaded(const std::string& name) { return loadedModelFiles.count(name) > 0; }

		std::vector<std::string> GetTextureKeys();
		std::vector<std::string> GetCubemapKeys();
		std::vector<std::string> GetMeshKeys();
		std::vector<std::string> GetShaderKeys();
		std::vector<std::string> GetMaterialKeys();

	};
}