#pragma once
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Animation/Animation.h"
#include "Graphics/Armature.h"
#include "Graphics/HdrCubemap.h"
#include "Graphics/Font.h"

#include <unordered_map>
#include <utility>

namespace Seidon
{
	class ResourceManager
	{
	public:
		void Init();
		void Destroy();

		void Save(std::ofstream& out);
		void Load(std::ifstream& in);
		
		Shader*			LoadShader(const std::string& path, UUID id = UUID());
		Mesh*			LoadMesh(const std::string& path);
		SkinnedMesh*	LoadSkinnedMesh(const std::string& path);
		Material*		LoadMaterial(const std::string& path);
		Texture*		LoadTexture(const std::string& path);
		HdrCubemap*		LoadCubemap(const std::string& path);
		Animation*		LoadAnimation(const std::string& path);
		Font*			LoadFont(const std::string& path);

		Shader*			LoadShader(UUID id);
		Mesh*			LoadMesh(UUID id);
		SkinnedMesh*	LoadSkinnedMesh(UUID id);
		Material*		LoadMaterial(UUID id);
		Texture*		LoadTexture(UUID id);
		HdrCubemap*		LoadCubemap(UUID id);
		Animation*		LoadAnimation(UUID id);
		Font*			LoadFont(UUID id);

		void RegisterShader(Shader* shader, const std::string& path);
		void RegisterMesh(Mesh* mesh, const std::string& path);
		void RegisterSkinnedMesh(SkinnedMesh* mesh, const std::string& path);
		void RegisterMaterial(Material* material, const std::string& path);
		void RegisterTexture(Texture* texture, const std::string& path);
		void RegisterCubemap(HdrCubemap* cubemap, const std::string& path);
		void RegisterAnimation(Animation* animation, const std::string& path);
		void RegisterFont(Font* font, const std::string& path);

		bool IsShaderRegistered(UUID id) { return idToShaderPath.count(id); }
		bool IsMeshRegistered(UUID id) { return idToMeshPath.count(id); }
		bool IsSkinnedMeshRegistered(UUID id) { return idToSkinnedMeshPath.count(id); }
		bool IsMaterialRegistered(UUID id) { return idToMaterialPath.count(id); }
		bool IsTextureRegistered(UUID id) { return idToTexturePath.count(id); }
		bool IsCubemapRegistered(UUID id) { return idToCubemapPath.count(id); }
		bool IsAnimationRegistered(UUID id) { return idToAnimationPath.count(id); }
		bool IsFontRegistered(UUID id) { return idToFontPath.count(id); }

		inline Texture*		GetTexture(const std::string& name)		{ return textures[nameToTextureId.at(name)];			}
		inline Mesh*		GetMesh(const std::string& name)		{ return meshes[nameToMeshId.at(name)];					}
		inline SkinnedMesh* GetSkinnedMesh(const std::string& name) { return skinnedMeshes[nameToSkinnedMeshId.at(name)];	}
		inline Shader*		GetShader(const std::string& name)		{ return shaders[nameToShaderId.at(name)];				}
		inline Material*	GetMaterial(const std::string& name)	{ return materials[nameToMaterialId.at(name)];			}
		inline HdrCubemap*	GetCubemap(const std::string& name)		{ return cubemaps[nameToCubemapId.at(name)];			}
		inline Animation*	GetAnimation(const std::string& name)	{ return animations[nameToAnimationId.at(name)];		}
		inline Font*		GetFont(const std::string& name)		{ return fonts[nameToFontId.at(name)];					}

		inline Texture*		GetTexture(UUID id)		{ return textures[id]; }
		inline Mesh*		GetMesh(UUID id)		{ return meshes[id]; }
		inline SkinnedMesh* GetSkinnedMesh(UUID id) { return skinnedMeshes[id]; }
		inline Shader*		GetShader(UUID id)		{ return shaders[id]; }
		inline Material*	GetMaterial(UUID id)	{ return materials[id]; }
		inline HdrCubemap*	GetCubemap(UUID id)		{ return cubemaps[id]; }
		inline Animation*	GetAnimation(UUID id)	{ return animations[id]; }
		inline Font*		GetFont(UUID id)		{ return fonts[id]; }

		inline std::string GetTexturePath(UUID id)		{ return idToTexturePath[id]; }
		inline std::string GetMeshPath(UUID id)			{ return idToMeshPath[id]; }
		inline std::string GetSkinnedMeshPath(UUID id)  { return idToSkinnedMeshPath[id]; }
		inline std::string GetShaderPath(UUID id)		{ return idToShaderPath[id]; }
		inline std::string GetMaterialPath(UUID id)		{ return idToMaterialPath[id]; }
		inline std::string GetCubemapPath(UUID id)		{ return idToCubemapPath[id]; }
		inline std::string GetAnimationPath(UUID id)	{ return idToAnimationPath[id]; }
		inline std::string GetFontPath(UUID id)			{ return idToFontPath[id]; }

		Texture*		GetOrLoadTexture(const std::string& name);
		Mesh*			GetOrLoadMesh(const std::string& name);
		SkinnedMesh*	GetOrLoadSkinnedMesh(const std::string& name);
		Shader*			GetOrLoadShader(const std::string& name);
		Material*		GetOrLoadMaterial(const std::string& name);
		HdrCubemap*		GetOrLoadCubemap(const std::string& name);
		Animation*		GetOrLoadAnimation(const std::string& name);
		Font*			GetOrLoadFont(const std::string& name);

		Texture*		GetOrLoadTexture(UUID id);
		Mesh*			GetOrLoadMesh(UUID id);
		SkinnedMesh*	GetOrLoadSkinnedMesh(UUID id);
		Shader*			GetOrLoadShader(UUID id);
		Material*		GetOrLoadMaterial(UUID id);
		HdrCubemap*		GetOrLoadCubemap(UUID id);
		Animation*		GetOrLoadAnimation(UUID id);
		Font*			GetOrLoadFont(UUID id);

		std::vector<Texture*>		GetTextures();
		std::vector<Mesh*>			GetMeshes();
		std::vector<SkinnedMesh*>	GetSkinnedMeshes();
		std::vector<Shader*>		GetShaders();
		std::vector<Material*>		GetMaterials();
		std::vector<HdrCubemap*>	GetCubemaps();
		std::vector<Animation*>		GetAnimations();
		std::vector<Font*>			GetFonts();

		void AddTexture(const std::string& name, Texture* texture);
		void AddMesh(const std::string& name, Mesh* mesh);
		void AddSkinnedMesh(const std::string& name, SkinnedMesh* mesh);
		void AddShader(const std::string& name, Shader* shader);
		void AddMaterial(const std::string& name, Material* material);
		void AddCubemap(const std::string& name, HdrCubemap* cubemap);
		void AddAnimation(const std::string& name, Animation* animation);
		void AddFont(const std::string& name, Font* font);

		inline bool IsTextureLoaded(const std::string& name) { return nameToTextureId.count(name) > 0; }
		inline bool IsCubemapLoaded(const std::string& name) { return nameToCubemapId.count(name) > 0; }
		inline bool IsMeshLoaded(const std::string& name) { return nameToMeshId.count(name) > 0; }
		inline bool IsSkinnedMeshLoaded(const std::string& name) { return nameToSkinnedMeshId.count(name) > 0; }
		inline bool IsShaderLoaded(const std::string& name) { return nameToShaderId.count(name) > 0; }
		inline bool IsMaterialLoaded(const std::string& name) { return nameToMaterialId.count(name) > 0; }
		inline bool IsAnimationLoaded(const std::string& name) { return nameToAnimationId.count(name) > 0; }
		inline bool IsFontLoaded(const std::string& name) { return nameToFontId.count(name) > 0; }

	private:
		std::unordered_map<UUID, Texture*>		textures;
		std::unordered_map<UUID, HdrCubemap*>	cubemaps;
		std::unordered_map<UUID, Material*>		materials;
		std::unordered_map<UUID, Mesh*>			meshes;
		std::unordered_map<UUID, SkinnedMesh*>	skinnedMeshes;
		std::unordered_map<UUID, Shader*>		shaders;
		std::unordered_map<UUID, Animation*>	animations;
		std::unordered_map<UUID, Font*>			fonts;

		std::unordered_map<std::string, UUID> nameToTextureId;
		std::unordered_map<std::string, UUID> nameToCubemapId;
		std::unordered_map<std::string, UUID> nameToMaterialId;
		std::unordered_map<std::string, UUID> nameToMeshId;
		std::unordered_map<std::string, UUID> nameToSkinnedMeshId;
		std::unordered_map<std::string, UUID> nameToShaderId;
		std::unordered_map<std::string, UUID> nameToAnimationId;
		std::unordered_map<std::string, UUID> nameToFontId;

		std::unordered_map<UUID, std::string> idToTexturePath;
		std::unordered_map<UUID, std::string> idToCubemapPath;
		std::unordered_map<UUID, std::string> idToMaterialPath;
		std::unordered_map<UUID, std::string> idToMeshPath;
		std::unordered_map<UUID, std::string> idToSkinnedMeshPath;
		std::unordered_map<UUID, std::string> idToShaderPath;
		std::unordered_map<UUID, std::string> idToAnimationPath;
		std::unordered_map<UUID, std::string> idToFontPath;
	};
}