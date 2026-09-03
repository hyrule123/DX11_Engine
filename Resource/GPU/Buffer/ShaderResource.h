#pragma once
#include <Engine/Core/DX11.h>
#include <Engine/Core/SmartPointer.h>

// 공통 기반 클래스
// SRV를 사용하고자 하는 클래스 공통
namespace engine
{
	class ShaderResource // 인터페이스가 아니라 공통 기반 
	{
	public:
		virtual ~ShaderResource() = default;
		ID3D11ShaderResourceView* GetSRV() const { return SRV_.Get(); } // non-virtual 
	protected:
		ComPtr<ID3D11ShaderResourceView> SRV_ = {};
	};
}