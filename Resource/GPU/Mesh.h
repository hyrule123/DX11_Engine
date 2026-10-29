#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Render/RenderTypes.h>

#include <Engine/Core/Geometry.h>

namespace engine
{
    class Mesh
		: public Resource
    {
		ENTITY_INFO(Mesh, Resource)

	public:
		struct DrawArgs
		{
			uint32_t instance_start;
			uint32_t instance_count;
			uint32_t submesh_index;
		};

		struct SubMesh
		{
			// 현재 SubMesh의 Index 시작 지점.
			uint32_t index_start = 0;

			// 현재 SubMesh의 Index 개수.
			uint32_t index_count = 0;
		};

		Mesh();
		virtual ~Mesh() override;

		bool IsReady() const { return (bool)vertex_buffer_ && (bool)index_buffer_; }
		void Bind(ID3D11DeviceContext* context) const;
		void Draw(ID3D11DeviceContext* context, DrawArgs args) const;

		virtual bool LoadFromFile(const std::filesystem::path& path) override;

		template <typename T>
		bool CreateVertexBuffer(std::span<const T> vertices, const AABB3D& local_bounds)
		{
			return CreateVertexBuffer(vertices.data(), (uint32_t)sizeof(T), (uint32_t)vertices.size(), local_bounds);
		}

		bool CreateIndexBuffer(std::span<const uint16_t> indices, D3D11_PRIMITIVE_TOPOLOGY topology, std::vector<SubMesh> sub_meshes = {}) {
			return CreateIndexBuffer((const void*)indices.data(), DXGI_FORMAT_R16_UINT, (uint32_t)indices.size(), topology, sub_meshes);
		}
		bool CreateIndexBuffer(std::span<const uint32_t> indices, D3D11_PRIMITIVE_TOPOLOGY topology, std::vector<SubMesh> sub_meshes = {}) {
			return CreateIndexBuffer((const void*)indices.data(), DXGI_FORMAT_R32_UINT, (uint32_t)indices.size(), topology, sub_meshes);
		}

		bool CreateIndexBuffer(const void* p_data, DXGI_FORMAT format, uint32_t data_count, D3D11_PRIMITIVE_TOPOLOGY topology, std::vector<SubMesh> sub_meshes = {});

		size_t GetSubMeshCount() const { return sub_meshes_.size(); }

		const AABB3D& GetLocalBounds() const { return local_bounds_; }

		uint32_t GetMeshID() const { return mesh_ID_.Get(); }

	private:
		bool CreateVertexBuffer(const void* data, uint32_t data_stride, uint32_t data_count, const AABB3D& bounds);

	private:
		MeshID mesh_ID_;    // RenderKey에 패킹되는 Mesh 고유 ID. ScopedID로 관리됨

#pragma region VERTEX
		uint32_t vb_data_stride_ = {};
		uint32_t vb_data_count_ = {};

		ComPtr<ID3D11Buffer> vertex_buffer_ = {};
#pragma endregion VERTEX

#pragma region INDEX
		DXGI_FORMAT ib_format_ = {};
		uint32_t ib_data_stride_ = {};
		uint32_t ib_data_count_ = {};
		D3D11_PRIMITIVE_TOPOLOGY ib_topology_ = {};
		ComPtr<ID3D11Buffer> index_buffer_ = {};

		std::vector<SubMesh> sub_meshes_ = {};
#pragma endregion INDEX

		AABB3D local_bounds_ = {};
    };
}


