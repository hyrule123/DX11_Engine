#pragma once

#include <Engine/Resource/Resource.h>

struct ID3D11DeviceContext;

namespace engine
{
	class VertexBuffer;
	class IndexBuffer;

    class Mesh
		: public Resource
    {
		ENTITY_INFO(Mesh, Resource)
	public:
		Mesh();
		virtual ~Mesh() override;

		void Draw(ID3D11DeviceContext* context, size_t instance_count);

		virtual bool LoadFromFile(const stdfs::path& path) override;

		void SetVertexBuffer(s_ptr<VertexBuffer> vertex_buffer) {
			vertex_buffer_ = vertex_buffer;
		}
		void SetIndexBuffer(s_ptr<IndexBuffer> index_buffer) {
			index_buffer_ = index_buffer;
		}
		void SetBuffers(s_ptr<VertexBuffer> vertex_buffer, s_ptr<IndexBuffer> index_buffer) {
			SetVertexBuffer(vertex_buffer);
			SetIndexBuffer(index_buffer);
		}

	private:
		s_ptr<VertexBuffer> vertex_buffer_;
		s_ptr<IndexBuffer> index_buffer_;
    };
}


