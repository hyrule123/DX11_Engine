#pragma once

#include <Engine/Math/SimpleMath.h>
#include <Engine/Resource/Resource.h>

namespace engine
{
	class VertexBuffer;
	class IndexBuffer;

    class Mesh
		: public Resource
    {
	public:
		Mesh();
		virtual ~Mesh() override;

		void Render();

		virtual bool LoadFromFile(const stdfs::path& path) override;

		inline void SetVertexBuffer(s_ptr<VertexBuffer> vertex_buffer) 
		{
			vertex_buffer_ = vertex_buffer;
		}
		inline void SetIndexBuffer(s_ptr<IndexBuffer> index_buffer)
		{
			index_buffer_ = index_buffer;
		}
		inline void SetBuffers(s_ptr<VertexBuffer> vertex_buffer, s_ptr<IndexBuffer> index_buffer)
		{
			SetVertexBuffer(vertex_buffer);
			SetIndexBuffer(index_buffer);
		}

	private:
		s_ptr<VertexBuffer> vertex_buffer_;
		s_ptr<IndexBuffer> index_buffer_;
    };
}


