#pragma once
#include <Engine/Game/SceneEntity.h>
#include <Engine/Game/GameObject.h>
#include <Engine/Game/Component/ComponentCategory.h>

#include <Engine/Core/UtilMacro.h>

#include <Engine/Collision/Collision.h>


namespace engine
{
    class Component :
        public SceneEntity
    {
        ENTITY_INFO(Component, SceneEntity)

            friend class GameObject;
    public:
        Component(
            const HashedStringView& concrete_class_name,
            ComponentCategory category
        );
        virtual ~Component() override;

        virtual void Init() override;

        virtual void Awake();
        virtual void OnEnable();
        virtual void BeginPlay();

        virtual void Update() {}
        virtual void FixedUpdate() {}
        virtual void LateUpdate() {}

        virtual void OnDisable();
        virtual void OnDestroy() {}

        virtual void OnCollisionEnter2D(const Collision2D& _info) {}
        virtual void OnCollisionStay2D(const Collision2D& _info) {}
        virtual void OnCollisionExit2D(Collider2D* other) {}

        virtual void OnTriggerEnter2D(Collider2D* other) {}
        virtual void OnTriggerStay2D(Collider2D* other) {}
        virtual void OnTriggerExit2D(Collider2D* other) {}

        virtual void OnLayerChanged(uint32 prev_layer, uint32 new_layer) {}

        ComponentCategory GetComponentCategory() const { return category_; }

        void SetOwnerGameObject(GameObject* owner) { owner_game_object_ = owner; }
        GameObject* GetOwnerGameObject() const { return owner_game_object_; }

        template <typename T>
        T* GetComponent() {
            if (owner_game_object_) {
                return owner_game_object_->GetComponent<T>();
            }
            return nullptr;
        }

        bool HasInitialized() const { return has_initialized_; }
        bool HasAwaken() const { return has_awaken_; }
        bool HasBegunPlay() const { return has_begun_play_; }
        bool IsEnabled() const { return is_enabled_; }
        bool IsEnabledAndActiveInHierarchy() const { return is_enabled_and_active_in_hierarchy_; }

        void SetEnable(bool enable) {
            if (is_enabled_ == enable) { return; }
            is_enabled_ = enable;
            UpdateEnableState(GetOwnerGameObject()->IsActiveInHierarchy());
        }

        // Destroy 로직 순서는 Unity의 것을 따름
        // https://gamedevbeginner.com/how-to-destroy-an-object-in-unity/
		// 요약: 즉시 OnDisable 호출, Destroy 예약, 이번 프레임 끝에 OnDestroy 호출
        void Destroy();
        bool IsDestroyed() const { return is_destroyed_; }

        void SubscribeCollisionEvents(bool subscribe);

    private:
        void UpdateEnableState(bool is_active_in_hierarchy);
        void RegisterCollisionListener();
        void UnregisterCollisionListener();

        ComponentCategory category_ = {};

        GameObject* owner_game_object_ = {};

        bool has_initialized_ = false;
        bool has_awaken_ = false;
        bool has_begun_play_ = false;
        bool is_enabled_ = true;
        bool is_enabled_and_active_in_hierarchy_ = true;

        bool has_collision_subscribed_ = false;
        bool has_collision_sub_registered_ = false;

		bool is_destroyed_ = false;
    };
}


