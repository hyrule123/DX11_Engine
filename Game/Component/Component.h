#pragma once
#include <Engine/Core/Entity.h>

#include <Engine/Game/GameObject.h>

#include <Engine/Game/Component/ComponentCategory.h>

#include <Engine/Core/UtilMacro.h>

namespace engine
{
    class Component :
        public Entity
    {
        CLASS_INFO(Component, Entity)
        
		friend class GameObject;
    public:
        Component(
            const HashedStringView& concrete_class_name, 
            ComponentCategory category
            );
        virtual ~Component() override;

        virtual void Init() = 0;
		virtual void OnEnable() {}
        virtual void Awake();
        virtual void BeginPlay();
        
        virtual void Update() {}
        virtual void LateUpdate() {}

		virtual void OnDisable() {}
		virtual void OnDestroy() {}

        ComponentCategory GetComponentCategory() const { return category_; }

        void SetOwnerGameObject(s_ptr<GameObject> owner) { owner_game_object_ = owner; }
        s_ptr<GameObject> GetOwnerGameObject() const { return owner_game_object_.lock(); }
        
        template <typename T>
        s_ptr<T> GetComponent() {
            if (!(owner_game_object_.expired())) {
                return owner_game_object_.lock()->GetComponent<T>();
            }
            return nullptr;
        }

        bool HasInitialized() const { return has_initialized_; }
        bool HasAwaken() const { return has_awaken_; }
        bool HasBegunPlay() const { return has_begun_play_; }
        bool IsEnabled() const { return is_enabled_; }
		bool IsEnabledAndActiveInHierarchy() const { return is_enabled_and_active_in_hierarchy_; }
		bool IsDestroyed() const { return is_destroyed_; }

        void SetEnable(bool enable) { 
			if (is_enabled_ == enable) { return; }
            is_enabled_ = enable; 
            UpdateEnableState(GetOwnerGameObject()->IsActiveInHierarchy()); 
        }

        void Destroy();

    private:
		void UpdateEnableState(bool is_active_in_hierarchy);

        ComponentCategory category_ = {};

        w_ptr<GameObject> owner_game_object_ = {};

        bool has_initialized_ = false;
        bool has_awaken_ = false;
        bool has_begun_play_ = false;
		bool is_enabled_ = true;
		bool is_enabled_and_active_in_hierarchy_ = true;
		bool is_destroyed_ = false;
    };
}


