#include "Engine/Core/pch.h"
#include "RenderTargetPool.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Resource/GPU/Buffer/RenderTarget.h>
#include <Engine/Resource/GPU/Buffer/DepthStencilTarget.h>

#include <Engine/Core/Debug.h>

#include <algorithm>
#include <cmath>

namespace engine
{
	RenderTargetPool::Entry::Entry()
		: kind(Kind::Color)
		, color_desc()
		, depth_desc()
		, unused_frames(0)
		, texture()
	{}

	RenderTargetPool::RenderTargetPool()
		: entries_()
	{}

	RenderTargetPool::~RenderTargetPool()
	{}

	std::shared_ptr<RenderTarget> RenderTargetPool::AcquireColor(const HashedStringView& name, const ColorTargetDesc& desc)
	{
		bool failed = false;
		if (Entry* entry = FindExisting(name, Kind::Color, failed))
		{
			if (false == ENSURE_F(entry->color_desc == desc, "RenderTargetPool: '{}' desc 불일치", name.GetStringView()))
			{
				return nullptr;
			}

			entry->unused_frames = 0;
			return std::static_pointer_cast<RenderTarget>(entry->texture);
		}

		if (failed)
		{
			return nullptr;
		}

		std::shared_ptr<RenderTarget> rtv = CreateColor(desc);
		if (rtv == nullptr)
		{
			ERR_MSG("RenderTargetPool: '{}' 생성 실패", name.GetStringView());
			return nullptr;
		}

		Entry entry;
		entry.kind = Kind::Color;
		entry.color_desc = desc;
		entry.texture = rtv;
		entries_.insert(name, std::move(entry));

		return rtv;
	}

	std::shared_ptr<DepthStencilTarget> RenderTargetPool::AcquireDepth(const HashedStringView& name, const DepthTargetDesc& desc)
	{
		bool failed = false;
		if (Entry* entry = FindExisting(name, Kind::Depth, failed))
		{
			if (false == ENSURE_F(entry->depth_desc == desc, "RenderTargetPool: '{}' desc 불일치", name.GetStringView()))
			{
				return nullptr;
			}

			entry->unused_frames = 0;
			return std::static_pointer_cast<DepthStencilTarget>(entry->texture);
		}

		if (failed)
		{
			return nullptr;
		}

		std::shared_ptr<DepthStencilTarget> dsv = CreateDepth(desc);
		if (dsv == nullptr)
		{
			ERR_MSG("RenderTargetPool: '{}' 생성 실패", name.GetStringView());
			return nullptr;
		}

		Entry entry;
		entry.kind = Kind::Depth;
		entry.depth_desc = desc;
		entry.texture = dsv;
		entries_.insert(name, std::move(entry));

		return dsv;
	}

	void RenderTargetPool::FrameEnd()
	{
		auto& cont = entries_.cont;
		for (auto it = cont.begin(); it != cont.end(); )
		{
			Entry& entry = it->second;

			// 풀 밖에서 누군가 들고 있음 = 사용 중
			if (entry.texture.use_count() > 1)
			{
				entry.unused_frames = 0;
				++it;
				continue;
			}

			++entry.unused_frames;
			if (entry.unused_frames < kReleaseAfterUnusedFrames)
			{
				++it;
				continue;
			}

			DEBUG_LOG("RenderTargetPool: '{}' 해제 ({}x{}, {}프레임 미사용)",
				it->first, entry.texture->GetWidth(), entry.texture->GetHeight(), entry.unused_frames);
			it = cont.erase(it);
		}
	}

	void RenderTargetPool::OnScreenSizeChange(uint32_t width, uint32_t height)
	{
		for (auto& [name, entry] : entries_.cont)
		{
			const RenderTargetSize& size = GetSize(entry);
			if (size.base != RenderTargetSizeBase::SwapChain)
			{
				continue;
			}

			uint32_t new_width = 0;
			uint32_t new_height = 0;
			if (false == ResolveSize(size, new_width, new_height))
			{
				continue;
			}

			if (new_width == entry.texture->GetWidth() && new_height == entry.texture->GetHeight())
			{
				continue;
			}

			// 객체는 유지하고 내부 텍스처/뷰만 교체 (발급된 shared_ptr 유효)
			bool result = entry.texture->Resize(new_width, new_height);
			ASSERT_F(result, "RenderTargetPool: '{}' Resize 실패", name);
		}
	}

	RenderTargetPool::Entry* RenderTargetPool::FindExisting(const HashedStringView& name, Kind kind, bool& out_failed)
	{
		out_failed = false;

		auto it = entries_.find(name);
		if (it == entries_.end())
		{
			return nullptr;
		}

		Entry& entry = it->second;
		if (false == ENSURE_F(entry.kind == kind, "RenderTargetPool: '{}' 종류 불일치 (Color/Depth)", name.GetStringView()))
		{
			out_failed = true;
			return nullptr;
		}

		return &entry;
	}

	std::shared_ptr<RenderTarget> RenderTargetPool::CreateColor(const ColorTargetDesc& desc)
	{
		uint32_t width = 0;
		uint32_t height = 0;
		if (false == ResolveSize(desc.size, width, height))
		{
			return nullptr;
		}

		D3D11_TEXTURE2D_DESC tex_desc = {};
		tex_desc.Width = width;
		tex_desc.Height = height;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = desc.format;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
		if (desc.create_srv)
		{
			tex_desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}
		if (desc.create_uav)
		{
			tex_desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		}

		std::shared_ptr<RenderTarget> rtv = EntityManager::CreateEntity<RenderTarget>();

		// BindFlags에 따라 기본 SRV/UAV도 함께 생성됨
		if (false == rtv->CreateTexture2D(&tex_desc))
		{
			return nullptr;
		}
		if (false == rtv->CreateRTV(nullptr))
		{
			return nullptr;
		}

		return rtv;
	}

	std::shared_ptr<DepthStencilTarget> RenderTargetPool::CreateDepth(const DepthTargetDesc& desc)
	{
		uint32_t width = 0;
		uint32_t height = 0;
		if (false == ResolveSize(desc.size, width, height))
		{
			return nullptr;
		}

		D3D11_TEXTURE2D_DESC tex_desc = {};
		tex_desc.Width = width;
		tex_desc.Height = height;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = desc.format;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		std::shared_ptr<DepthStencilTarget> dsv = EntityManager::CreateEntity<DepthStencilTarget>();
		if (false == dsv->CreateTexture2D(&tex_desc))
		{
			return nullptr;
		}
		if (false == dsv->CreateDSV(nullptr))
		{
			return nullptr;
		}

		return dsv;
	}

	bool RenderTargetPool::ResolveSize(const RenderTargetSize& size, uint32_t& out_width, uint32_t& out_height)
	{
		switch (size.base)
		{
		case RenderTargetSizeBase::Fixed:
		{
			out_width = size.width;
			out_height = size.height;
			break;
		}
		case RenderTargetSizeBase::SwapChain:
		{
			const GraphicsDevice& device = GraphicsDevice::GetInst();
			out_width = std::max(1u, (uint32_t)std::lround(device.GetResolutionWidth() * size.scale));
			out_height = std::max(1u, (uint32_t)std::lround(device.GetResolutionHeight() * size.scale));
			break;
		}
		default:
		{
			ASSERT_F(false, "RenderTargetPool: 알 수 없는 RenderTargetSizeBase");
			return false;
		}
		}

		return ENSURE_F(out_width > 0 && out_height > 0, "RenderTargetPool: 크기가 0");
	}

	const RenderTargetSize& RenderTargetPool::GetSize(const Entry& entry)
	{
		return (entry.kind == Kind::Color) ? entry.color_desc.size : entry.depth_desc.size;
	}
}