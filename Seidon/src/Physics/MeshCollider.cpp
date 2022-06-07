#include "MeshCollider.h"

#include "../Graphics/Mesh.h"
#include "../Core/Application.h"

#include <limits>

namespace Seidon
{
	MeshCollider::MeshCollider(UUID id)
	{
		this->id = id;	
		vertexStream = new physx::PxDefaultMemoryOutputStream();
	}

	MeshCollider::~MeshCollider()
	{
		delete vertexStream;
	}

	void MeshCollider::Destroy()
	{
		meshData->release();
		meshData = nullptr;
	}

	bool MeshCollider::CreateFromMesh(Mesh* mesh)
	{
		PhysicsApi& api = *Application::Get()->GetPhysicsApi();

		int vertexCount = 0;
		int indexCount = 0;
		for (Submesh* submesh : mesh->subMeshes)
		{
			vertexCount += submesh->vertices.size();
			indexCount += submesh->indices.size();
		}

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		vertices.resize(vertexCount);
		indices.resize(indexCount);

		int vertexOffset = 0;
		int indexOffset = 0;
		for (Submesh* submesh : mesh->subMeshes)
		{
			memcpy(&vertices[vertexOffset], &submesh->vertices[0], submesh->vertices.size() * sizeof(Vertex));

			for (int i = 0; i < submesh->indices.size(); i++)
				indices[indexOffset + i] = submesh->indices[i] + vertexOffset;

			vertexOffset += submesh->vertices.size();
			indexOffset += submesh->indices.size();
		}

		physx::PxTriangleMeshDesc meshDesc;
		meshDesc.setToDefault();

		meshDesc.points.count = vertices.size();
		meshDesc.points.stride = sizeof(Vertex);
		meshDesc.points.data = vertices.data();

		meshDesc.triangles.count = indices.size();
		meshDesc.triangles.stride = 3 * sizeof(int);
		meshDesc.triangles.data = indices.data();

		if (indices.size() >= std::numeric_limits<uint16_t>::max())
		{
			std::cerr << "Error cooking mesh " << mesh->name << ": index count exceeds the max of " << std::numeric_limits<uint16_t>::max() << std::endl;
			return false;
		}

		physx::PxTriangleMeshCookingResult::Enum result;
		physx::PxCookingParams params = api.GetCooker()->getParams();

		params.meshPreprocessParams.set(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);

		bool status = api.GetCooker()->cookTriangleMesh(meshDesc, *vertexStream, &result);

		if (!status)
		{
			std::cerr << "Error cooking mesh " << mesh->name << std::endl;
			return false;
		}

		physx::PxDefaultMemoryInputData readBuffer(vertexStream->getData(), vertexStream->getSize());
		meshData = api.GetPhysics()->createTriangleMesh(readBuffer);

		return status;
	}

	void MeshCollider::Save(std::ofstream& out)
	{
		out.write((char*)&id, sizeof(UUID));

		size_t size = vertexStream->getSize();
		out.write((char*)&size, sizeof(size_t));
		out.write((char*)vertexStream->getData(), size);
	}

	void MeshCollider::Load(std::ifstream& in)
	{
		in.read((char*)&id, sizeof(UUID));

		size_t size = 0;
		in.read((char*)&size, sizeof(size_t));

		byte* data = new byte[size];
		in.read((char*)data, size);

		vertexStream->write(data, size);

		PhysicsApi& api = *Application::Get()->GetPhysicsApi();
		physx::PxDefaultMemoryInputData readBuffer(vertexStream->getData(), vertexStream->getSize());
		meshData = api.GetPhysics()->createTriangleMesh(readBuffer);

		delete data;
	}
}