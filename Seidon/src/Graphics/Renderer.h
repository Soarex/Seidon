#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "HdrCubemap.h"

#include <unordered_map>

namespace Seidon
{
	typedef unsigned char byte;

	struct RenderStats
	{
		uint32_t vertexCount;
		uint32_t vertexBufferSize;
		uint32_t indexCount;
		uint32_t indexBufferSize;
		uint32_t objectCount;
		uint32_t batchCount;
	};

	struct RenderCommand
	{
		uint32_t count;
		uint32_t instanceCount;
		uint32_t firstIndex;
		uint32_t baseVertex;
		uint32_t baseInstance;
	};

	struct CacheEntry
	{
		uint32_t indexBufferBegin;
		uint32_t indexBufferSize;

		uint32_t vertexBufferBegin;
		uint32_t vertexBufferSize;
	};

	struct MaterialData
	{
		uint32_t size = 0;
		byte data[500];
	};

	struct BatchData
	{
		uint32_t objectCount = 0;
		std::vector<RenderCommand> commands;
		std::vector<glm::mat4> transforms;
		std::vector<MaterialData> materials;
	};

	struct WireframeBatchData
	{
		uint32_t objectCount = 0;
		std::vector<RenderCommand> commands;
		std::vector<glm::mat4> transforms;
		std::vector<glm::vec4> colors;
	};

	struct DirectionalLightData
	{
		glm::vec3 direction;
		glm::vec3 color;
	};

	struct CameraData
	{
		glm::vec3 position;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
	};

	struct ShadowMappingData
	{
		Texture* shadowMaps;
		float* cascadeFarPlaneDistances;
		glm::mat4* lightSpaceMatrices;
	};

	class Renderer
	{
	private:
		static constexpr int CASCADE_COUNT = 4;
		size_t maxVertexCount;
		size_t maxObjects;

		std::unordered_map <UUID, std::vector<CacheEntry>> meshCache;
		std::unordered_map<Shader*, BatchData> batches;

		Shader* wireframeShader;
		WireframeBatchData wireframeBatch;
		BatchData spriteObjects;

		RenderStats stats;

		uint32_t vao;
		uint32_t vertexBuffer;
		uint32_t indexBuffer;
		uint32_t instanceDataBuffer;

		uint32_t storageBuffers[3];
		uint32_t materialBuffers[3];
		uint32_t indirectBuffers[3];

		uint32_t objectCount = 0;
		uint32_t nextIndexPosition = 0;
		uint32_t nextVertexPosition = 0;

		uint32_t vertexBufferHeadPosition = 0;
		uint32_t indexBufferHeadPosition = 0;

		glm::mat4* storageBufferPointers[3];
		byte* materialBufferPointers[3];
		RenderCommand* indirectBufferPointers[3];

		glm::mat4* storageBufferHead = 0;
		byte* materialBufferHead = 0;
		RenderCommand* indirectBufferHead = 0;

		int tripleBufferStage = 0;
		GLsync locks[3];
		
		HdrCubemap* ibl;
		DirectionalLightData directionalLight;
		CameraData camera;
		ShadowMappingData shadowMaps;

		double time;
	public:
		Renderer(size_t maxObjects = 1000, size_t maxVertexCount = 1000000);

		void Init();
		void Begin();

		void SubmitMesh(Mesh* mesh, std::vector<Material*>& materials, glm::mat4& transform);
		void SubmitMeshWireframe(Mesh* mesh, const glm::vec3& color, glm::mat4& transform);

		void SetCamera(const CameraData& camera);
		void SetShadowMaps(const ShadowMappingData& shadowMaps);
		void SetDirectionalLight(const DirectionalLightData& directionalLight);
		void SetIBL(HdrCubemap* cubemap);
		void SetTime(double time);

		const RenderStats& GetRenderStats() { return stats; }

		void Render();
		void End();
		void Destroy();

	private:
		void DrawMeshes(int& offset, int& materialOffset);
		void DrawWireframes(int& offset, int& materialOffset);
	};
}