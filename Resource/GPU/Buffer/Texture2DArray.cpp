#include "Engine/Core/pch.h"
#include "Texture2DArray.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/Debug.h>
#include <Engine/Core/DX11.h>

#include <DirectXTex/Include/DirectXTex.h>

namespace engine
{
	Texture2DArray::Texture2DArray()
		: Super(Texture2DArray::kClassConcreteName)
	{}
	Texture2DArray::~Texture2DArray()
	{}

    bool Texture2DArray::CreateImmutableFromMemory(uint32_2 tex_size, uint32 tex_count, DXGI_FORMAT format, std::span<const uint8> data, uint32 row_pitch, uint32 slice_pitch)
    {
        ASSERT(tex_count > 0);
        ASSERT(tex_size.x > 0 && tex_size.y > 0);
        ASSERT((size_t)row_pitch * tex_size.y <= (size_t)slice_pitch);

        auto device = GraphicsDevice::GetInst().GetDevice();

		const size_t total_byte_size = (size_t)tex_count * (size_t)slice_pitch;
        if (data.size() != total_byte_size)
        {
			ERR_MSG("data size mismatch: expected {}, actual {}", total_byte_size, data.size());
            return false;
        }

        // 2. 비Texture2DArray를 생성합니다.
        D3D11_TEXTURE2D_DESC sprite_desc = {};
        sprite_desc.Width = tex_size.x;
        sprite_desc.Height = tex_size.y;
        sprite_desc.MipLevels = 1;                 // 밉맵 없음
        sprite_desc.ArraySize = tex_count;         // 프레임 개수만큼 층 생성
        sprite_desc.Format = format;
        sprite_desc.SampleDesc.Count = 1;
        sprite_desc.Usage = D3D11_USAGE_IMMUTABLE;   // 이미 만들어진 데이터가 있음 -> IMMUTABLE
        sprite_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        std::vector<D3D11_SUBRESOURCE_DATA> initial_data;
        initial_data.resize(tex_count);
        for (uint32 i = 0; i < tex_count; ++i)
        {
            initial_data[i].pSysMem = data.data() + (size_t)i * (size_t)slice_pitch;
            initial_data[i].SysMemPitch = row_pitch;
            initial_data[i].SysMemSlicePitch = 0;  // 2D 배열에서는 사용되지 않음
        }

        ComPtr<ID3D11Texture2D> sprite_tex = nullptr;
        HRESULT hr = device->CreateTexture2D(&sprite_desc, initial_data.data(), sprite_tex.GetAddressOf());
        if (FAILED(hr))
        {
            ERR_MSG_HRESULT(hr);
            return false;
        }


        // nullptr을 그대로 넘기면 Array Size == 1일 경우 Array가 아닌 단순 텍스처 2D가 생성됨!
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Format = format;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srv_desc.Texture2DArray.MostDetailedMip = 0;
        srv_desc.Texture2DArray.MipLevels = 1;
        srv_desc.Texture2DArray.FirstArraySlice = 0;
        srv_desc.Texture2DArray.ArraySize = tex_count;

        ComPtr<ID3D11ShaderResourceView> srv = nullptr;
        hr = device->CreateShaderResourceView(sprite_tex.Get(), &srv_desc, srv.GetAddressOf());

        if (FAILED(hr))
        {
            ERR_MSG_HRESULT(hr);
            return false;
        }

        SetTexture2D(sprite_tex);
        SetSRV(srv);

        // 가로/세로 프레임 개수 및 총 배열 크기 계산
        frame_count_ = tex_count;
        row_count_ = 1;
        col_count_ = tex_count;

        return true;
    }

	bool Texture2DArray::Slice(uint32 row_count, uint32 col_count)
	{
        auto device = GraphicsDevice::GetInst().GetDevice();
        auto context = GraphicsDevice::GetInst().GetContext();

		ComPtr<ID3D11Texture2D> atlas_tex = GetTexture2D();
        if (!atlas_tex)
        {
            ERR_MSG("이미지를 먼저 로드하세요");
            return false;
        }

        // 1. 원본 아틀라스의 정보(포맷 등)를 가져옵니다.
        D3D11_TEXTURE2D_DESC atlas_desc;
        atlas_tex->GetDesc(&atlas_desc);

        // 가로/세로 프레임 개수 및 총 배열 크기 계산
        UINT frame_width = atlas_desc.Width / (UINT)col_count;
        UINT frame_height = atlas_desc.Height / (UINT)row_count;
        uint32 frame_count = (uint32)((size_t)row_count * (size_t)col_count);

        // 2. 비어있는 Texture2DArray를 생성합니다. (초기 데이터 없이 빈 공간만 할당)
        D3D11_TEXTURE2D_DESC sprite_desc = {};
        sprite_desc.Width = frame_width;
        sprite_desc.Height = frame_height;
        sprite_desc.MipLevels = 1;                 // 밉맵 없음
        sprite_desc.ArraySize = frame_count;         // 프레임 개수만큼 층 생성
        sprite_desc.Format = atlas_desc.Format;     // 원본 아틀라스와 동일한 픽셀 포맷
        sprite_desc.SampleDesc.Count = 1;
        sprite_desc.Usage = D3D11_USAGE_DEFAULT;   // GPU가 읽고 쓸 수 있는 기본 사용법 (쓰기 가능 - 빈 공간을 만들어 놓고 채워야 하므로)
        sprite_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        ComPtr<ID3D11Texture2D> sprite_tex = nullptr;
        HRESULT hr = device->CreateTexture2D(&sprite_desc, nullptr, sprite_tex.GetAddressOf());
        if (FAILED(hr))
        {
            ERR_MSG_HRESULT(hr);
            return false;
        }

        // 3. GPU 명령어(Context)를 통해 아틀라스의 영역을 잘라서 Array로 복사합니다.
        for (UINT i = 0; i < frame_count; ++i)
        {
            UINT col = i % col_count;
            UINT row = i / col_count;

            // 원본 아틀라스에서 잘라낼 사각형 영역(Box) 정의
            D3D11_BOX src_box_region;
            src_box_region.left = col * frame_width;
            src_box_region.right = src_box_region.left + frame_height;
            src_box_region.top = row * frame_height;
            src_box_region.bottom = src_box_region.top + frame_height;
            src_box_region.front = 0;
            src_box_region.back = 1; // 2D 텍스처이므로 깊이는 1

            // 목적지(Array)의 서브리소스 인덱스 계산 (Mip 0번, i번째 슬라이스)
            UINT dest = D3D11CalcSubresource(0, i, sprite_desc.MipLevels);

            // 소스(Atlas)의 서브리소스 인덱스 (Mip 0번, 0번째 슬라이스)
            UINT src = 0;

            // 핵심 함수: 아틀라스의 특정 영역을 배열의 i번째 층에 복사!
            context->CopySubresourceRegion(
                sprite_tex.Get(),          // 복사될 목적지 (Array)
                dest,               // 목적지의 몇 번째 층인가?
                0, 0, 0,           // 목적지의 X, Y, Z 시작 좌표 (0,0부터 채움)
                atlas_tex.Get(),    // 복사할 원본 (Atlas)
                src,                // 원본의 어떤 서브리소스인가?
                &src_box_region            // 원본에서 잘라낼 영역
            );
        }

        // nullptr을 그대로 넘기면 Array Size == 1일 경우 Array가 아닌 단순 텍스처 2D가 생성됨!
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Format = sprite_desc.Format;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srv_desc.Texture2DArray.MostDetailedMip = 0;
        srv_desc.Texture2DArray.MipLevels = 1;
        srv_desc.Texture2DArray.FirstArraySlice = 0;
        srv_desc.Texture2DArray.ArraySize = frame_count;
        ComPtr<ID3D11ShaderResourceView> srv = nullptr;
        hr = device->CreateShaderResourceView(sprite_tex.Get(), &srv_desc, srv.GetAddressOf());

        if (FAILED(hr))
        {
            ERR_MSG_HRESULT(hr);
            return false;
        }

        SetTexture2D(sprite_tex);
        SetSRV(srv);

		frame_count_ = frame_count;
		row_count_ = row_count;
		col_count_ = col_count;

		return true;
	}

}