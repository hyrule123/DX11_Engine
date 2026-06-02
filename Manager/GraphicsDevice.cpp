#include "Engine/pch.h"
#include "GraphicsDevice.h"

#include <Engine/Constant.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace engine
{
	GraphicsDevice::GraphicsDevice()
	{}
	GraphicsDevice::~GraphicsDevice()
	{}

	bool GraphicsDevice::Init()
	{
		// Device, Device Context
		UINT DeviceFlag{};
#ifdef _DEBUG
		DeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

		D3D_FEATURE_LEVEL MaxSupportedFeatureLevel = D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL FeatureLevels[] = {
			D3D_FEATURE_LEVEL_11_0,
			//D3D_FEATURE_LEVEL_10_1,
			//D3D_FEATURE_LEVEL_10_0,
			//D3D_FEATURE_LEVEL_9_3,
			//D3D_FEATURE_LEVEL_9_2,
			//D3D_FEATURE_LEVEL_9_1
		};

		HRESULT result = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			DeviceFlag,
			FeatureLevels,
			sizeof(FeatureLevels) / sizeof(D3D_FEATURE_LEVEL),
			D3D11_SDK_VERSION,
			&device_,
			&MaxSupportedFeatureLevel,
			&context_);

		if (FAILED(result))
		{
			ASSERT_RELEASE_MESSAGE(false, "Failed to create D3D11 Device and Device Context!");
			return false;
		}

		return true;
	}
}