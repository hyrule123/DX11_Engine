#pragma once

#include <Engine/Collision/Collision.h>	//AABB2D

#include <Engine/Core/Enum.h>
#include <Engine/Core/Debug.h>

#include <Engine/Util/IDAllocator.h>

struct ID3D11DeviceContext;

namespace engine
{
	class Renderer;
	struct RendererInfo2D
	{
		Renderer* renderer;
		AABB2D world_bounds;
		bool is_bounds_dirty;
	};

	class RenderView;
	class RendererRegistry;
	class InstanceBufferPool;

	// 패스 실행에 필요한 프레임 단위 입력. RenderManager가 매 프레임 구성해 넘긴다.
	// 패스는 RenderManager(싱글턴)에 직접 접근하지 않고 이것만 사용한다.
	struct RenderContext
	{
		ID3D11DeviceContext* dc;
		const RenderView& view;					// 메인 카메라: 행렬, 출력 대상, 뷰포트
		const RendererRegistry& registry;       // renderer_id → Renderer* 조회
		InstanceBufferPool& instance_buffers;   // stride별 인스턴스 버퍼
	};

	namespace render_pass
	{
		enum class ID : uint32
		{
			// 내장 패스
			ForwardOpaque = 0, ForwardBlend, Present,

			// 커스텀 예약 슬롯 (콘텐츠에서 이름을 붙여 사용)
			Custom0, Custom1, Custom2, Custom3, Custom4, Custom5, Custom6, Custom7,
			Custom8, Custom9, Custom10, Custom11, Custom12,

			kCount
		};

		static_assert((size_t)ID::kCount < 32, "render_pass::ID can't exceed 32");

		ENABLE_ENUM_FLAGS(Flags, ID, uint32);
	}

	enum class ShaderStage : uint8
	{
		Vertex = 0,
		Geometry,
		Pixel,
		Compute,
		kCount
	};
	ENABLE_ENUM_FLAGS(ShaderStageFlags, ShaderStage, uint8);
	constexpr ShaderStageFlags kShaderStageAllFlags = ShaderStageFlags::Full();
	constexpr ShaderStageFlags kShaderStageAllGraphicsFlags = ShaderStage::Vertex | ShaderStage::Geometry | ShaderStage::Pixel;


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
		RenderKey(uint32 material_id, uint32 mesh_id, uint8 submesh_slot)
			: key(
				(uint64)material_id << kMaterialIDShift
				| 
				(uint64)mesh_id << kMeshIDShift
				| 
				(uint64)submesh_slot << kSubMeshIndexShift)
		{}

		uint32 GetMaterialID() const { return (uint32)((key >> kMaterialIDShift) & kMaterialIDMask); }
		uint32 GetMeshID() const { return (uint32)((key >> kMeshIDShift) & kMeshIDMask); }
		uint8 GetSubMeshIndex() const { return (uint8)((key >> kSubMeshIndexShift) & kSubMeshIndexMask); }
		void Invalidate() { key = kInvalidID; }

		uint64 key = kInvalidID;

		auto operator<=>(const RenderKey& other) const = default;
		bool operator==(const RenderKey& other) const = default;

		static constexpr uint64 kInvalidID = (uint64)0;
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

	struct SubMesh
	{
		std::shared_ptr<Material> material = {};
		RenderKey key = {};
	};


	enum class RegisterType : uint8 { T, B, S, U };

	template <RegisterType Type>
	struct RegisterSlot
	{
		uint32 slot_num;
		constexpr explicit RegisterSlot(uint32 slot_num) : slot_num(slot_num) {}
		constexpr uint32 Get() const { return slot_num; }

		friend constexpr bool operator==(RegisterSlot a, RegisterSlot b)
		{
			return a.slot_num == b.slot_num;
		}
	};

	using RegisterT = RegisterSlot<RegisterType::T>;
	using RegisterB = RegisterSlot<RegisterType::B>;
	using RegisterS = RegisterSlot<RegisterType::S>;
	using RegisterU = RegisterSlot<RegisterType::U>;


	// RenderTargetGroup의 Clear API가 Depth/Stencil을 함께 지우므로 둘을 하나로 묶는다.
	enum class ClearTarget : uint8
	{
		Color = 0,
		DepthStencil,
		kCount
	};
	ENABLE_ENUM_FLAGS(ClearTargetFlags, ClearTarget, uint8);

	struct ClearSettings
	{
		ClearTargetFlags flags = ClearTarget::Color | ClearTarget::DepthStencil;
		std::array<float, 4> color = { 0.0f, 0.0f, 0.0f, 0.0f };
		float depth = 1.0f;
		uint8 stencil = 0;
	};
}