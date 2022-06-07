#include "Renderer.h"

#include "QuadMesh.h"

#include "../Debug/Debug.h"
#include "../Debug/Timer.h"

#include "../Reflection/Reflection.h"

#include "../Utils/StringUtils.h"

namespace Seidon
{
	Renderer::Renderer(size_t maxObjects, size_t maxVertexCount, size_t maxSkinnedVertexCount, size_t maxTextCharacterCount)
		: maxObjects(maxObjects), maxVertexCount(maxVertexCount), maxSkinnedVertexCount(maxSkinnedVertexCount), maxTextCharacterCount(maxTextCharacterCount){}

	void Renderer::Init()
	{
		GL_CHECK(glGenBuffers(1, &instanceDataBuffer));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, instanceDataBuffer));

		uint32_t* ids = new uint32_t[maxObjects];

		for (int i = 0; i < maxObjects; i++)
			ids[i] = i;

		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, maxObjects * sizeof(uint32_t), ids, GL_STATIC_DRAW));

		delete[] ids;

		InitStaticMeshBuffers();
		InitSkinnedMeshBuffers();
		InitSpriteBuffers();
		InitTextBuffers();
		InitStorageBuffers();

		locks[0] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		locks[1] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		locks[2] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

		wireframeShader = new Shader();
		wireframeShader->Load("Shaders/Wireframe.sdshader");

		spriteShader = new Shader();
		spriteShader->Load("Shaders/Sprite.sdshader");

		textShader = new Shader();
		textShader->Load("Shaders/Text.sdshader");
	}

	void Renderer::InitStaticMeshBuffers()
	{
		GL_CHECK(glGenVertexArrays(1, &vao));
		GL_CHECK(glBindVertexArray(vao));

		GL_CHECK(glGenBuffers(1, &vertexBuffer));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, maxVertexCount * sizeof(Vertex), nullptr, GL_STATIC_DRAW));

		stats.vertexBufferSize = maxVertexCount;

		GL_CHECK(glGenBuffers(1, &indexBuffer));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxVertexCount * 3 * sizeof(int), nullptr, GL_STATIC_DRAW));

		stats.indexBufferSize = maxVertexCount * 3;

		// vertex positions
		GL_CHECK(glEnableVertexAttribArray(0));
		GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));

		// vertex normals
		GL_CHECK(glEnableVertexAttribArray(1));
		GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)));

		// vertex tangent
		GL_CHECK(glEnableVertexAttribArray(2));
		GL_CHECK(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent)));

		// vertex texture coords
		GL_CHECK(glEnableVertexAttribArray(3));
		GL_CHECK(glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords)));

		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, instanceDataBuffer));

		GL_CHECK(glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(uint32_t), 0));
		GL_CHECK(glVertexAttribDivisor(6, 1));
		GL_CHECK(glEnableVertexAttribArray(6));

		GL_CHECK(glBindVertexArray(0));
	}

	void Renderer::InitSkinnedMeshBuffers()
	{
		GL_CHECK(glGenVertexArrays(1, &skinnedVao));
		GL_CHECK(glBindVertexArray(skinnedVao));

		GL_CHECK(glGenBuffers(1, &skinnedVertexBuffer));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, skinnedVertexBuffer));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, maxSkinnedVertexCount * sizeof(SkinnedVertex), nullptr, GL_STATIC_DRAW));

		stats.skinnedVertexBufferSize = maxSkinnedVertexCount;

		GL_CHECK(glGenBuffers(1, &skinnedIndexBuffer));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skinnedIndexBuffer));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxSkinnedVertexCount * 3 * sizeof(int), nullptr, GL_STATIC_DRAW));

		stats.skinnedIndexBufferSize = maxSkinnedVertexCount * 3;

		// vertex positions
		GL_CHECK(glEnableVertexAttribArray(0));
		GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)0));

		// vertex normals
		GL_CHECK(glEnableVertexAttribArray(1));
		GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, normal)));

		// vertex tangent
		GL_CHECK(glEnableVertexAttribArray(2));
		GL_CHECK(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, tangent)));

		// vertex texture coords
		GL_CHECK(glEnableVertexAttribArray(3));
		GL_CHECK(glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, texCoords)));

		// vertex bone ids
		GL_CHECK(glEnableVertexAttribArray(4));
		GL_CHECK(glVertexAttribIPointer(4, 4, GL_INT, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, boneIds)));

		// vertex weights
		GL_CHECK(glEnableVertexAttribArray(5));
		GL_CHECK(glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, weights)));

		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, instanceDataBuffer));

		GL_CHECK(glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(uint32_t), 0));
		GL_CHECK(glVertexAttribDivisor(6, 1));
		GL_CHECK(glEnableVertexAttribArray(6));

		GL_CHECK(glBindVertexArray(0));
	}

	void Renderer::InitSpriteBuffers()
	{
		QuadMesh quad;

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, vertexBufferHeadPosition, quad.subMeshes[0]->vertices.size() * sizeof(Vertex), (void*)&quad.subMeshes[0]->vertices[0]);
		vertexBufferHeadPosition += quad.subMeshes[0]->vertices.size() * sizeof(Vertex);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexBufferHeadPosition, quad.subMeshes[0]->indices.size() * sizeof(uint32_t), (void*)&quad.subMeshes[0]->indices[0]);
		indexBufferHeadPosition += quad.subMeshes[0]->indices.size() * sizeof(uint32_t);
		glBindVertexArray(0);

		spriteBatch.command.firstIndex = nextIndexPosition;
		spriteBatch.command.baseVertex = nextVertexPosition;
		spriteBatch.command.count = quad.subMeshes[0]->indices.size();
		spriteBatch.command.baseInstance = 0;
		spriteBatch.command.instanceCount = 0;

		nextIndexPosition += quad.subMeshes[0]->indices.size();
		nextVertexPosition += quad.subMeshes[0]->vertices.size();
	}

	void Renderer::InitTextBuffers()
	{
		GL_CHECK(glGenVertexArrays(1, &textVao));
		GL_CHECK(glBindVertexArray(textVao));

		int flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
		GL_CHECK(glGenBuffers(3, textVertexBuffers));
		for (int i = 0; i < 3; i++)
		{
			GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, textVertexBuffers[i]));
			GL_CHECK(glBufferStorage(GL_ARRAY_BUFFER, maxTextCharacterCount * 4 * sizeof(TextVertexData), nullptr, flags));
			textBufferPointers[i] = (TextVertexData*)glMapBufferRange(GL_ARRAY_BUFFER, 0, maxTextCharacterCount * 4 * sizeof(TextVertexData), flags);
		}

		uint32_t* indices = new uint32_t[maxTextCharacterCount * 6 * sizeof(uint32_t)];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < maxTextCharacterCount * 6 * sizeof(uint32_t); i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 2;
			indices[i + 2] = offset + 1;

			indices[i + 3] = offset + 1;
			indices[i + 4] = offset + 2;
			indices[i + 5] = offset + 3;

			offset += 4;
		}

		GL_CHECK(glGenBuffers(1, &textIndexBuffer));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textIndexBuffer));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxTextCharacterCount * 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW));

		delete[] indices;

		// vertex positions
		GL_CHECK(glEnableVertexAttribArray(0));
		GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextVertexData), (void*)0));

		// vertex texture coords
		GL_CHECK(glEnableVertexAttribArray(3));
		GL_CHECK(glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertexData), (void*)offsetof(TextVertexData, uv)));

		// vertex color
		GL_CHECK(glEnableVertexAttribArray(7));
		GL_CHECK(glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(TextVertexData), (void*)offsetof(TextVertexData, color)));

		// shadow color and distance
		GL_CHECK(glEnableVertexAttribArray(8));
		GL_CHECK(glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertexData), (void*)offsetof(TextVertexData, shadowColorAndDistance)));

		// atlas handle
		GL_CHECK(glEnableVertexAttribArray(9));
		GL_CHECK(glVertexAttribLPointer(9, 1, GL_UNSIGNED_INT64_ARB, sizeof(TextVertexData), (void*)offsetof(TextVertexData, atlasHandle)));

		// entity id
		GL_CHECK(glEnableVertexAttribArray(10));
		GL_CHECK(glVertexAttribIPointer(10, 1, GL_INT, sizeof(TextVertexData), (void*)offsetof(TextVertexData, owningEntityId)));


		GL_CHECK(glBindVertexArray(0));
	}

	void Renderer::InitStorageBuffers()
	{
		int flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

		GL_CHECK(glGenBuffers(3, indirectBuffers));
		for (int i = 0; i < 3; i++)
		{
			GL_CHECK(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffers[i]));
			GL_CHECK(glBufferStorage(GL_DRAW_INDIRECT_BUFFER, maxObjects * sizeof(RenderCommand), nullptr, flags));
			indirectBufferPointers[i] = (RenderCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, maxObjects * sizeof(RenderCommand), flags);
		}

		GL_CHECK(glGenBuffers(3, transformBuffers));
		for (int i = 0; i < 3; i++)
		{
			GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, transformBuffers[i]));
			GL_CHECK(glBufferStorage(GL_SHADER_STORAGE_BUFFER, maxObjects * sizeof(glm::mat4), nullptr, flags));
			transformBufferPointers[i] = (glm::mat4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, maxObjects * sizeof(glm::mat4), flags);
		}

		GL_CHECK(glGenBuffers(1, &boneTransformBuffer));
		GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, boneTransformBuffer));
		GL_CHECK(glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_BONE_COUNT * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW));

		GL_CHECK(glGenBuffers(3, entityIdBuffers));
		for (int i = 0; i < 3; i++)
		{
			GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, entityIdBuffers[i]));
			GL_CHECK(glBufferStorage(GL_SHADER_STORAGE_BUFFER, maxObjects * sizeof(int), nullptr, flags));
			entityIdBufferPointers[i] = (int*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, maxObjects * sizeof(int), flags);
		}

		GL_CHECK(glGenBuffers(3, materialBuffers));
		for (int i = 0; i < 3; i++)
		{
			GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialBuffers[i]));
			GL_CHECK(glBufferStorage(GL_SHADER_STORAGE_BUFFER, maxObjects * sizeof(MaterialData), nullptr, flags));
			materialBufferPointers[i] = (byte*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, maxObjects * 500, flags);
		}

		glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &shaderBufferOffsetAlignment);
	}

	void Renderer::Begin()
	{
		static uint64_t oneSecondInNanoSeconds = 1000000000;
		uint64_t waitDuration = 0;
		int waitFlags = 0;

		while (1) 
		{
			GLenum res = glClientWaitSync(locks[tripleBufferStage], waitFlags, waitDuration);
			
			if (res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED)
				break;

			if (res == GL_WAIT_FAILED)
			{
				std::cerr << "Fence wait failed" << std::endl;
				break;
			}

			waitFlags = GL_SYNC_FLUSH_COMMANDS_BIT;
			waitDuration = oneSecondInNanoSeconds;
		}

		GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, transformBuffers[tripleBufferStage]));
		GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, materialBuffers[tripleBufferStage]));
		GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, entityIdBuffers[tripleBufferStage]));
		//GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, boneTransformBuffers[tripleBufferStage]));

		transformBufferHead = transformBufferPointers[tripleBufferStage];
		//boneTransformBufferHead = boneTransformBufferPointers[tripleBufferStage];
		entityIdBufferHead = entityIdBufferPointers[tripleBufferStage];
		materialBufferHead = materialBufferPointers[tripleBufferStage];
		indirectBufferHead = indirectBufferPointers[tripleBufferStage];
		textBufferHead = textBufferPointers[tripleBufferStage];

		stats.batchCount = 0;
		stats.objectCount = 0;

		characterCount = 0;
	}

	void Renderer::SubmitMesh(Mesh* mesh, std::vector<Material*>& materials, const glm::mat4& transform, EntityId owningEntityId)
	{
		if (meshCache.count(mesh->id) > 0)
		{
			std::vector<CacheEntry>& cachedSubmeshes = meshCache[mesh->id];

			int i = 0;
			for (CacheEntry& entry : cachedSubmeshes)
			{
				BatchData& batch = batches[materials[i]->shader];

				RenderCommand command;

				command.count = entry.indexBufferSize;
				command.instanceCount = 1;
				command.firstIndex = entry.indexBufferBegin;
				command.baseVertex = entry.vertexBufferBegin;
				command.baseInstance = batch.objectCount;
				objectCount++;

				stats.objectCount++;

				MaterialData material;
				SetupMaterialData(materials[i], material);

				batch.objectCount++;
				batch.transforms.push_back(transform);
				batch.commands.push_back(command);
				batch.materials.push_back(material);
				batch.entityIds.push_back((int)owningEntityId);

				i++;
			}

			return;
		}

		std::vector<CacheEntry> cache;
		cache.reserve(mesh->subMeshes.size());
		int i = 0;
		for (Submesh* s : mesh->subMeshes)
		{
			BatchData& batch = batches[materials[i]->shader];

			RenderCommand command;
			command.count = s->indices.size();
			command.instanceCount = 1;
			command.firstIndex = nextIndexPosition;
			command.baseVertex = nextVertexPosition;
			command.baseInstance = batch.objectCount;
			
			objectCount++;
			stats.objectCount++;
			CacheEntry entry;

			entry.vertexBufferBegin = nextVertexPosition;
			entry.vertexBufferSize = s->vertices.size();
			nextVertexPosition += s->vertices.size();

			entry.indexBufferBegin = nextIndexPosition;
			entry.indexBufferSize = s->indices.size();
			nextIndexPosition += s->indices.size();

			cache.push_back(entry);

			stats.vertexCount += s->vertices.size();
			stats.indexCount += s->indices.size();

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, vertexBufferHeadPosition, s->vertices.size() * sizeof(Vertex), (void*)&s->vertices[0]);
			vertexBufferHeadPosition += s->vertices.size() * sizeof(Vertex);
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexBufferHeadPosition, s->indices.size() * sizeof(uint32_t), (void*)&s->indices[0]);
			indexBufferHeadPosition += s->indices.size() * sizeof(uint32_t);
			glBindVertexArray(0);

			MaterialData material;
			SetupMaterialData(materials[i], material);

			batch.objectCount++;
			batch.transforms.push_back(transform);
			batch.commands.push_back(command);
			batch.materials.push_back(material);
			batch.entityIds.push_back((int)owningEntityId);

			i++;
		}

		meshCache[mesh->id] = cache;
	}
	
	void Renderer::SubmitSkinnedMesh(SkinnedMesh* mesh, std::vector<glm::mat4>& bones, std::vector<Material*>& materials, const glm::mat4& transform, EntityId owningEntityId)
	{
		if (meshCache.count(mesh->id) > 0)
		{
			std::vector<CacheEntry>& cachedSubmeshes = meshCache[mesh->id];

			int i = 0;
			for (CacheEntry& entry : cachedSubmeshes)
			{
				SkinnedMeshBatch& batch = skinnedMeshBatches[&bones];

				RenderCommand command;

				command.count = entry.indexBufferSize;
				command.instanceCount = 1;
				command.firstIndex = entry.indexBufferBegin;
				command.baseVertex = entry.vertexBufferBegin;
				command.baseInstance = 0;
				objectCount++;

				stats.objectCount++;

				MaterialData material;
				SetupMaterialData(materials[i], material);

				batch.objectCount++;
				batch.transforms.push_back(transform);
				batch.commands.push_back(command);
				batch.materials.push_back(material);
				batch.entityIds.push_back((int)owningEntityId);
				batch.bones = &bones;

				i++;
			}

			return;
		}

		std::vector<CacheEntry> cache;
		cache.reserve(mesh->subMeshes.size());
		int i = 0;
		for (SkinnedSubmesh* s : mesh->subMeshes)
		{
			SkinnedMeshBatch& batch = skinnedMeshBatches[&bones];

			RenderCommand command;
			command.count = s->indices.size();
			command.instanceCount = 1;
			command.firstIndex = nextSkinnedIndexPosition;
			command.baseVertex = nextSkinnedVertexPosition;
			command.baseInstance = 0;

			objectCount++;
			stats.objectCount++;
			CacheEntry entry;

			entry.vertexBufferBegin = nextSkinnedVertexPosition;
			entry.vertexBufferSize = s->vertices.size();
			nextSkinnedVertexPosition += s->vertices.size();

			entry.indexBufferBegin = nextSkinnedIndexPosition;
			entry.indexBufferSize = s->indices.size();
			nextSkinnedIndexPosition += s->indices.size();

			cache.push_back(entry);

			stats.skinnedVertexCount += s->vertices.size();
			stats.skinnedIndexCount += s->indices.size();

			glBindVertexArray(skinnedVao);
			glBindBuffer(GL_ARRAY_BUFFER, skinnedVertexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, skinnedVertexBufferHeadPosition, s->vertices.size() * sizeof(SkinnedVertex), (void*)&s->vertices[0]);
			skinnedVertexBufferHeadPosition += s->vertices.size() * sizeof(SkinnedVertex);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skinnedIndexBuffer);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, skinnedIndexBufferHeadPosition, s->indices.size() * sizeof(uint32_t), (void*)&s->indices[0]);
			skinnedIndexBufferHeadPosition += s->indices.size() * sizeof(uint32_t);
			glBindVertexArray(0);
			MaterialData material;
			SetupMaterialData(materials[i], material);

			batch.objectCount++;
			batch.transforms.push_back(transform);
			batch.commands.push_back(command);
			batch.materials.push_back(material);
			batch.entityIds.push_back((int)owningEntityId);
			batch.bones = &bones;

			i++;
		}

		meshCache[mesh->id] = cache;
	}
	
	void Renderer::SubmitMeshWireframe(Mesh* mesh, const glm::vec3& color, const glm::mat4& transform, EntityId owningEntityId)
	{
		if (meshCache.count(mesh->id) > 0)
		{
			std::vector<CacheEntry>& cachedSubmeshes = meshCache[mesh->id];

			int i = 0;
			for (CacheEntry& entry : cachedSubmeshes)
			{
				RenderCommand command;

				command.count = entry.indexBufferSize;
				command.instanceCount = 1;
				command.firstIndex = entry.indexBufferBegin;
				command.baseVertex = entry.vertexBufferBegin;
				command.baseInstance = wireframeBatch.objectCount;
				objectCount++;

				stats.objectCount++;

				wireframeBatch.objectCount++;
				wireframeBatch.transforms.push_back(transform);
				wireframeBatch.commands.push_back(command);
				wireframeBatch.colors.push_back(glm::vec4(color, 1.0));
				wireframeBatch.entityIds.push_back((int)owningEntityId);
			}

			return;
		}

		std::vector<CacheEntry> cache;
		cache.reserve(mesh->subMeshes.size());
		for (Submesh* s : mesh->subMeshes)
		{
			RenderCommand command;
			command.count = s->indices.size();
			command.instanceCount = 1;
			command.firstIndex = nextIndexPosition;
			command.baseVertex = nextVertexPosition;
			command.baseInstance = wireframeBatch.objectCount;

			objectCount++;
			stats.objectCount++;
			CacheEntry entry;

			entry.vertexBufferBegin = nextVertexPosition;
			entry.vertexBufferSize = s->vertices.size();
			nextVertexPosition += s->vertices.size();

			entry.indexBufferBegin = nextIndexPosition;
			entry.indexBufferSize = s->indices.size();
			nextIndexPosition += s->indices.size();

			cache.push_back(entry);

			stats.vertexCount += s->vertices.size();
			stats.indexCount += s->indices.size();

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, vertexBufferHeadPosition, s->vertices.size() * sizeof(Vertex), (void*)&s->vertices[0]);
			vertexBufferHeadPosition += s->vertices.size() * sizeof(Vertex);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexBufferHeadPosition, s->indices.size() * sizeof(uint32_t), (void*)&s->indices[0]);
			indexBufferHeadPosition += s->indices.size() * sizeof(uint32_t);
			glBindVertexArray(0);

			wireframeBatch.objectCount++;
			wireframeBatch.transforms.push_back(transform);
			wireframeBatch.commands.push_back(command);
			wireframeBatch.colors.push_back(glm::vec4(color, 1.0));
			wireframeBatch.entityIds.push_back((int)owningEntityId);
		}

		meshCache[mesh->id] = cache;
	}

	void Renderer::SubmitSprite(Texture* sprite, const glm::vec3& tint, const glm::mat4& transform, EntityId owningEntityId)
	{
		SpriteMaterialData material;
		sprite->MakeResident();
		material.textureHandle = sprite->GetRenderHandle();
		material.tint = glm::vec4(tint, 1.0);
		
		spriteBatch.command.instanceCount++;
		spriteBatch.objectCount++;
		spriteBatch.sprites.push_back(material);
		spriteBatch.transforms.push_back(transform);
		spriteBatch.entityIds.push_back((int)owningEntityId);
	}

	void Renderer::SubmitText(const std::string& string, Font* font, const glm::vec3& color, 
		 const glm::mat4& transform, float shadowDistance, const glm::vec3& shadowColor, EntityId owningEntityId)
	{
		if (string.empty()) return;

		std::u32string utf32string = ConvertToUTF32(string);

		font->GetAtlas()->MakeResident();
		glm::vec2 pos(0);

		float zBias = -0.001f;
		glm::vec4 currentBias = { 0, 0, 0, 0 };

		for (int i = 0; i < utf32string.size(); i++)
		{
			char32_t character = utf32string[i];
			
			if (character == '\n')
			{
				pos.x = 0;
				pos.y -= font->GetMetrics().lineHeight;
				continue;
			}

			const Glyph& glyph = font->GetGlyph(character);

			BoundingBox uvBounds = glyph.uvBounds;
			BoundingBox bounds = glyph.bounds;
			
			bounds.left += pos.x, bounds.bottom += pos.y, bounds.right += pos.x, bounds.top += pos.y;

			double texelWidth = 1. / font->GetAtlas()->GetWidth();
			double texelHeight = 1. / font->GetAtlas()->GetHeight();
			uvBounds.left *= texelWidth, uvBounds.bottom *= texelHeight, uvBounds.right *= texelWidth, uvBounds.top *= texelHeight;

			uint64_t atlasHandle = font->GetAtlas()->GetRenderHandle();

			textBufferHead->position = transform * glm::vec4(bounds.left, bounds.bottom, 0.0f, 1.0f) + currentBias;
			textBufferHead->uv = { uvBounds.left, uvBounds.bottom };
			textBufferHead->color = color;
			textBufferHead->shadowColorAndDistance = glm::vec4(shadowColor, shadowDistance);
			textBufferHead->atlasHandle = atlasHandle;
			textBufferHead->owningEntityId = owningEntityId;
			textBufferHead++;

			textBufferHead->position = transform * glm::vec4(bounds.left, bounds.top, 0.0f, 1.0f) + currentBias;
			textBufferHead->uv = { uvBounds.left, uvBounds.top };
			textBufferHead->color = color;
			textBufferHead->shadowColorAndDistance = glm::vec4(shadowColor, shadowDistance);
			textBufferHead->atlasHandle = atlasHandle;
			textBufferHead->owningEntityId = owningEntityId;
			textBufferHead++;

			textBufferHead->position = transform * glm::vec4(bounds.right, bounds.bottom, 0.0f, 1.0f) + currentBias;
			textBufferHead->uv = { uvBounds.right, uvBounds.bottom };
			textBufferHead->color = color;
			textBufferHead->shadowColorAndDistance = glm::vec4(shadowColor, shadowDistance);
			textBufferHead->atlasHandle = atlasHandle;
			textBufferHead->owningEntityId = owningEntityId;
			textBufferHead++;

			textBufferHead->position = transform * glm::vec4(bounds.right, bounds.top, 0.0f, 1.0f) + currentBias;
			textBufferHead->uv = { uvBounds.right, uvBounds.top };
			textBufferHead->color = color;
			textBufferHead->shadowColorAndDistance = glm::vec4(shadowColor, shadowDistance);
			textBufferHead->atlasHandle = atlasHandle;
			textBufferHead->owningEntityId = owningEntityId;
			textBufferHead++;

			characterCount++;

			pos.x += font->GetAdvance(character, utf32string[i + 1]);
			currentBias.z += zBias;
		}
	}

	void Renderer::SetCamera(const CameraData& camera)
	{
		this->camera = camera;
	}

	void Renderer::SetShadowMaps(const ShadowMappingData& shadowMaps)
	{
		this->shadowMaps = shadowMaps;
	}

	void Renderer::SetDirectionalLight(const DirectionalLightData& directionalLight)
	{
		this->directionalLight = directionalLight;
	}

	void Renderer::SetIBL(HdrCubemap* cubemap)
	{
		this->ibl = cubemap;
	}

	void Renderer::SetTime(double time)
	{
		this->time = time;
	}

	void Renderer::Render()
	{
		GL_CHECK(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffers[tripleBufferStage]));

		int offset = 0;
		int materialOffset = 0;
		int idOffset = 0;

		DrawMeshes(offset, materialOffset, idOffset);
		DrawSkinnedMeshes(offset, materialOffset, idOffset);
		DrawWireframes(offset, materialOffset, idOffset);
		DrawSprites(offset, materialOffset, idOffset);
		DrawText();

		GL_CHECK(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0));

		objectCount = 0;
	}

	void Renderer::End()
	{
		glDeleteSync(locks[tripleBufferStage]);
		locks[tripleBufferStage] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		tripleBufferStage = (tripleBufferStage + 1) % 3;
	}

	void Renderer::Destroy()
	{
		for(int i = 0; i < 3; i++)
		{
			glUnmapNamedBuffer(transformBuffers[i]);
			glUnmapNamedBuffer(materialBuffers[i]);
			glUnmapNamedBuffer(indirectBuffers[i]);
			glUnmapNamedBuffer(textVertexBuffers[i]);
			glDeleteSync(locks[i]);
		}

		glDeleteBuffers(3, transformBuffers);
		glDeleteBuffers(3, materialBuffers);
		glDeleteBuffers(3, indirectBuffers);
		glDeleteBuffers(3, textVertexBuffers);
		glDeleteBuffers(1, &boneTransformBuffer);

		glDeleteBuffers(1, &indexBuffer);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &skinnedIndexBuffer);
		glDeleteBuffers(1, &skinnedVertexBuffer);
		glDeleteBuffers(1, &instanceDataBuffer);
		glDeleteBuffers(1, &textIndexBuffer);

		glDeleteVertexArrays(1, &vao);
		glDeleteVertexArrays(1, &skinnedVao);
		glDeleteVertexArrays(1, &textVao);

		delete wireframeShader;
		delete textShader;
	}

	void Renderer::DrawMeshes(int& offset, int& materialOffset, int& idOffset)
	{
		glBindVertexArray(vao);
		for (auto& [shader, batch] : batches)
		{
			shader->Use();

			shader->SetMat4("camera.viewMatrix", camera.viewMatrix);
			shader->SetMat4("camera.projectionMatrix", camera.projectionMatrix);
			shader->SetVec3("camera.position", camera.position);

			shader->SetVec3("directionalLight.direction", directionalLight.direction);
			shader->SetVec3("directionalLight.color", directionalLight.color);

			if(shadowMaps.lightSpaceMatrices)
				for (int i = 0; i < CASCADE_COUNT; i++)
				{
					shader->SetMat4("lightSpaceMatrices[" + std::to_string(i) + "]", shadowMaps.lightSpaceMatrices[i]);
					shader->SetFloat("shadowMappingData.cascadeFarPlaneDistances[" + std::to_string(i) + "]", shadowMaps.cascadeFarPlaneDistances[i]);
					shader->SetInt("shadowMappingData.cascadeCount", CASCADE_COUNT);
					shadowMaps.shadowMaps[i].Bind(8 + i);
				}

			shader->SetDouble("time", time);

			if (ibl)
			{
				ibl->BindIrradianceMap(5);
				ibl->BindPrefilteredMap(6);
				ibl->BindBRDFLookupMap(7);
			}

			memcpy(indirectBufferHead, &batch.commands[0], batch.commands.size() * sizeof(RenderCommand));
			indirectBufferHead += batch.commands.size();

			memcpy(transformBufferHead, &batch.transforms[0], batch.transforms.size() * sizeof(glm::mat4));
			transformBufferHead += batch.transforms.size();

			memcpy(entityIdBufferHead, &batch.entityIds[0], batch.entityIds.size() * sizeof(int));
			entityIdBufferHead += batch.entityIds.size();

			int materialSize = 0;
			for (MaterialData& m : batch.materials)
			{
				memcpy(materialBufferHead, m.data, m.size);
				materialBufferHead += m.size;
				materialSize += m.size;
			}

			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, transformBuffers[tripleBufferStage], offset * sizeof(glm::mat4), batch.transforms.size() * sizeof(glm::mat4));
			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, entityIdBuffers[tripleBufferStage], idOffset, batch.entityIds.size() * sizeof(int));

			if (materialSize != 0)
				glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, materialBuffers[tripleBufferStage], materialOffset, materialSize);

			GL_CHECK(glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(offset * sizeof(RenderCommand)), batch.objectCount, 0));

			offset += batch.objectCount;
			materialOffset += materialSize;
			idOffset += batch.objectCount * sizeof(int);

			if (idOffset % shaderBufferOffsetAlignment != 0)
			{
				int alignedOffset = shaderBufferOffsetAlignment - (idOffset % shaderBufferOffsetAlignment);;
				idOffset += alignedOffset;
				entityIdBufferHead = (int*)((byte*)entityIdBufferHead + alignedOffset);
			}

			stats.batchCount++;
		}
		GL_CHECK(glBindVertexArray(0));
		batches.clear();
	}

	void Renderer::DrawSkinnedMeshes(int& offset, int& materialOffset, int& idOffset)
	{
		glBindVertexArray(skinnedVao);

		for (auto& [bones, batch] : skinnedMeshBatches)
		{
			Shader* shader = batch.materials[0].shader;
			shader->Use();

			shader->SetMat4("camera.viewMatrix", camera.viewMatrix);
			shader->SetMat4("camera.projectionMatrix", camera.projectionMatrix);
			shader->SetVec3("camera.position", camera.position);

			shader->SetVec3("directionalLight.direction", directionalLight.direction);
			shader->SetVec3("directionalLight.color", directionalLight.color);

			if (shadowMaps.lightSpaceMatrices)
				for (int i = 0; i < CASCADE_COUNT; i++)
				{
					shader->SetMat4("lightSpaceMatrices[" + std::to_string(i) + "]", shadowMaps.lightSpaceMatrices[i]);
					shader->SetFloat("shadowMappingData.cascadeFarPlaneDistances[" + std::to_string(i) + "]", shadowMaps.cascadeFarPlaneDistances[i]);
					shader->SetInt("shadowMappingData.cascadeCount", CASCADE_COUNT);
					shadowMaps.shadowMaps[i].Bind(8 + i);
				}

			shader->SetDouble("time", time);

			if (ibl)
			{
				ibl->BindIrradianceMap(5);
				ibl->BindPrefilteredMap(6);
				ibl->BindBRDFLookupMap(7);
			}

			memcpy(indirectBufferHead, &batch.commands[0], batch.commands.size() * sizeof(RenderCommand));
			indirectBufferHead += batch.commands.size();

			memcpy(transformBufferHead, &batch.transforms[0], batch.transforms.size() * sizeof(glm::mat4));
			transformBufferHead += batch.transforms.size();

			memcpy(entityIdBufferHead, &batch.entityIds[0], batch.entityIds.size() * sizeof(int));
			entityIdBufferHead += batch.entityIds.size();

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, boneTransformBuffer);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, batch.bones->size() * sizeof(glm::mat4), &(*batch.bones)[0]);

			int materialSize = 0;
			for (MaterialData& m : batch.materials)
			{
				memcpy(materialBufferHead, m.data, m.size);
				materialBufferHead += m.size;
				materialSize += m.size;
			}

			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, transformBuffers[tripleBufferStage], offset * sizeof(glm::mat4), batch.transforms.size() * sizeof(glm::mat4));
			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, boneTransformBuffer, 0, batch.bones->size() * sizeof(glm::mat4));
			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, entityIdBuffers[tripleBufferStage], idOffset, sizeof(int));

			if (materialSize != 0)
				glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, materialBuffers[tripleBufferStage], materialOffset, materialSize);

			GL_CHECK(glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(offset * sizeof(RenderCommand)), batch.objectCount, 0));

			offset += batch.objectCount;
			materialOffset += materialSize;
			idOffset += batch.entityIds.size() * sizeof(int);

			if (idOffset % shaderBufferOffsetAlignment != 0)
			{
				int alignedOffset = shaderBufferOffsetAlignment - (idOffset % shaderBufferOffsetAlignment);;
				idOffset += alignedOffset;
				entityIdBufferHead = (int*)((byte*)entityIdBufferHead + alignedOffset);
			}

			stats.batchCount++;
		}

		skinnedMeshBatches.clear();
		GL_CHECK(glBindVertexArray(0));
	}

	void Renderer::DrawWireframes(int& offset, int& materialOffset, int& idOffset)
	{
		if (wireframeBatch.objectCount == 0) return;

		glBindVertexArray(vao);
		wireframeShader->Use();

		wireframeShader->SetMat4("camera.viewMatrix", camera.viewMatrix);
		wireframeShader->SetMat4("camera.projectionMatrix", camera.projectionMatrix);
		wireframeShader->SetVec3("camera.position", camera.position);

		memcpy(indirectBufferHead, &wireframeBatch.commands[0], wireframeBatch.commands.size() * sizeof(RenderCommand));
		indirectBufferHead += wireframeBatch.commands.size();

		memcpy(transformBufferHead, &wireframeBatch.transforms[0], wireframeBatch.transforms.size() * sizeof(glm::mat4));
		transformBufferHead += wireframeBatch.transforms.size();
		
		memcpy(materialBufferHead, &wireframeBatch.colors[0], wireframeBatch.colors.size() * sizeof(glm::vec4));
		materialBufferHead += wireframeBatch.colors.size() * sizeof(glm::vec4);
		
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, transformBuffers[tripleBufferStage], offset * sizeof(glm::mat4), wireframeBatch.transforms.size() * sizeof(glm::mat4));
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, materialBuffers[tripleBufferStage], materialOffset, wireframeBatch.colors.size() * sizeof(glm::vec4));

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(offset * sizeof(RenderCommand)), wireframeBatch.objectCount, 0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		offset += wireframeBatch.objectCount;
		materialOffset += wireframeBatch.colors.size() * sizeof(glm::vec4);

		stats.batchCount++;

		wireframeBatch.colors.clear();
		wireframeBatch.commands.clear();
		wireframeBatch.transforms.clear();
		wireframeBatch.entityIds.clear();
		wireframeBatch.objectCount = 0;

		GL_CHECK(glBindVertexArray(0));
	}

	void Renderer::DrawSprites(int& offset, int& materialOffset, int& idOffset)
	{
		if (spriteBatch.objectCount == 0) return;

		glBindVertexArray(vao);
		spriteShader->Use();

		spriteShader->SetMat4("camera.viewMatrix", camera.viewMatrix);
		spriteShader->SetMat4("camera.projectionMatrix", camera.projectionMatrix);
		spriteShader->SetVec3("camera.position", camera.position);

		*indirectBufferHead = spriteBatch.command;
		indirectBufferHead ++;

		memcpy(transformBufferHead, &spriteBatch.transforms[0], spriteBatch.transforms.size() * sizeof(glm::mat4));
		transformBufferHead += spriteBatch.transforms.size();

		memcpy(materialBufferHead, &spriteBatch.sprites[0], spriteBatch.sprites.size() * sizeof(SpriteMaterialData));
		materialBufferHead += spriteBatch.sprites.size() * sizeof(SpriteMaterialData);

		memcpy(entityIdBufferHead, &spriteBatch.entityIds[0], spriteBatch.entityIds.size() * sizeof(int));
		entityIdBufferHead += spriteBatch.entityIds.size();

		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, transformBuffers[tripleBufferStage], offset * sizeof(glm::mat4), spriteBatch.transforms.size() * sizeof(glm::mat4));
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, materialBuffers[tripleBufferStage], materialOffset, spriteBatch.sprites.size() * sizeof(SpriteMaterialData));
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, entityIdBuffers[tripleBufferStage], idOffset, sizeof(int));

		glDisable(GL_CULL_FACE);

		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(offset * sizeof(RenderCommand)), 1, 0);

		glEnable(GL_CULL_FACE);

		offset++;
		materialOffset += spriteBatch.sprites.size() * sizeof(SpriteMaterialData);

		idOffset += spriteBatch.entityIds.size() * sizeof(int);

		if (idOffset % shaderBufferOffsetAlignment != 0)
		{
			int alignedOffset = shaderBufferOffsetAlignment - (idOffset % shaderBufferOffsetAlignment);;
			idOffset += alignedOffset;
			entityIdBufferHead = (int*)((byte*)entityIdBufferHead + alignedOffset);
		}

		stats.batchCount++;

		spriteBatch.transforms.clear();
		spriteBatch.entityIds.clear();
		spriteBatch.sprites.clear();

		spriteBatch.objectCount = 0;
		spriteBatch.command.instanceCount = 0;

		GL_CHECK(glBindVertexArray(0));
	}

	void Renderer::DrawText()
	{
		glBindVertexArray(textVao);
		glBindBuffer(GL_ARRAY_BUFFER, textVertexBuffers[tripleBufferStage]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textIndexBuffer);

		textShader->Use();
		textShader->SetMat4("camera.viewMatrix", camera.viewMatrix);
		textShader->SetMat4("camera.projectionMatrix", camera.projectionMatrix);
		textShader->SetVec3("camera.position", camera.position);

		glDisable(GL_CULL_FACE);

		glDrawElements(GL_TRIANGLES, characterCount * 6, GL_UNSIGNED_INT, nullptr);

		glEnable(GL_CULL_FACE);

		GL_CHECK(glBindVertexArray(0));

		characterCount = 0;
	}

	void Renderer::SetupMaterialData(Material* material, MaterialData& materialData)
	{
		int offset = 0;
		int alignment = 0;
		materialData.shader = material->shader;
		byte* shaderData = materialData.data;

		for (MemberData& m : material->shader->GetBufferLayout()->members)
			switch (m.type)
			{
			case Types::FLOAT_NORMALIZED: case Types::FLOAT: case Types::FLOAT_ANGLE:
			{
				*(float*)(shaderData + offset) = *(float*)(material->data + m.offset);

				offset += sizeof(float);
				alignment = std::max<int>(alignment, sizeof(float));
				break;
			}
			case Types::VECTOR2: case Types::VECTOR2_ANGLES:
			{
				*(glm::vec2*)(shaderData + offset) = *(glm::vec2*)(material->data + m.offset);

				offset += sizeof(glm::vec2);
				alignment = std::max<int>(alignment, sizeof(glm::vec2));
				break;
			}
			case Types::VECTOR3_COLOR: case Types::VECTOR3: case Types::VECTOR3_ANGLES:
			{
				*(glm::vec3*)(shaderData + offset) = *(glm::vec3*)(material->data + m.offset);

				offset += sizeof(glm::vec4);
				alignment = std::max<int>(alignment, sizeof(glm::vec4));
				break;
			}
			case Types::TEXTURE:
			{
				Texture* t = *(Texture**)(material->data + m.offset);
				t->MakeResident();
				*(uint64_t*)(shaderData + offset) = t->GetRenderHandle();

				offset += sizeof(uint64_t);
				alignment = std::max<int>(alignment, sizeof(uint64_t));
				break;
			}
			}

		if (alignment != 0 && offset % alignment != 0)
			materialData.size = offset + alignment - (offset % alignment);
		else
			materialData.size = offset;
	}
}