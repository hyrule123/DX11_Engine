#pragma once
#include <Engine/Core/Enum.h>
#include <Engine/Core/Debug.h>
#include <Engine/Core/Geometry.h>

#include <Engine/Util/IDAllocator.h>

#ifdef NDEBUG
#define ENABLE_DEBUG_DRAW 0
#else
#define ENABLE_DEBUG_DRAW 1
#endif

struct ID3D11DeviceContext;

namespace engine
{
	class Renderer;
	struct RendererInfo2D
	{
		Renderer* renderer;
		AABB2D world_bounds;
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
		enum class ID : uint32_t
		{
			// 내장 패스
			ForwardOpaque = 0, ForwardBlend, Present, DebugDraw,

			// 커스텀 예약 슬롯 (콘텐츠에서 이름을 붙여 사용)
			Custom0, Custom1, Custom2, Custom3, Custom4, Custom5, Custom6, Custom7,
			Custom8,

			kCount
		};

		static_assert((size_t)ID::kCount < 32, "render_pass::ID can't exceed 32");

		ENABLE_ENUM_FLAGS(Flags, ID, uint32_t);
	}

	enum class ShaderStage : uint8_t
	{
		Vertex = 0,
		Geometry,
		Pixel,
		Compute,
		kCount
	};
	ENABLE_ENUM_FLAGS(ShaderStageFlags, ShaderStage, uint8_t);
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

	constexpr uint32_t kMaterialIDBits = 20;
	constexpr uint32_t kMeshIDBits = 20;
	constexpr uint32_t kSubMeshIndexBits = 8;
	static_assert(kSubMeshIndexBits <= 8);
	constexpr uint32_t kMaxSubMeshCount = 1u << kSubMeshIndexBits;

	using MaterialID = ScopedID<Material, kMaterialIDBits>;
	using MeshID = ScopedID<Mesh, kMeshIDBits>;

	struct RenderKey
	{
		static_assert(kMaterialIDBits + kMeshIDBits + kSubMeshIndexBits <= 64,
			"RenderKey layout overflows uint64_t.");

		RenderKey() = default;
		RenderKey(uint32_t material_id, uint32_t mesh_id, uint8_t submesh_slot)
			: key(
				(uint64_t)material_id << kMaterialIDShift
				| 
				(uint64_t)mesh_id << kMeshIDShift
				| 
				(uint64_t)submesh_slot << kSubMeshIndexShift)
		{}

		uint32_t GetMaterialID() const { return (uint32_t)((key >> kMaterialIDShift) & kMaterialIDMask); }
		uint32_t GetMeshID() const { return (uint32_t)((key >> kMeshIDShift) & kMeshIDMask); }
		uint8_t GetSubMeshIndex() const { return (uint8_t)((key >> kSubMeshIndexShift) & kSubMeshIndexMask); }
		void Invalidate() { key = kInvalidID; }

		uint64_t key = kInvalidID;

		auto operator<=>(const RenderKey& other) const = default;
		bool operator==(const RenderKey& other) const = default;

		static constexpr uint64_t kInvalidID = (uint64_t)0;
		static constexpr uint32_t kSubMeshIndexShift = 0;
		static constexpr uint32_t kMeshIDShift = kSubMeshIndexShift + kSubMeshIndexBits;
		static constexpr uint32_t kMaterialIDShift = kMeshIDShift + kMeshIDBits;

		static constexpr uint64_t kMeshIDMask = ((uint64_t)1 << kMeshIDBits) - 1;
		static constexpr uint64_t kMaterialIDMask = ((uint64_t)1 << kMaterialIDBits) - 1;
		static constexpr uint64_t kSubMeshIndexMask = ((uint64_t)1 << kSubMeshIndexBits) - 1;
	};

	struct RenderKeyHasher
	{
		size_t operator()(const RenderKey& key) const
		{
			return std::hash<uint64_t>()(key.key);
		}
	};

	struct SubMesh
	{
		std::shared_ptr<Material> material = {};
		RenderKey key = {};
	};


	enum class RegisterType : uint8_t { T, B, S, U };

	template <RegisterType Type>
	struct RegisterSlot
	{
		uint32_t slot_num;
		constexpr explicit RegisterSlot(uint32_t slot_num) : slot_num(slot_num) {}
		constexpr uint32_t Get() const { return slot_num; }

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
	enum class ClearTarget : uint8_t
	{
		Color = 0,
		DepthStencil,
		kCount
	};
	ENABLE_ENUM_FLAGS(ClearTargetFlags, ClearTarget, uint8_t);

	struct ClearSettings
	{
		ClearTargetFlags flags = ClearTarget::Color | ClearTarget::DepthStencil;
		std::array<float, 4> color = { 0.0f, 0.0f, 0.0f, 0.0f };
		float depth = 1.0f;
		uint8_t stencil = 0;
	};

	struct InstanceDataAllocation
	{
		struct InstanceBufferEntry* entry = nullptr;	// nullptr = 할당 없음 (stride 0)
		uint32_t start = 0;					// Draw의 instance_start
		uint32_t count = 0;					// Draw의 instance_count, 범위 검사용

		bool IsValid() const { return entry != nullptr; }
	};

	enum class DrawFlag : uint16_t
	{
		NoBatching,
		kCount
	};
	ENABLE_ENUM_FLAGS(DrawFlags, DrawFlag, uint16_t);
}