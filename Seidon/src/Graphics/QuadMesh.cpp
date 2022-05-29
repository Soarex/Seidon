#include "QuadMesh.h"

namespace Seidon
{
	QuadMesh::QuadMesh(UUID id)
		: Mesh(id)
	{
		name = "Quad Mesh";

		Submesh* submesh = new Submesh();
		submesh->name = "Quad";

		Vertex v;
		v.position = { -0.5f, 0.0f, 0.5f };
		v.normal = { 0, 1, 0 };
		v.tangent = { 1, 0, 0 };
		v.texCoords = { 0, 1};
		submesh->vertices.push_back(v);

		v.position = { 0.5f, 0.0f, 0.5f };
		v.normal = { 0, 1, 0 };
		v.tangent = { 1, 0, 0 };
		v.texCoords = { 1, 1 };
		submesh->vertices.push_back(v);

		v.position = { 0.5f, 0.0f, -0.5f };
		v.normal = { 0, 1, 0 };
		v.tangent = { 1, 0, 0 };
		v.texCoords = { 1, 0 };
		submesh->vertices.push_back(v);

		v.position = { -0.5f, 0.0f, -0.5f };
		v.normal = { 0, 1, 0 };
		v.tangent = { 1, 0, 0 };
		v.texCoords = { 0, 0 };
		submesh->vertices.push_back(v);

		submesh->indices = { 0, 1, 2, 0, 2, 3 };
	}
}