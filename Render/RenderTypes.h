#pragma once

#include <Engine/Collision/Collision.h>	//AABB2D

#include <Engine/Core/Enum.h>
#include <Engine/Core/Debug.h>

#include <Engine/Util/IDAllocator.h>

namespace engine
{
	class Renderer;
	struct RendererInfo2D
	{
		Renderer* renderer;
		AABB2D world_bounds;
		bool is_dirty;
	};

	struct RenderPassContext
	{
		AABB2D view_bounds_2d;
	};

	enum class RenderPassOrder : uint32
	{
		ForwardOpaque = 0u,
		Present,
		kCount
	};
	using RenderPassFlags = EnumFlags<RenderPassOrder, uint32>;
	// Flags<RenderPassOrder>

	// ---------------------------------------------------------------------------
	// RenderKey 대응 별칭 — Tag는 전방 선언만으로 충분하다.
	//
	// 비트 폭은 이곳이 유일한 출처다. 패킹에 쓰이는 시프트/마스크는
	// RenderKey 쪽에서 이 상수로부터 유도하며, 어디에도 리터럴을 두지 않는다.
	// ---------------------------------------------------------------------------
	class Material;
	class Mesh;

	constexpr uint32 kMaterialIDBits = 20;
	constexpr uint32 kMeshIDBits = 20;
	constexpr uint32 kSubMeshIndexBits = 8;
	constexpr uint32 kMaxSubMeshCount = 1u << kSubMeshIndexBits;

	using MaterialID = ScopedID<Material, kMaterialIDBits>;
	using MeshID = ScopedID<Mesh, kMeshIDBits>;

	struct RenderKey
	{
		static_assert(kMaterialIDBits + kMeshIDBits + kSubMeshIndexBits <= 64,
			"RenderKey layout overflows uint64.");

		RenderKey() = default;
		RenderKey(MaterialID material_id, MeshID mesh_id, uint8 submesh_slot)
			: key(
				(uint64)material_id.Get() << kMaterialIDShift
				| 
				(uint64)mesh_id.Get() << kMeshIDShift
				| 
				(uint64)submesh_slot << kSubMeshIndexShift)
		{
		}

		uint32 GetMaterialID() const { return (uint32)((key >> kMaterialIDShift) & kMaterialIDMask); }
		uint32 GetMeshID() const { return (uint32)((key >> kMeshIDShift) & kMeshIDMask); }
		uint8 GetSubMeshIndex() const { return (uint8)((key >> kSubMeshIndexShift) & kSubMeshIndexMask); }

		uint64 key = 0;

		auto operator<=>(const RenderKey& other) const = default;
		bool operator==(const RenderKey& other) const = default;

		static constexpr uint32 kSubMeshIndexShift = 0;
		static constexpr uint32 kMeshIDShift = kSubMeshIndexShift + kSubMeshIndexBits;
		static constexpr uint32 kMaterialIDShift = kMeshIDShift + kMeshIDBits;

		static constexpr uint64 kMeshIDMask = ((uint64)1 << kMeshIDBits) - 1;
		static constexpr uint64 kMaterialIDMask = ((uint64)1 << kMaterialIDBits) - 1;
		static constexpr uint64 kSubMeshIndexMask = ((uint64)1 << kSubMeshIndexBits) - 1;
	};

	struct RenderKeyHasher
	{
		size_t operator()(const RenderKey& key) const
		{
			return std::hash<uint64>()(key.key);
		}
	};

	struct SubMeshRenderData
	{
		RenderKey key;
		RenderPassFlags pass_flags;
	};
}