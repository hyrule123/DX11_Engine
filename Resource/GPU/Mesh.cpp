#include "Engine/Core/pch.h"
#include "Mesh.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/Debug.h>
#include <Engine/Core/Constant.h>

namespace engine
{
	Mesh::Mesh()
		: Resource(Mesh::kClassConcreteName)
		, mesh_ID_()	// 기본 생성자에서 자동 발급됨(RAII)
	{}
	Mesh::~Mesh()
	{}

	void Mesh::Bind(ID3D11DeviceContext* context)
	{
		if (false == IsReady())
		{
			DEBUG_MESSAGE("Mesh가 준비되지 않았습니다.");
			return;
		}

		uint32_t offset = 0;
		// 0번 슬롯에 1개의 버퍼를 바인딩합니다.
		context->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), &vb_data_stride_, &offset);

		context->IASetPrimitiveTopology(ib_topology_);
		context->IASetIndexBuffer(index_buffer_.Get(), ib_format_, 0);
	}

	void Mesh::Draw(ID3D11DeviceContext* context, uint32 instance_count, uint32 submesh_index)
	{
		ASSERT(IsReady());
		ASSERT(submesh_index < (UINT)sub_meshes_.size());

		const SubMesh& sub_mesh = sub_meshes_[submesh_index];
		context->DrawIndexedInstanced((UINT)sub_mesh.index_count, (UINT)instance_count, (UINT)sub_mesh.index_start, 0, 0);
	}

	bool Mesh::LoadFromFile(const stdfs::path& path)
	{
		ASSERT(false && "아직은... 미구현(차후 fbx 임포트 시 구현 예정)");
		return false;
	}

	bool Mesh::CreateVertexBuffer(const void* data, uint32 data_stride, uint32 data_count)
	{
		if (nullptr == data)
		{
			return false;
		}

		D3D11_BUFFER_DESC desc = {};
		// 버텍스 버퍼
		desc.ByteWidth = data_stride * data_count;
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
		desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sub_data = {};
		sub_data.pSysMem = data;

		auto* device = GraphicsDevice::GetInst().GetDevice();
		ComPtr<ID3D11Buffer> vb = nullptr;
		HRESULT hr = device->CreateBuffer(&desc, &sub_data, vb.GetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		vertex_buffer_ = vb;
		vb_data_stride_ = data_stride;
		vb_data_count_ = data_count;

		return true;
	}

	bool Mesh::CreateIndexBuffer(const void* p_data, DXGI_FORMAT format, uint32 data_count, D3D11_PRIMITIVE_TOPOLOGY topology, std::vector<SubMesh> sub_meshes)
	{
		UINT data_stride = GetDXGIFormatByteStride(format);

		if (data_stride == 0)
		{
			ERROR_MESSAGE("지원하지 않는 DXGI_FORMAT입니다.");
			return false;
		}
		if (data_count == 0)
		{
			ERROR_MESSAGE("인덱스 데이터 개수가 0입니다.");
			return false;
		}

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = (UINT)data_stride * (UINT)data_count;
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
		desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;

		//인덱스 버퍼 데이터 복사
		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = p_data;

		auto* device = GraphicsDevice::GetInst().GetDevice();
		ComPtr<ID3D11Buffer> ib = nullptr;
		HRESULT hr = device->CreateBuffer(&desc, &data, ib.GetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		// SubMesh가 비어있으면 전체 범위를 하나의 SubMesh로 추가
		if (sub_meshes.empty())
		{
			DEBUG_LOG("SubMesh가 비어있습니다. 전체 범위를 하나의 SubMesh로 추가합니다.");
			// SubMesh가 비어있으면 전체 범위를 하나의 SubMesh로 추가
			sub_meshes.push_back({ 0, data_count });
		}
		else
		{
			// 비어있지 않다면 SubMesh 검증
			if(sub_meshes.size() > kMaxSubMeshCount)
			{
				ERROR_MESSAGE("SubMesh 개수가 최대치를 초과했습니다.");
				return false;
			}
			for (const SubMesh& sm : sub_meshes)
			{
				if (sm.index_start + sm.index_count > data_count)
				{
					ERROR_MESSAGE("SubMesh 범위가 인덱스 버퍼를 벗어납니다.");
					return false;
				}
			}
		}

		index_buffer_ = ib;
		ib_format_ = format;
		ib_topology_ = topology;
		sub_meshes_ = std::move(sub_meshes);

		return true;
	}
}

