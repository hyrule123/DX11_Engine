#include "Engine/Core/pch.h"
#include "MapScopeWriteDiscard.h"

#include <Engine/Core/DX11.h>

namespace engine
{
    MapScopeWriteDiscard::MapScopeWriteDiscard(ID3D11DeviceContext* ctx, ID3D11Buffer* buf, uint32 stride, uint32 capacity)
        : context_(ctx), buffer_(buf), stride_(stride), capacity_(capacity)
    {
        D3D11_MAPPED_SUBRESOURCE mapped = {};
        HRESULT hr = context_->Map(buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (FAILED(hr))
        {
            ERR_MSG_HRESULT(hr);
            mapped_ok_ = false;
        }
        else
        {
            p_data_ = static_cast<uint8*>(mapped.pData);
            mapped_ok_ = true;
        }
    }

    MapScopeWriteDiscard::~MapScopeWriteDiscard()
    {
        Release();
    }
    MapScopeWriteDiscard::MapScopeWriteDiscard(MapScopeWriteDiscard&& move) noexcept
        : context_(move.context_)
        , buffer_(move.buffer_)
        , stride_(move.stride_)
        , capacity_(move.capacity_)
        , p_data_(move.p_data_)
        , count_(move.count_)
        , mapped_ok_(move.mapped_ok_)
    {
        move.p_data_ = nullptr;
        move.count_ = 0;
        move.mapped_ok_ = false;
    }
    MapScopeWriteDiscard& MapScopeWriteDiscard::operator=(MapScopeWriteDiscard&& move) noexcept
    {
        if (this != &move)
        {
            Release();					// 내가 쥔 Map 먼저 해제

            context_ = move.context_;
            buffer_ = move.buffer_;
            stride_ = move.stride_;
            capacity_ = move.capacity_;
            p_data_ = move.p_data_;
            count_ = move.count_;
            mapped_ok_ = move.mapped_ok_;

            move.p_data_ = nullptr;		// 원본은 비움 (Unmap하지 않도록)
            move.count_ = 0;
            move.mapped_ok_ = false;
        }
        return *this;
    }
    void MapScopeWriteDiscard::Release() noexcept
    {
        if (mapped_ok_)
        {
            context_->Unmap(buffer_, 0);
        }
        p_data_ = nullptr;
        count_ = 0;
        mapped_ok_ = false;
    }
}