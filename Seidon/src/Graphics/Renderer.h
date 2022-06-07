#pragma once
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "HdrCubemap.h"

#include "../Ecs/EnttWrappers.h"

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

		uint32_t skinnedVertexCount;
		uint32_t skinnedVertexBufferSize;
		uint32_t skinnedIndexCount;
		uint32_t skinnedIndexBufferSize;

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
		Shader* shader;
		uint32_t size = 0;
		byte data[500];
	};

	struct BatchData
	{
		uint32_t objectCount = 0;
		std::vector<RenderCommand> commands;
		std::vector<glm::mat4> transforms;
		std::vector<int> entityIds;
		std::vector<MaterialData> materials;
	};

	struct SkinnedMeshBatch
	{
		uint32_t objectCount = 0;

		std::vector<RenderCommand> commands;
		std::vector<glm::mat4> transforms;
		std::vector<MaterialData> materials;
		std::vector<int> entityIds;

		std::vector<glm::mat4>* bones;
	};

	struct WireframeBatchData
	{
		uint32_t objectCount = 0;
		std::vector<RenderCommand> commands;
		std::vector<glm::mat4> transforms;
		std::vector<int> entityIds;
		std::vector<glm::vec4> colors;
	};

	struct SpriteMaterialData
	{
		glm::vec4 tint;
		uint64_t textureHandle;
		uint64_t padding;
	};

	struct SpriteBatchData
	{
		uint32_t objectCount = 0;
		RenderCommand command;
		std::vector<SpriteMaterialData> sprites;
		std::vector<glm::mat4> transforms;
		std::vector<int> entityIds;
	};

	struct TextVertexData
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 color;
		glm::vec4 shadowColorAndDistance;
		uint64_t atlasHandle;
		EntityId owningEntityId;
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
	public:
		Renderer(size_t maxObjects = 1000, size_t maxVertexCount = 1000000, size_t maxSkinnedVertexCount = 100000, size_t maxTextCharacterCount = 10000);

		void Init();
		void Begin();

		void SubmitMesh(Mesh* mesh, std::vector<Material*>& materials, const glm::mat4& transform, EntityId owningEntityId = NullEntityId);
		void SubmitSkinnedMesh(SkinnedMesh* mesh, std::vector<glm::mat4>& bones, std::vector<Material*>& materials, const glm::mat4& transform, EntityId owningEntityId = NullEntityId);
		void SubmitMeshWireframe(Mesh* mesh, const glm::vec3& color, const glm::mat4& transform, EntityId owningEntityId = NullEntityId);
		void SubmitSprite(Texture* sprite,const glm::vec3& color, const glm::mat4& transform, EntityId owningEntityId = NullEntityId);

		void SubmitText(const std::string& string, Font* font, const glm::vec3& color, const glm::mat4& transform, 
			float shadowDistance = 0, const glm::vec3& shadowColor = glm::vec3(0), EntityId owningEntityId = NullEntityId);

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
		static constexpr int CASCADE_COUNT = 4;
		size_t maxVertexCount;
		size_t maxSkinnedVertexCount;
		size_t maxTextCharacterCount;
		size_t maxObjects;

		int shaderBufferOffsetAlignment;

		std::unordered_map <UUID, std::vector<CacheEntry>> meshCache;

		std::unordered_map<Shader*, BatchData> batches;

		//TODO: Change batching method
		std::unordered_map<std::vector<glm::mat4>*, SkinnedMeshBatch> skinnedMeshBatches;

		Shader* wireframeShader;
		WireframeBatchData wireframeBatch;
		SpriteBatchData spriteBatch;

		RenderStats stats;

		uint32_t vao;
		uint32_t vertexBuffer;
		uint32_t indexBuffer;
		uint32_t nextIndexPosition = 0;
		uint32_t nextVertexPosition = 0;
		uint32_t vertexBufferHeadPosition = 0;
		uint32_t indexBufferHeadPosition = 0;

		uint32_t skinnedVao;
		uint32_t skinnedVertexBuffer;
		uint32_t skinnedIndexBuffer;
		uint32_t nextSkinnedIndexPosition = 0;
		uint32_t nextSkinnedVertexPosition = 0;
		uint32_t skinnedVertexBufferHeadPosition = 0;
		uint32_t skinnedIndexBufferHeadPosition = 0;

		uint32_t instanceDataBuffer;

		uint32_t textVao;
		uint32_t textIndexBuffer;
		uint32_t textVertexBuffers[3];
		TextVertexData* textBufferPointers[3];
		TextVertexData* textBufferHead = 0;
		Shader* textShader;
		Shader* spriteShader;
		uint32_t characterCount = 0;

		uint32_t transformBuffers[3];
		glm::mat4* transformBufferPointers[3];
		glm::mat4* transformBufferHead = 0;

		uint32_t entityIdBuffers[3];
		int* entityIdBufferPointers[3];
		int* entityIdBufferHead = 0;

		uint32_t materialBuffers[3];
		byte* materialBufferPointers[3];
		byte* materialBufferHead = 0;

		uint32_t indirectBuffers[3];
		RenderCommand* indirectBufferPointers[3];
		RenderCommand* indirectBufferHead = 0;

		uint32_t boneTransformBuffer;

		uint32_t objectCount = 0;

		int tripleBufferStage = 0;
		GLsync locks[3];

		HdrCubemap* ibl;
		DirectionalLightData directionalLight;
		CameraData camera;
		ShadowMappingData shadowMaps;

		double time;

	private:
		void InitStaticMeshBuffers();
		void InitSkinnedMeshBuffers();
		void InitSpriteBuffers();
		void InitTextBuffers();
		void InitStorageBuffers();

		void SetupMaterialData(Material* material, MaterialData& materialData);
		void DrawMeshes(int& offset, int& materialOffset, int& idOffset);
		void DrawSkinnedMeshes(int& offset, int& materialOffset, int& idOffset);
		void DrawSprites(int& offset, int& materialOffset, int& idOffset);
		void DrawWireframes(int& offset, int& materialOffset, int& idOffset);
		void DrawText();
	};
}