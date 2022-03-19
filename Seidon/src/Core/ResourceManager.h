#pragma once
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Animation/Animation.h"
#include "Graphics/Armature.h"
#include "Graphics/HdrCubemap.h"

#include <unordered_map>
#include <utility>

namespace Seidon
{
	class ResourceManager
	{
	private:
		std::unordered_map<UUID, Texture*>		textures;
		std::unordered_map<UUID, HdrCubemap*>	cubemaps;
		std::unordered_map<UUID, Material*>		materials;
		std::unordered_map<UUID, Mesh*>			meshes;
		std::unordered_map<UUID, Shader*>		shaders;
		std::unordered_map<UUID, Animation*>	animations;
		std::unordered_map<UUID, Armature*>		armatures;

		std::unordered_map<std::string, UUID> nameToTextureId;
		std::unordered_map<std::string, UUID> nameToCubemapId;
		std::unordered_map<std::string, UUID> nameToMaterialId;
		std::unordered_map<std::string, UUID> nameToMeshId;
		std::unordered_map<std::string, UUID> nameToShaderId;
		std::unordered_map<std::string, UUID> nameToAnimationId;
		std::unordered_map<std::string, UUID> nameToArmatureId;

		std::unordered_map<UUID, std::string> idToTexturePath;
		std::unordered_map<UUID, std::string> idToCubemapPath;
		std::unordered_map<UUID, std::string> idToMaterialPath;
		std::unordered_map<UUID, std::string> idToMeshPath;
		std::unordered_map<UUID, std::string> idToShaderPath;
		std::unordered_map<UUID, std::string> idToAnimationPath;
		std::unordered_map<UUID, std::string> idToArmaturePath;
	public:
		void Init();
		void Destroy();

		void SaveText(const std::string& path);
		void LoadText(const std::string& path);
		
		Shader* LoadShader(const std::string& path, UUID id = UUID());
		Mesh* LoadMesh(const std::string& path);
		Material* LoadMaterial(const std::string& path);
		Texture* LoadTexture(const std::string& path);
		HdrCubemap* LoadCubemap(const std::string& path);
		Armature* LoadArmature(const std::string& path);
		Animation* LoadAnimation(const std::string& path);

		Shader*		LoadShader(UUID id);
		Mesh*		LoadMesh(UUID id);
		Material*	LoadMaterial(UUID id);
		Texture*	LoadTexture(UUID id);
		HdrCubemap* LoadCubemap(UUID id);
		Armature*	LoadArmature(UUID id);
		Animation*	LoadAnimation(UUID id);

		void RegisterShader(Shader* shader, const std::string& path);
		void RegisterMesh(Mesh* mesh, const std::string& path);
		void RegisterMaterial(Material* material, const std::string& path);
		void RegisterTexture(Texture* texture, const std::string& path);
		void RegisterCubemap(HdrCubemap* cubemap, const std::string& path);
		void RegisterArmature(Armature* armature, const std::string& path);
		void RegisterAnimation(Animation* animation, const std::string& path);

		inline Texture*		GetTexture(const std::string& name)		{ return textures[nameToTextureId.at(name)];	}
		inline Mesh*		GetMesh(const std::string& name)		{ return meshes[nameToMeshId.at(name)];			}
		inline Shader*		GetShader(const std::string& name)		{ return shaders[nameToShaderId.at(name)];		}
		inline Material*	GetMaterial(const std::string& name)	{ return materials[nameToMaterialId.at(name)];	}
		inline HdrCubemap*	GetCubemap(const std::string& name)		{ return cubemaps[nameToCubemapId.at(name)];	}
		inline Armature*	GetArmature(const std::string& name)	{ return armatures[nameToArmatureId.at(name)]; }
		inline Animation*	GetAnimation(const std::string& name)	{ return animations[nameToAnimationId.at(name)]; }

		inline Texture*		GetTexture(UUID id) { return textures[id]; }
		inline Mesh*		GetMesh(UUID id) { return meshes[id]; }
		inline Shader*		GetShader(UUID id) { return shaders[id]; }
		inline Material*	GetMaterial(UUID id) { return materials[id]; }
		inline HdrCubemap*	GetCubemap(UUID id) { return cubemaps[id]; }
		inline Armature*	GetArmature(UUID id) { return armatures[id]; }
		inline Animation*	GetAnimation(UUID id) { return animations[id]; }

		Texture*	GetOrLoadTexture(const std::string& name);
		Mesh*		GetOrLoadMesh(const std::string& name);
		Shader*		GetOrLoadShader(const std::string& name);
		Material*	GetOrLoadMaterial(const std::string& name);
		HdrCubemap* GetOrLoadCubemap(const std::string& name);
		Armature*	GetOrLoadArmature(const std::string& name);
		Animation*	GetOrLoadAnimation(const std::string& name);

		Texture*	GetOrLoadTexture(UUID id);
		Mesh*		GetOrLoadMesh(UUID id);
		Shader*		GetOrLoadShader(UUID id);
		Material*	GetOrLoadMaterial(UUID id);
		HdrCubemap*	GetOrLoadCubemap(UUID id);
		Armature*	GetOrLoadArmature(UUID id);
		Animation*	GetOrLoadAnimation(UUID id);

		std::vector<Texture*>		GetTextures();
		std::vector<Mesh*>			GetMeshes();
		std::vector<Shader*>		GetShaders();
		std::vector<Material*>		GetMaterials();
		std::vector<HdrCubemap*>	GetCubemaps();
		std::vector<Armature*>		GetArmatures();
		std::vector<Animation*>		GetAnimations();

		void AddTexture(const std::string& name, Texture* texture);
		void AddMesh(const std::string& name, Mesh* mesh);
		void AddShader(const std::string& name, Shader* shader);
		void AddMaterial(const std::string& name, Material* material);
		void AddCubemap(const std::string& name, HdrCubemap* cubemap);
		void AddAnimation(const std::string& name, Animation* animation);
		void AddArmature(const std::string& name, Armature* armature);

		inline bool IsTextureLoaded(const std::string& name) { return nameToTextureId.count(name) > 0; }
		inline bool IsCubemapLoaded(const std::string& name) { return nameToCubemapId.count(name) > 0; }
		inline bool IsMeshLoaded(const std::string& name) { return nameToMeshId.count(name) > 0; }
		inline bool IsShaderLoaded(const std::string& name) { return nameToShaderId.count(name) > 0; }
		inline bool IsMaterialLoaded(const std::string& name) { return nameToMaterialId.count(name) > 0; }
	};
}