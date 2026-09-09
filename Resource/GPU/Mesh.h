#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/DX11.h>

#include <Engine/Util/IDAllocator.h>

namespace engine
{
    class Mesh
		: public Resource
    {
		ENTITY_INFO(Mesh, Resource)

	public:
		struct SubMesh
		{
			// 현재 SubMesh의 Index 시작 지점.
			uint32 index_start = 0;

			// 현재 SubMesh의 Index 개수.
			uint32 index_count = 0;
		};

		Mesh();
		virtual ~Mesh() override;

		bool IsReady() const { return (bool)vertex_buffer_ && (bool)index_buffer_; }
		void Bind(ID3D11DeviceContext* context);
		void Draw(ID3D11DeviceContext* context, uint32 instance_count, uint32 submesh_index = 0);

		virtual bool LoadFromFile(const stdfs::path& path) override;

		bool CreateVertexBuffer(const void* data, uint32 data_stride, uint32 data_count);

		template <typename T>
		bool CreateVertexBuffer(const std::vector<T>& vertices)
		{
			return CreateVertexBuffer(vertices.data(), (uint32)sizeof(T), (uint32)vertices.size());
		}

		bool CreateIndexBuffer(const std::vector<uint16>& indices, D3D11_PRIMITIVE_TOPOLOGY topology, std::vector<SubMesh> sub_meshes = {}) {
			return CreateIndexBuffer((const void*)indices.data(), DXGI_FORMAT_R16_UINT, (uint32)indices.size(), topology, sub_meshes);
		}
		bool CreateIndexBuffer(const std::vector<uint32>& indices, D3D11_PRIMITIVE_TOPOLOGY topology, std::vector<SubMesh> sub_meshes = {}) {
			return CreateIndexBuffer((const void*)indices.data(), DXGI_FORMAT_R32_UINT, (uint32)indices.size(), topology, sub_meshes);
		}

		bool CreateIndexBuffer(const void* p_data, DXGI_FORMAT format, uint32 data_count, D3D11_PRIMITIVE_TOPOLOGY topology, std::vector<SubMesh> sub_meshes = {});

	private:
		MeshID mesh_ID_;    // RenderKey에 패킹되는 Mesh 고유 ID. ScopedID로 관리됨

#pragma region VERTEX
		uint32 vb_data_stride_ = {};
		uint32 vb_data_count_ = {};

		ComPtr<ID3D11Buffer> vertex_buffer_ = {};
#pragma endregion VERTEX

#pragma region INDEX
		DXGI_FORMAT ib_format_ = {};
		uint32 ib_data_stride_ = {};
		uint32 ib_data_count_ = {};
		D3D11_PRIMITIVE_TOPOLOGY ib_topology_ = {};
		ComPtr<ID3D11Buffer> index_buffer_ = {};

		std::vector<SubMesh> sub_meshes_ = {};
#pragma endregion INDEX
    };
}


