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

	bool Texture2DArray::Slice(uint32 row_count, uint32 col_count)
	{
        auto device = GraphicsDevice::GetInst().GetDevice();
        auto context = GraphicsDevice::GetInst().GetContext();

		ComPtr<ID3D11Texture2D> atlas_tex = GetTexture2D();
        if (!atlas_tex)
        {
            ERROR_MESSAGE("이미지를 먼저 로드하세요");
            return false;
        }

        // 1. 원본 아틀라스의 정보(포맷 등)를 가져옵니다.
        D3D11_TEXTURE2D_DESC atlas_desc;
        atlas_tex->GetDesc(&atlas_desc);

        // 가로/세로 프레임 개수 및 총 배열 크기 계산
        UINT frame_width = atlas_desc.Width / (UINT)col_count;
        UINT frame_height = atlas_desc.Height / (UINT)row_count;
        frame_count_ = row_count * col_count;
        row_count_ = row_count;
        col_count_ = col_count;

        // 2. 비어있는 Texture2DArray를 생성합니다. (초기 데이터 없이 빈 공간만 할당)
        D3D11_TEXTURE2D_DESC sprite_desc = {};
        sprite_desc.Width = frame_width;
        sprite_desc.Height = frame_height;
        sprite_desc.MipLevels = 1;                 // 밉맵 없음
        sprite_desc.ArraySize = frame_count_;         // 프레임 개수만큼 층 생성
        sprite_desc.Format = atlas_desc.Format;     // 원본 아틀라스와 동일한 픽셀 포맷
        sprite_desc.SampleDesc.Count = 1;
        sprite_desc.Usage = D3D11_USAGE_DEFAULT;   // GPU가 읽고 쓸 수 있는 기본 사용법
        sprite_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        ComPtr<ID3D11Texture2D> sprite_tex = nullptr;
        HRESULT hr = device->CreateTexture2D(&sprite_desc, nullptr, &sprite_tex);
        if (FAILED(hr))
        {
            HRESULT_ERROR_MESSAGE(hr);
            return false;
        }

        // 3. GPU 명령어(Context)를 통해 아틀라스의 영역을 잘라서 Array로 복사합니다.
        for (UINT i = 0; i < frame_count_; ++i)
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

        ComPtr<ID3D11ShaderResourceView> srv = nullptr;
        hr = device->CreateShaderResourceView(sprite_tex.Get(), nullptr, &srv);

        if (FAILED(hr))
        {
            HRESULT_ERROR_MESSAGE(hr);
            return false;
        }

        SetTexture2D(sprite_tex);
        SetSRV(srv);

		return true;
	}

}