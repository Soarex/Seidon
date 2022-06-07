#pragma once
#include "../Core/UUID.h"
#include "../Core/Asset.h"

#include "../Graphics/Mesh.h"

namespace physx
{
	class PxTriangleMesh;
	class PxDefaultMemoryOutputStream;
}

namespace Seidon
{
	class MeshCollider : public Asset
	{
	public:
		MeshCollider(UUID id = UUID());
		~MeshCollider();

		bool CreateFromMesh(Mesh* mesh);
		void Destroy();

		physx::PxTriangleMesh* GetMeshData() { return meshData; }

		using Asset::Save;
		using Asset::Load;

		void Save(std::ofstream& out) override;
		void Load(std::ifstream& in) override;
	private:
		physx::PxTriangleMesh* meshData = nullptr;
		physx::PxDefaultMemoryOutputStream* vertexStream = nullptr;

	};
}