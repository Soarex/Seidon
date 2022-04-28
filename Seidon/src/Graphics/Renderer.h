#pragma once
#include "Mesh.h"
#include "Shader.h"

#include <unordered_map>

namespace Seidon
{
	typedef unsigned char byte;

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

	class Renderer
	{
	private:
		static constexpr int VERTEX_COUNT = 10000000;
		static constexpr int MAX_OBJECTS = 1000;

		std::unordered_map <UUID, std::vector<CacheEntry>> meshCache;

		std::unordered_map<Shader*, BatchData> batches;

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
		
		glm::vec3 cameraPosition;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		double time;
	public:
		void Init();
		void Begin();
		void SubmitMesh(Mesh* mesh, std::vector<Material*>& materials, glm::mat4& transform);
		void SetCameraPosition(const glm::vec3& cameraPosition);
		void SetViewMatrix(const glm::mat4& viewMatrix);
		void SetProjectionMatrix(const glm::mat4& projectionMatrix);
		void SetTime(double time);

		void Render();
		void End();
		void Destroy();
	};
}