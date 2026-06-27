#include "Engine/Core/pch.h"
#include "SpriteTextureArray.h"

#include <DirectXTex/Include/DirectXTex.h>

namespace engine
{
	SpriteTextureArray::SpriteTextureArray()
		: Super(STRINGIFY(SpriteTextureArray))
	{}
	SpriteTextureArray::~SpriteTextureArray()
	{}
	bool SpriteTextureArray::LoadFromFile(const stdfs::path & res_path)
	{
		//Texture2D의 로딩 과정 의도적으로 건너뜀
		if (false == Resource::LoadFromFile(res_path))
		{
			return false;
		}

        /*
        // 0. 원본 아틀라스 이미지 정보 가져오기
        const DirectX::Image* src_img = atlasScratch.GetImage(0, 0, 0); // 가작 첫 번째 밉맵, 첫 이미지
        if (!src_img) return nullptr;

        size_t numCols = src_img->width / frameWidth;
        size_t numRows = src_img->height / frameHeight;
        size_t arraySize = numCols * numRows; // 총 프레임(슬라이스) 개수

        // 1. 새로운 텍스처 배열용 ScratchImage 초기화
        // 가로, 세로, 배열 크기(arraySize), 밉맵 수(우선 1개로 생성 후 나중에 생성)
        ScratchImage arrayScratch;
        HRESULT hr = arrayScratch.Initialize2D(
            src_img->format,
            frameWidth,
            frameHeight,
            arraySize,
            1
        );
        if (FAILED(hr)) return nullptr;

        // 포맷에 따른 픽셀당 바이트 수 계산 (예: DXGI_FORMAT_R8G8B8A8_UNORM은 4바이트)
        size_t bpp = BitsPerPixel(src_img->format) / 8;

        // 2. CPU 메모리 상에서 아틀라스 쪼개서 복사하기
        for (size_t i = 0; i < arraySize; ++i)
        {
            size_t col = i % numCols;
            size_t row = i / numCols;

            // 목적지가 될 배열의 i번째 슬라이스(Image)를 가져옴
            const Image* destImg = arrayScratch.GetImage(0, i, 0); // mip=0, item=i, slice=0

            // 픽셀을 가로 한 줄(Row)씩 복사
            for (size_t y = 0; y < frameHeight; ++y)
            {
                // 소스 아틀라스에서의 절대 Y, X 좌표 계산
                size_t srcX = col * frameWidth;
                size_t srcY = row * frameHeight + y;

                // 소스 및 목적지 주소 계산
                uint8_t* srcPtr = src_img->pixels + (srcY * src_img->rowPitch) + (srcX * bpp);
                uint8_t* destPtr = destImg->pixels + (y * destImg->rowPitch);

                // 한 줄의 가로 길이만큼 픽셀 데이터 복사
                memcpy(destPtr, srcPtr, frameWidth * bpp);
            }
        }

        // 3. (강력 추천) 텍스처 배열의 밉맵 생성
        // 텍스처 배열 상태에서 밉맵을 만들면 각 슬라이스 독립적으로 밉맵이 계산되므로 블리딩이 원천 차단됩니다.
        ScratchImage mipchainScratch;
        hr = GenerateMipMaps(
            arrayScratch.GetImages(),
            arrayScratch.GetImageCount(),
            arrayScratch.GetMetadata(),
            TEX_FILTER_DEFAULT,
            0, // 0을 주면 가능한 끝까지 밉맵 생성
            mipchainScratch
        );

        // 밉맵 생성이 성공하면 밉맵이 포함된 ScratchImage를 사용하고, 실패하면 원본 배열 사용
        const ScratchImage& finalScratch = SUCCEEDED(hr) ? mipchainScratch : arrayScratch;

        // 4. Direct3D 11 리소스(Shader Resource View) 생성
        ID3D11ShaderResourceView* srv = nullptr;
        hr = CreateShaderResourceView(
            device,
            finalScratch.GetImages(),
            finalScratch.GetImageCount(),
            finalScratch.GetMetadata(),
            &srv
        );

        if (FAILED(hr)) return nullptr;

        return srv; // 성공적으로 생성된 Texture2DArray의 SRV 반환
        */

		return false;
	}
}