#pragma once
#include "../Core/UUID.h"
#include "../Core/Asset.h"

#include "Texture.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "Mesh.h"

#include <StbImage/stb_image.h>
#include <glad/glad.h>

#include <string>
#include <iostream>
#include <vector>

namespace Seidon
{
	class Material;

	class HdrCubemap : public Asset
	{
	public:
		HdrCubemap(UUID id, unsigned int faceSize = 512, unsigned int irradianceMapSize = 32, unsigned int prefilteredMapSize = 128, unsigned int BRDFLookupSize = 512);
		HdrCubemap(unsigned int faceSize = 512, unsigned int irradianceMapSize = 32, unsigned int prefilteredMapSize = 128, unsigned int BRDFLookupSize = 512);
		~HdrCubemap();

		void Destroy();


		using Asset::Save;
		using Asset::Load;

		void Save(std::ofstream& out);
		void Load(std::ifstream& in);
		
		void CreateFromEquirectangularMap(Texture* texture);
		void CreateFromMaterial(Material* material);
		void LoadFromEquirectangularMap(std::string path);
		
		void BindSkybox(unsigned int slot = 0);
		void BindIrradianceMap(unsigned int slot = 0);
		void BindPrefilteredMap(unsigned int slot = 0);
		void BindBRDFLookupMap(unsigned int slot = 0);

		inline const std::string& GetPath() { return name; }
		inline UUID GetId() { return id; }
		inline unsigned int GetSkyboxID() { return skyboxID; }

	private:
		bool initialized = false;

		unsigned int skyboxID;
		unsigned int irradianceMapID;
		unsigned int prefilteredMapID;

		Texture* BRDFLookupMap;

		unsigned int faceSize;
		unsigned int irradianceMapSize;
		unsigned int prefilteredMapSize;
		unsigned int BRDFLookupSize;

		static constexpr unsigned int maxMipLevels = 5;

	private:
		void SaveCubemap(std::ofstream& out);
		void LoadCubemap(std::ifstream& in);

		void SaveIrradianceMap(std::ofstream& out);
		void LoadIrradianceMap(std::ifstream& in);

		void SavePrefilteredMap(std::ofstream& out);
		void LoadPrefilteredMap(std::ifstream& in);

		void ToCubemap(Texture& equirectangularMap);
		void GenerateIrradianceMap();
		void GeneratePrefilteredMap();
		void GenerateBRDFLookupMap();
		void DrawCaptureCube();
		void DrawCaptureQuad();
		int MaterialToShaderData(Material* material, void* shaderData);

		friend class ResourceManager;
		friend class AssetImporter;
	};
}