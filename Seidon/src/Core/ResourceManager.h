#pragma once
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/HdrCubemap.h"

#include "Utils/ModelImporter.h"

#include <yaml-cpp/yaml.h>

#include <unordered_map>
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
		std::unordered_map<UUID, Texture*>		textures;
		std::unordered_map<UUID, HdrCubemap*>	cubemaps;
		std::unordered_map<UUID, Material*>		materials;
		std::unordered_map<UUID, Mesh*>			meshes;
		std::unordered_map<UUID, Shader*>		shaders;

		std::unordered_map<std::string, UUID> nameToTextureId;
		std::unordered_map<std::string, UUID> nameToCubemapId;
		std::unordered_map<std::string, UUID> nameToMaterialId;
		std::unordered_map<std::string, UUID> nameToMeshId;
		std::unordered_map<std::string, UUID> nameToShaderId;

		std::unordered_map<UUID, std::string> idToTexturePath;
		std::unordered_map<UUID, std::string> idToCubemapPath;
		std::unordered_map<UUID, std::string> idToMaterialPath;
		std::unordered_map<UUID, std::string> idToMeshPath;
		std::unordered_map<UUID, std::string> idToShaderPath;
	public:
		void Init();
		void Destroy();

		void SaveText(YAML::Emitter& out);
		void LoadText(YAML::Node& node);
		
		Shader* LoadShader(const std::string& path, UUID id = UUID());
		Mesh* LoadMesh(const std::string& path);
		Material* LoadMaterial(const std::string& path);
		Texture* LoadTexture(const std::string& path);
		HdrCubemap* LoadCubemap(const std::string& path);

		Shader* LoadShader(UUID id);
		Mesh* LoadMesh(UUID id);
		Material* LoadMaterial(UUID id);
		Texture* LoadTexture(UUID id);
		HdrCubemap* LoadCubemap(UUID id);
		
		Texture* ImportTexture(const std::string& path, bool gammaCorrection = false, UUID id = UUID());
		HdrCubemap* ImportCubemap(const std::string& path, UUID id = UUID());
		const ModelFileInfo& ImportModelFile(const std::string& path);

		Texture* GetOrImportTexture(const std::string& path, bool gammaCorrection = false, UUID id = UUID());
		HdrCubemap* GetOrImportCubemap(const std::string& path, UUID id = UUID());

		Mesh* CreateMesh(const MeshImportData& importData, UUID id = UUID());
		Material* CreateMaterial(const MaterialImportData& importData, UUID id = UUID());
		ModelFileInfo CreateFromImportData(const ModelImportData& importData);

		inline Texture*		GetTexture(const std::string& name)		{ return textures[nameToTextureId.at(name)];	}
		inline Mesh*		GetMesh(const std::string& name)		{ return meshes[nameToMeshId.at(name)];			}
		inline Shader*		GetShader(const std::string& name)		{ return shaders[nameToShaderId.at(name)];		}
		inline Material*	GetMaterial(const std::string& name)	{ return materials[nameToMaterialId.at(name)];	}
		inline HdrCubemap*	GetCubemap(const std::string& name)		{ return cubemaps[nameToCubemapId.at(name)];	}

		inline Texture*		GetTexture(UUID id) { return textures[id]; }
		inline Mesh*		GetMesh(UUID id) { return meshes[id]; }
		inline Shader*		GetShader(UUID id) { return shaders[id]; }
		inline Material*	GetMaterial(UUID id) { return materials[id]; }
		inline HdrCubemap*	GetCubemap(UUID id) { return cubemaps[id]; }

		Texture*	GetOrLoadTexture(const std::string& name);
		Mesh*		GetOrLoadMesh(const std::string& name);
		Shader*		GetOrLoadShader(const std::string& name);
		Material*	GetOrLoadMaterial(const std::string& name);
		HdrCubemap* GetOrLoadCubemap(const std::string& name);

		Texture*	GetOrLoadTexture(UUID id);
		Mesh*		GetOrLoadMesh(UUID id);
		Shader*		GetOrLoadShader(UUID id);
		Material*	GetOrLoadMaterial(UUID id);
		HdrCubemap*	GetOrLoadCubemap(UUID id);

		std::vector<Texture*>		GetTextures();
		std::vector<Mesh*>			GetMeshes();
		std::vector<Shader*>		GetShaders();
		std::vector<Material*>		GetMaterials();
		std::vector<HdrCubemap*>	GetCubemaps();

		inline void AddTexture(const std::string& name, Texture* texture) { textures[texture->GetId()] = texture; nameToTextureId[name] = texture->GetId(); }
		inline void AddMesh(const std::string& name, Mesh* mesh) { meshes[mesh->id] = mesh; nameToMeshId[name] = mesh->id; }
		inline void AddShader(const std::string& name, Shader* shader) { shaders[shader->GetId()] = shader; nameToShaderId[name] = shader->GetId(); }
		inline void AddMaterial(const std::string& name, Material* material) { materials[material->id] = material; nameToMaterialId[name] = material->id; }
		inline void AddCubemap(const std::string& name, HdrCubemap* cubemap) { cubemaps[cubemap->GetId()] = cubemap; nameToCubemapId[name] = cubemap->GetId(); }

		inline bool IsTextureLoaded(const std::string& name) { return nameToTextureId.count(name) > 0; }
		inline bool IsCubemapLoaded(const std::string& name) { return nameToCubemapId.count(name) > 0; }
		inline bool IsMeshLoaded(const std::string& name) { return nameToMeshId.count(name) > 0; }
		inline bool IsShaderLoaded(const std::string& name) { return nameToShaderId.count(name) > 0; }
		inline bool IsMaterialLoaded(const std::string& name) { return nameToMaterialId.count(name) > 0; }
	};
}