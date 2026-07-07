#include "Engine/Core/pch.h"
#include "Mesh.h"

#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/Graphics/Buffer/VertexBuffer.h>
#include <Engine/Resource/Graphics/Buffer/IndexBuffer.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/Debug.h>

namespace engine
{
	Mesh::Mesh()
		: Resource(STRINGIFY(Mesh))
	{}
	Mesh::~Mesh()
	{}

	void Mesh::Draw(ID3D11DeviceContext* context, size_t instance_count)
	{
		if (!(vertex_buffer_ && index_buffer_))
		{
			ERROR_MESSAGE("뭔가 부족함");
			return;
		}

		vertex_buffer_->Bind(context);
		index_buffer_->Bind(context);

		context->DrawIndexedInstanced(index_buffer_->GetIndexCount(), (UINT)instance_count, 0, 0, 0);
	}

	bool Mesh::LoadFromFile(const stdfs::path& path)
	{
		
		//아직은... 미구현(차후 fbx 임포트 시 구현 예정)
		return false;
	}
}

