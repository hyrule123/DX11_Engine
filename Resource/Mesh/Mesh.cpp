#include "Engine/Core/pch.h"
#include "Mesh.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Resource/Mesh/VertexBuffer.h>
#include <Engine/Resource/Mesh/IndexBuffer.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	Mesh::Mesh()
		: Resource(STRINGIFY(Mesh))
	{}
	Mesh::~Mesh()
	{}

	void Mesh::Render()
	{
		if (!(vertex_buffer_ && index_buffer_))
		{
			ERROR_MESSAGE("뭔가 부족함");
			return;
		}

		auto context = GraphicsDevice::GetInst().GetContext();

		vertex_buffer_->Bind(context);
		index_buffer_->Bind(context);


		context->DrawIndexed(index_buffer_->GetIndexCount(), 0, 0);
	}

	bool Mesh::LoadFromFile(const stdfs::path& path)
	{
		
		//아직은... 미구현(차후 fbx 임포트 시 구현 예정)
		return false;
	}
}

