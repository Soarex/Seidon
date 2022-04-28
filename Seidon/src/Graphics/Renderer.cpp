#include "Renderer.h"

#include "../Debug/Debug.h"
#include "../Debug/Timer.h"

#include "../Reflection/Reflection.h"

namespace Seidon
{
	void Renderer::Init()
	{
		GL_CHECK(glGenVertexArrays(1, &vao));
		GL_CHECK(glBindVertexArray(vao));

		GL_CHECK(glGenBuffers(1, &vertexBuffer));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * sizeof(Vertex), nullptr, GL_STATIC_DRAW));

		GL_CHECK(glGenBuffers(1, &indexBuffer));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, (VERTEX_COUNT / 3) * sizeof(int), nullptr, GL_STATIC_DRAW));

		int flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

		GL_CHECK(glGenBuffers(3, indirectBuffers));
		for (int i = 0; i < 3; i++)
		{
			GL_CHECK(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffers[i]));
			GL_CHECK(glBufferStorage(GL_DRAW_INDIRECT_BUFFER, MAX_OBJECTS * sizeof(RenderCommand), nullptr, flags));
			indirectBufferPointers[i] = (RenderCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, MAX_OBJECTS * sizeof(RenderCommand), flags);
		}

		GL_CHECK(glGenBuffers(3, storageBuffers));
		for (int i = 0; i < 3; i++)
		{
			GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, storageBuffers[i]));
			GL_CHECK(glBufferStorage(GL_SHADER_STORAGE_BUFFER, MAX_OBJECTS * sizeof(glm::mat4), nullptr, flags));
			storageBufferPointers[i] = (glm::mat4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, MAX_OBJECTS * sizeof(glm::mat4), flags);
		}


		GL_CHECK(glGenBuffers(3, materialBuffers));
		for (int i = 0; i < 3; i++)
		{
			GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialBuffers[i]));
			GL_CHECK(glBufferStorage(GL_SHADER_STORAGE_BUFFER, MAX_OBJECTS * sizeof(MaterialData), nullptr, flags));
			materialBufferPointers[i] = (byte*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, MAX_OBJECTS * 500, flags);
		}

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

		// vertex bone ids
		GL_CHECK(glEnableVertexAttribArray(4));
		GL_CHECK(glVertexAttribIPointer(4, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIds)));

		// vertex weights
		GL_CHECK(glEnableVertexAttribArray(5));
		GL_CHECK(glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights)));

		GL_CHECK(glGenBuffers(1, &instanceDataBuffer));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, instanceDataBuffer));

		uint32_t ids[MAX_OBJECTS];

		for (int i = 0; i < MAX_OBJECTS; i++)
			ids[i] = i;
		
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, MAX_OBJECTS * sizeof(uint32_t), ids, GL_STATIC_DRAW));

		GL_CHECK(glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(uint32_t), 0));
		GL_CHECK(glVertexAttribDivisor(6, 1));
		GL_CHECK(glEnableVertexAttribArray(6));

		GL_CHECK(glBindVertexArray(0));

		locks[0] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		locks[1] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		locks[2] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
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

		GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, storageBuffers[tripleBufferStage]));
		GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, materialBuffers[tripleBufferStage]));

		storageBufferHead = storageBufferPointers[tripleBufferStage];
		materialBufferHead = materialBufferPointers[tripleBufferStage];
		indirectBufferHead = indirectBufferPointers[tripleBufferStage];
	}

	void Renderer::SubmitMesh(Mesh* mesh, std::vector<Material*>& materials, glm::mat4& transform)
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

				MaterialData material;
				int offset = 0;
				int alignment = 0;

				for (MemberData& m : materials[i]->shader->GetBufferLayout()->members)
					switch (m.type)
					{
					case Types::VECTOR3_COLOR:
					{
						*(glm::vec3*)(material.data + offset) = *(glm::vec3*)(materials[i]->data + m.offset);

						offset += sizeof(glm::vec4);
						alignment = std::max<int>(alignment, sizeof(glm::vec4));
						break;
					}
					case Types::TEXTURE:
					{
						Texture* t = *(Texture**)(materials[i]->data + m.offset);
						t->MakeResident();
						*(uint64_t*)(material.data + offset) = t->GetRenderHandle();

						offset += sizeof(uint64_t);
						alignment = std::max<int>(alignment, sizeof(uint64_t));
						break;
					}
					}

				if (alignment != 0 && offset % alignment != 0)
					material.size = offset + alignment - (offset % alignment);
				else
					material.size = offset;

				batch.objectCount++;
				batch.transforms.push_back(transform);
				batch.commands.push_back(command);
				batch.materials.push_back(material);

				i++;
			}

			return;
		}

		std::vector<CacheEntry> cache;
		cache.reserve(mesh->subMeshes.size());
		int i = 0;
		for (SubMesh* s : mesh->subMeshes)
		{
			BatchData& batch = batches[materials[i]->shader];

			RenderCommand command;
			command.count = s->indices.size();
			command.instanceCount = 1;
			command.firstIndex = nextIndexPosition;
			command.baseVertex = nextVertexPosition;
			command.baseInstance = batch.objectCount;
			
			objectCount++;
			CacheEntry entry;

			entry.vertexBufferBegin = nextVertexPosition;
			entry.vertexBufferSize = s->vertices.size();
			nextVertexPosition += s->vertices.size();

			entry.indexBufferBegin = nextIndexPosition;
			entry.indexBufferSize = s->indices.size();
			nextIndexPosition += s->indices.size();

			cache.push_back(entry);

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, vertexBufferHeadPosition, s->vertices.size() * sizeof(Vertex), (void*)&s->vertices[0]);
			vertexBufferHeadPosition += s->vertices.size() * sizeof(Vertex);
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexBufferHeadPosition, s->indices.size() * sizeof(uint32_t), (void*)&s->indices[0]);
			indexBufferHeadPosition += s->indices.size() * sizeof(uint32_t);

			MaterialData material;
			int offset = 0;
			int alignment = 0;

			for (MemberData& m : materials[i]->shader->GetBufferLayout()->members)
				switch (m.type)
				{
				case Types::VECTOR3_COLOR:
				{
					*(glm::vec3*)(material.data + offset) = *(glm::vec3*)(materials[i]->data + m.offset);

					offset += sizeof(glm::vec4);
					alignment = std::max<int>(alignment, sizeof(glm::vec4));
					break;
				}
				case Types::TEXTURE:
				{
					Texture* t = *(Texture**)(materials[i]->data + m.offset);
					t->MakeResident();
					*(uint64_t*)(material.data + offset) = t->GetRenderHandle();

					offset += sizeof(uint64_t);
					alignment = std::max<int>(alignment, sizeof(uint64_t));
					break;
				}
				}

			if (alignment != 0 && offset % alignment != 0)
				material.size = offset + alignment - (offset % alignment);
			else
				material.size = offset;

			batch.objectCount++;
			batch.transforms.push_back(transform);
			batch.commands.push_back(command);
			batch.materials.push_back(material);

			i++;
		}

		meshCache[mesh->id] = cache;
	}

	void Renderer::SetCameraPosition(const glm::vec3& cameraPosition)
	{
		this->cameraPosition = cameraPosition;
	}

	void Renderer::SetViewMatrix(const glm::mat4& viewMatrix)
	{
		this->viewMatrix = viewMatrix;
	}

	void Renderer::SetProjectionMatrix(const glm::mat4& projectionMatrix)
	{
		this->projectionMatrix = projectionMatrix;
	}

	void Renderer::SetTime(double time)
	{
		this->time = time;
	}

	void Renderer::Render()
	{
		if (objectCount == 0) return;

		GL_CHECK(glBindVertexArray(vao));
		GL_CHECK(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffers[tripleBufferStage]));

		int offset = 0;
		int materialOffset = 0;
		for (auto& [shader, batch] : batches)
		{
			shader->Use();
			shader->SetMat4("camera.viewMatrix", viewMatrix);
			shader->SetMat4("camera.projectionMatrix", projectionMatrix);
			shader->SetVec3("camera.position", cameraPosition);
			shader->SetDouble("time", time);

			memcpy(indirectBufferHead, &batch.commands[0], batch.commands.size() * sizeof(RenderCommand));
			indirectBufferHead += batch.commands.size();

			memcpy(storageBufferHead, &batch.transforms[0], batch.transforms.size() * sizeof(glm::mat4));
			storageBufferHead += batch.transforms.size();

			int materialSize = 0;
			for (MaterialData& m : batch.materials)
			{
				memcpy(materialBufferHead, m.data, m.size);
				materialBufferHead += m.size;
				materialSize += m.size;
			}

			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, storageBuffers[tripleBufferStage], offset * sizeof(glm::mat4), batch.transforms.size() * sizeof(glm::mat4));
			
			if(materialSize != 0)
				glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, materialBuffers[tripleBufferStage], materialOffset, materialSize);
			
			GL_CHECK(glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(offset * sizeof(RenderCommand)), batch.objectCount, 0));

			offset += batch.objectCount;
			materialOffset += materialSize;
		}

		GL_CHECK(glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0));

		GL_CHECK(glBindVertexArray(0));

		objectCount = 0;

		batches.clear();
	}

	void Renderer::End()
	{
		glDeleteSync(locks[tripleBufferStage]);
		locks[tripleBufferStage] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		tripleBufferStage = (tripleBufferStage + 1) % 3;
	}

	void Renderer::Destroy()
	{

	}
}