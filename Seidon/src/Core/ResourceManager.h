#pragma once
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"

#include "Utils/ModelImporter.h"

#include <map>
#include <utility>

namespace Seidon
{
	class ResourceManager
	{
	private:
		std::map<std::string, Texture*>		textures;
		std::map<std::string, Material*>		materials;
		std::map<std::string, Mesh*>			meshes;
		std::map<std::string, Shader*>		shaders;

	public:
		void Init();
		void Destroy();
		
		Texture* LoadTexture(const std::string& path, bool gammaCorrection = false);
		Shader* LoadShader(const std::string& path);

		Mesh* CreateMesh(MeshImportData& importData);
		Material* CreateMaterial(MaterialImportData& importData);
		std::vector<Mesh*> CreateFromImportData(ModelImportData& importData);

		inline Texture*	GetTexture(const std::string& name) { return textures.at(name); }
		inline Mesh*		GetMesh(const std::string& name) { return meshes.at(name); }
		inline Shader*	GetShader(const std::string& name) { return shaders.at(name); }
		inline Material* GetMaterial(const std::string& name) { return materials.at(name); }

		inline void AddTexture(const std::string& name, Texture* texture) { textures[name] = texture; }
		inline void AddMesh(const std::string& name, Mesh* mesh) { meshes[name] = mesh; }
		inline void AddShader(const std::string& name, Shader* shader) { shaders[name] = shader; }
		inline void AddMaterial(const std::string& name, Material* material) { materials[name] = material; }

		inline bool IsTextureLoaded(const std::string& name) { return textures.count(name) > 0; }
		inline bool IsMeshLoaded(const std::string& name) { return meshes.count(name) > 0; }
		inline bool IsShaderLoaded(const std::string& name) { return shaders.count(name) > 0; }
		inline bool IsMaterialLoaded(const std::string& name) { return materials.count(name) > 0; }


		std::vector<std::string> GetTextureKeys();
		std::vector<std::string> GetMeshKeys();
		std::vector<std::string> GetShaderKeys();
		std::vector<std::string> GetMaterialKeys();
	};
}