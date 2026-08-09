#pragma once
#include <Engine/Core/StdType.h>

#include <vector>
#include <limits>
#include <type_traits>
#include <concepts>

namespace engine
{
    //Generation(상위 비트 32) | ID(하위 비트 32)
    class Handle
    {
    public:
        // 0 == Invalid Handle (실제 Invalid Check는 Generation이 0인 경우만 진행)
        inline static constexpr uint32 kInvalidGeneration = 0;

        constexpr Handle() {}

        constexpr Handle(uint32 slot_id, uint32 generation)
            : value_((uint64(generation) << 32) | slot_id)
        {}

        constexpr uint32 GetSlotID() const
        {
            return static_cast<uint32>(value_);
        }

        constexpr uint32 GetGeneration() const
        {
            return static_cast<uint32>(value_ >> 32);
        }

        constexpr bool IsValid() const
        {
            return GetGeneration() != kInvalidGeneration;
        }

        constexpr uint64 Value() const
        {
            return value_;
        }

        constexpr bool operator==(const Handle& other) const
        {
            return value_ == other.value_;
        }

        void Reset()
        {
            value_ = {};
        }

    private:
        uint64 value_ = {};
    };

    class HandlePointerPool
    {
        template <typename T>
        friend class enable_handle_from_this;

        template <typename T>
        friend class weak_handle_ptr;

        inline static constexpr uint32 kUint32Max = (std::numeric_limits<uint32>::max)();

    public:
        static HandlePointerPool& GetInst()
        {
            static HandlePointerPool instance;
            return instance;
        }

    private:
        // 싱글턴 패턴의 소멸 시점을 정하고 싶다면 GetInst()를 미리 해 둘것.
        HandlePointerPool();
        ~HandlePointerPool();

        void* Resolve(Handle handle) const
        {
            if (!IsAlive(handle)) { return nullptr; }
            return slots_[handle.GetSlotID()].object;
        }

        Handle Create(void* object);
        void Destroy(Handle handle);

        bool IsAlive(Handle handle) const
        {
            if (!handle.IsValid()) { return false; }

            uint32 slot_id = handle.GetSlotID();
            if (slot_id >= slots_.size()) { return false; }

            const Slot& slot = slots_[slot_id];
            return slot.object != nullptr &&
                slot.generation == handle.GetGeneration();
        }

        struct Slot
        {
            void* object = nullptr;
            uint32 generation = 1u;
            uint32 next_free = kUint32Max;
        };

        std::vector<Slot> slots_;
        uint32 next_free_index_ = kUint32Max;
    };

    //전방 선언
    template <typename T>
    class weak_handle_ptr;

    template <typename T>
    class enable_handle_from_this
    {
        template <typename U>
        friend class weak_handle_ptr;
    public:
        using handle_root_type = T;

        weak_handle_ptr<T> weak_handle_from_this() const
        {
            return weak_handle_ptr<T>(handle_);
        }

    protected:
        //참고
        // enable_handle_from_this<T>* 와
        // T* 는
        // "주소가 다르다" (다중 상속 시)
        // 따라서 static_cast<T*>(this) 와 this는 다름

        enable_handle_from_this<T>()
            : handle_(HandlePointerPool::GetInst().Create(this))
        {}

        enable_handle_from_this<T>(const enable_handle_from_this<T>&)
            : handle_(HandlePointerPool::GetInst().Create(this))
        {}
        ~enable_handle_from_this<T>() { InvalidateHandle(); }

        //일단 복사 생성자와 이동 생성자는 금지
        enable_handle_from_this& operator=(const enable_handle_from_this&) = delete;
        enable_handle_from_this(enable_handle_from_this&&) = delete;
        enable_handle_from_this& operator=(enable_handle_from_this&&) = delete;

        //포인터는 즉시 삭제되지 않지만, wh_ptr 참조는 불가능
        void InvalidateHandle() {
            if (handle_.IsValid())
            {
                HandlePointerPool::GetInst().Destroy(handle_);
                handle_.Reset();
            }
        }

    private:
        Handle GetHandle() const
        {
            return handle_;
        }

        Handle handle_;
    };

    template <typename T>
    concept HandleEnabled = requires { typename T::handle_root_type; }&&
        std::is_base_of_v<typename T::handle_root_type, T>;

    template <typename T>
    class weak_handle_ptr
    {
        friend class enable_handle_from_this<T>;

        template <typename U>
        friend class weak_handle_ptr;
    public:
        weak_handle_ptr<T>() = default;

		weak_handle_ptr<T>(nullptr_t) {}

        // 업캐스팅
        template<typename U> requires std::derived_from<U, T>
        weak_handle_ptr(const weak_handle_ptr<U>& other) : handle_(other.handle_) {}

        // 포인터를 통해 생성 시, T 클래스가 include 되어 있어야 함
        template <typename U> requires std::is_base_of_v<T, U>
        weak_handle_ptr(U* entity)
        {
            static_assert(HandleEnabled<T>, "T must be derived from enable_handle_from_this");
            if (entity) { handle_ = entity->GetHandle(); }
        }

        //꺼내 쓰려면 cpp에서 포함시켜야 함 (shared_ptr과 다른 점)
        T* get() const
        {
            static_assert(HandleEnabled<T>, "T must be derived from enable_handle_from_this");

            // 1차 캐스팅: T::handle_root_type를 기반으로 
            // enable_handle_from_this<handle_root_type> 포인터를 얻음 
            using handle_root_type = typename T::handle_root_type;
            enable_handle_from_this<handle_root_type>* base_ptr = static_cast<enable_handle_from_this<handle_root_type>*>(
                HandlePointerPool::GetInst().Resolve(handle_)
                );

            // 2차 캐스팅: enable_handle_from_this<handle_root_type> 포인터를 T 포인터로 변환
            return static_cast<T*>(base_ptr);
        }

		void reset() { handle_.Reset(); }

        template<typename U, typename V>
        friend weak_handle_ptr<U> static_handle_cast(const weak_handle_ptr<V>& src) noexcept;

        template<typename U, typename V>
        friend weak_handle_ptr<U> dynamic_handle_cast(const weak_handle_ptr<V>& src) noexcept;
    private:
        weak_handle_ptr(Handle handle) : handle_(handle) {}
        Handle handle_;
    };

    template<typename U, typename V>
    weak_handle_ptr<U> static_handle_cast(const weak_handle_ptr<V>& src) noexcept
    {
        // 상속 관계가 있어야 함 (U가 V의 파생 클래스이거나, V가 U의 파생 클래스여야 함)
        static_assert(std::derived_from<U, V> || std::derived_from<V, U>);
		// U가 enable_handle_from_this를 상속받아야 함
        static_assert(HandleEnabled<U>, "U must be derived from enable_handle_from_this");
        return weak_handle_ptr<U>{ src.handle_ };
    }

    template<typename U, typename V>
    weak_handle_ptr<U> dynamic_handle_cast(const weak_handle_ptr<V>& src) noexcept
    {
		// dynamic_cast는 알아서 상속 관계를 확인하므로, static_assert로 상속 관계를 확인할 필요는 없음
		// U가 enable_handle_from_this를 상속받아야 함
        static_assert(HandleEnabled<U>, "U must be derived from enable_handle_from_this");
        if (dynamic_cast<U*>(src.get()) == nullptr) { return weak_handle_ptr<U>{}; }
        return weak_handle_ptr<U>{ src.handle_ };
    }
};