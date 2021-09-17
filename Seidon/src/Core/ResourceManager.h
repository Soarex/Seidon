#pragma once
#include <StbImage/stb_image.h>

#include <map>
#include <utility>

#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"

#include "Utils/ModelImporter.h"

#include "WorkManager.h"

namespace Seidon
{
	class ResourceManager
	{
	private:
		static std::map<std::string, Texture*>		textures;
		static std::map<std::string, Material*>		materials;
		static std::map<std::string, Mesh*>			meshes;
		static std::map<std::string, Shader*>		shaders;

	public:
		static void Init();
		static void Destroy();
		
		static Texture* LoadTexture(const std::string& path, bool gammaCorrection = false);
		static Shader* LoadShader(const std::string& path);

		static Mesh* CreateMesh(MeshImportData& importData);
		static Material* CreateMaterial(MaterialImportData& importData);
		static std::vector<Mesh*> CreateFromImportData(ModelImportData& importData);

		static inline Texture*	GetTexture(const std::string& name) { return textures.at(name); }
		static inline Mesh*		GetMesh(const std::string& name) { return meshes.at(name); }
		static inline Shader*	GetShader(const std::string& name) { return shaders.at(name); }
		static inline Material* GetMaterial(const std::string& name) { return materials.at(name); }

		static inline void AddTexture(const std::string& name, Texture* texture) { textures[name] = texture; }
		static inline void AddMesh(const std::string& name, Mesh* mesh) { meshes[name] = mesh; }
		static inline void AddShader(const std::string& name, Shader* shader) { shaders[name] = shader; }
		static inline void AddMaterial(const std::string& name, Material* material) { materials[name] = material; }

		static std::vector<std::string> GetTextureKeys();
		static std::vector<std::string> GetMeshKeys();
		static std::vector<std::string> GetShaderKeys();
		static std::vector<std::string> GetMaterialKeys();
	};
}