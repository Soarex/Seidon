#pragma once
#include <StbImage/stb_image.h>
#include <glad/glad.h>

#include <string>
#include <iostream>
#include <vector>

#include "Texture.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "Mesh.h"

namespace Seidon
{
	class HdrCubemap
	{
	private:
		UUID id;
		std::string filepath;
		unsigned int skyboxID;
		unsigned int irradianceMapID;
		unsigned int prefilteredMapID;
		
		Texture BRDFLookupMap;

		unsigned int faceSize;
		unsigned int irradianceMapSize;
		unsigned int prefilteredMapSize;
		unsigned int BRDFLookupSize;
	public:
		HdrCubemap(unsigned int faceSize = 512, unsigned int irradianceMapSize = 32, unsigned int prefilteredMapSize = 128, unsigned int BRDFLookupSize = 512);
		void CreateFromEquirectangularMap(Texture* texture);
		void LoadFromEquirectangularMap(std::string path);
		void BindSkybox(unsigned int slot = 0);
		void BindIrradianceMap(unsigned int slot = 0);
		void BindPrefilteredMap(unsigned int slot = 0);
		void BindBRDFLookupMap(unsigned int slot = 0);

		inline const std::string& GetPath() { return filepath; }
		inline UUID GetId() { return id; }
		inline unsigned int GetSkyboxID() { return skyboxID; }
	private:
		void ToCubemap(Texture& equirectangularMap);
		void GenerateIrradianceMap();
		void GeneratePrefilteredMap();
		void GenerateBRDFLookupMap();
		void DrawCaptureCube();
		void DrawCaptureQuad();
	};
}