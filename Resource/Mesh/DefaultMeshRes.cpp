#include "Engine/pch.h"
#include "DefaultMeshRes.h"

#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/Mesh/Mesh.h>
#include <Engine/Resource/Mesh/VertexBuffer.h>
#include <Engine/Resource/Mesh/IndexBuffer.h>
#include <Engine/Resource/Mesh/Vertex.h>

namespace engine
{
	void DefaultMeshRes::Load()
	{
		auto vb = std::make_shared<VertexBuffer>();

		std::vector<VertexDebug> vertices;
		vertices.resize(4);
		vertices[0].position = {-0.5f, 0.5f, 0.5f, 1.f};
		vertices[1].position = { 0.5f, 0.5f, 0.5f, 1.f };
		vertices[2].position = { 0.5f, -0.5f, 0.5f, 1.f };
		vertices[3].position = {-0.5f, -0.5f, 0.5f, 1.f};
		vb->Create(vertices);

		std::vector<UINT> indices;
		indices.push_back(0u);
		indices.push_back(1u);
		indices.push_back(2u);
		indices.push_back(3u);
		indices.push_back(0u);

		auto ib = std::make_shared<IndexBuffer>();
		ib->Create(indices, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		 
		auto msh = std::make_shared<Mesh>();

		msh->SetBuffers(vb, ib);

		ResourceManager::GetInst().AddResource("DebugMesh", msh);	
		ResourceManager::GetInst().SetDefaultResource(msh);
	}
}

