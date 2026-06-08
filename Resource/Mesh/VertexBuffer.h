#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Header/Common.h>

struct ID3D11Buffer;

namespace engine
{
    class VertexBuffer :
        public Resource
    {
	public:
		VertexBuffer();
		virtual ~VertexBuffer() override;

		bool Create(const void* data, UINT data_stride, UINT data_count);
        template <typename T>
        bool Create(const std::vector<T>& vertices)
        {
            return Create(vertices.data(), (UINT)sizeof(T), (UINT)vertices.size());
        }

		virtual bool LoadFromFile(const stdfs::path& path) override;

        void Bind(const ComPtr<ID3D11DeviceContext>& context);

	private:
        //Vertex 구조체는 언제든지 달라질수 있음 -> 통합 저장을 위해 unsigned char(byte) 형태로 저장.
        std::vector<unsigned char> data_ = {};
        UINT data_stride_ = {};
        UINT data_count_ = {};

        ComPtr<ID3D11Buffer> buffer_ = {};
    };
}


