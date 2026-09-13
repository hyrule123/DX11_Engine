#pragma once
#include <Engine/Core/StdType.h>
#include <Engine/Core/Debug.h>
#include <Engine/Core/Constant.h>

#include <vector>

namespace engine {

    // 타입별로 독립적인 ID 공간을 발급하는 정적 할당기.
    //
    // Tag마다 별개의 static 멤버가 인스턴스화되므로
    // IDAllocator<Material>과 IDAllocator<Mesh>는 서로 다른 카운터와
    // free list를 갖는다. Tag는 전방 선언만 되어 있어도 무방하다.
    //
    // kBitWidth는 RenderKey에 패킹될 비트 폭. 발급 상한을 여기서 묶어
    // 키 생성 시점이 아니라 발급 시점에 오버플로를 잡는다.
    //
    // 스레드 안전성 없음 — 단일 스레드 리소스 로딩 전제.
    // 직접 쓰지 않고 아래 ScopedID를 통해서만 사용한다.
    constexpr uint32 kInvalidID = 0;

    template <typename Tag, uint32 kBitWidth>
    class IDAllocator {
    public:
        using IDType = uint32;

        // ID 0을 무효값으로 예약하므로 실제 발급 가능한 최대치는 (2^n - 1).
        static constexpr IDType kMaxID = (kBitWidth >= 32)
            ? 0xFFFFFFFFu
            : ((1u << kBitWidth) - 1u);

        static_assert(kBitWidth > 0 && kBitWidth <= 32,
            "kBitWidth must be in (0, 32].");

        IDAllocator() = delete;

        static IDType Acquire() {
            std::vector<IDType>& free_list = FreeList();
            if (!free_list.empty()) {
                const IDType id = free_list.back();
                free_list.pop_back();
                MarkAlive(id, true);
                return id;
            }

            ASSERT_MESSAGE(next_id_ <= kMaxID, "ID space exhausted.");
            const IDType id = next_id_++;
            MarkAlive(id, true);
            return id;
        }

        static void Release(IDType id) {
            ASSERT_MESSAGE(id != kInvalidID, "Releasing an invalid ID.");
            ASSERT_MESSAGE(id < next_id_, "Releasing an ID that was never issued.");
            ASSERT_MESSAGE(IsAlive(id), "Double release detected.");

            MarkAlive(id, false);
            FreeList().push_back(id);
        }

        // 디버그/통계용 — 현재 살아있는 ID 개수
        static IDType GetAliveCount() {
            return next_id_ - 1 - static_cast<IDType>(FreeList().size());
        }

    private:
        // 의도적 leak. 정적 소멸 순서에 관계없이 Release()가 항상 유효한
        // 컨테이너를 보도록 하기 위함 — 리소스가 static보다 늦게 죽어도 안전하다.
        // 순수 데이터 레지스트리이므로 프로세스 종료 시 회수 불필요.
        static std::vector<IDType>& FreeList() {
            static std::vector<IDType>* list = new std::vector<IDType>();
            return *list;
        }

        // 상수 초기화라 정적 초기화 순서 문제 없음.
        inline static IDType next_id_ = kInvalidID + 1;

#if defined(_DEBUG)
        // index == ID. 0번 슬롯은 kInvalidID 자리라 사용하지 않는다.
        static std::vector<bool>& AliveFlags() {
            static std::vector<bool>* flags = new std::vector<bool>();
            return *flags;
        }

        static void MarkAlive(IDType id, bool alive) {
            std::vector<bool>& flags = AliveFlags();
            if (flags.size() <= id) {
                flags.resize(static_cast<size_t>(id) + 1, false);
            }
            flags[id] = alive;
        }

        static bool IsAlive(IDType id) {
            const std::vector<bool>& flags = AliveFlags();
            return id < flags.size() && flags[id];
        }
#else
        static void MarkAlive(IDType, bool) {}
        static bool IsAlive(IDType) { return true; }
#endif
    };

    // ---------------------------------------------------------------------------
    // ScopedID — ID의 발급과 반납을 객체 수명에 묶는 래퍼.
    //
    // 규칙: ID는 인스턴스의 정체성이다.
    //   - 모든 생성자(기본/복사/이동)가 각자 새로운 ID를 발급한다.
    //     복사본과 이동 결과물은 원본과 다른 인스턴스이므로 ID를 공유하지 않는다.
    //   - 대입은 정체성을 바꾸지 않는다. 이미 살아있는 객체의 ID는 유지된다.
    //   - 소멸자가 반드시 반납하므로 누락이 생기지 않는다.
    //
    // 생성 시점에 항상 유효한 ID를 갖게 되므로 무효 상태가 존재하지 않는다.
    // (생성자에서는 단순 초기화만 한다는 컨벤션의 의도적 예외 — Init()을 거치지
    //  않는 지역 객체 경로가 존재하는 이상 생성자가 아니면 보장할 수 없다.)
    // ---------------------------------------------------------------------------
    template <typename Tag, uint32 kBitWidth>
    class ScopedID {
    public:
        using AllocatorType = IDAllocator<Tag, kBitWidth>;
        using IDType = typename AllocatorType::IDType;

        ScopedID() : id_(AllocatorType::Acquire()) {}

        ~ScopedID() { AllocatorType::Release(id_); }

        // Acquire()가 free list 벡터를 건드리므로 noexcept를 붙일 수 없다.
        ScopedID(const ScopedID&) : id_(AllocatorType::Acquire()) {}
        ScopedID(ScopedID&&) : id_(AllocatorType::Acquire()) {}

        ScopedID& operator=(const ScopedID&) { return *this; }
        ScopedID& operator=(ScopedID&&) noexcept { return *this; }

        // 암시적 변환은 제공하지 않는다 — 패킹 시점을 항상 눈에 보이게 둔다.
        IDType Get() const { return id_; }

    private:
        IDType id_;
    };
}  // namespace engine