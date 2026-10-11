#include "Engine/Core/pch.h"
#include "MapScopeWriteDiscard.h"

#include <Engine/Core/DX11.h>

namespace engine
{
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