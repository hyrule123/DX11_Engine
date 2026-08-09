#pragma once
#include <Engine/Game/SceneEntity.h>

#include <Engine/Game/Component/ComponentCategory.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/UtilMacro.h>

#include <Engine/Collision/Collision.h>

#include <array>
#include <vector>
#include <string>
#include <type_traits>

namespace engine
{
	class Component;
	class Scene;
	class Transform;

	class GameObject :
		public SceneEntity
	{
		ENTITY_INFO(GameObject, SceneEntity)
	public:
		GameObject();
		GameObject(const HashedStringView& concrete_class_name);
		virtual ~GameObject() override;

		virtual void Init() override;

		bool HasPendingComponents() const { return !pending_add_components_.empty(); }
		void FlushPendingComponents();	// Component 추가 및 초기화 담당

		void Update();
		void FixedUpdate();
		void LateUpdate();
		void FrameEnd();	// Component 제거
		
		Component* AddComponent(const HashedStringView& concrete_class_name);

		template <typename T>
		T* AddComponent() {
			static_assert(std::is_base_of_v<Component, T>, "Component의 하위 클래스만 가능!");
			return static_cast<T*>(AddComponent(EntityManager::GetInst().CreateEntity<T>()));
		}

		Component* GetComponent(const HashedStringView& concrete_class_name) const;

		template <typename T>
		T* GetComponent() const;

		Transform* GetTransform() const { return transform_; }

		void SetName(const std::string_view name) { name_ = name; }
		const std::string& GetName() const { return name_; }

		void SetOwnerScene(Scene* scene) { owner_scene_ = scene; }
		Scene* GetOwnerScene() const { return owner_scene_; }

		bool HasInitialized() const { return has_initialized_; }

		void SetActive(bool is_active);
		bool IsActive() const { return is_active_in_hierarchy_; }
		bool IsActiveSelf() const { return is_active_; }
		bool IsActiveInHierarchy() const { return is_active_in_hierarchy_; }

		// 파괴(비가역)
		void Destroy();
		bool IsDestroyed() const { return is_destroyed_; }

		void SetLayer(uint32 layer);
		uint32 GetLayer() const { return layer_; }

		void AddCollisionListener(Component* listener) {
			if (listener) { collision_listeners_.push_back(listener); }
		}
		void RemoveCollisionListener(Component* listener) {
			if (listener) {
				auto it = std::find(collision_listeners_.begin(), collision_listeners_.end(), listener);
				if (it != collision_listeners_.end()) {
					collision_listeners_.erase(it);
				}
			}
		}

		void BroadcastCollisionEnter2D(const Collision2D& col_info);
		void BroadcastCollisionStay2D(const Collision2D& col_info);
		void BroadcastCollisionExit2D(Collider2D* other);
		void BroadcastTriggerEnter2D(Collider2D* other);
		void BroadcastTriggerStay2D(Collider2D* other);
		void BroadcastTriggerExit2D(Collider2D* other);

	private:
		Component* AddComponent(u_ptr<Component> component);
		Component* AddPendingComponent(u_ptr<Component> component);

		void UpdateHierarchyState(bool is_active_in_hierarchy);

		Scene* owner_scene_ = {};

		std::array<u_ptr<Component>, (size_t)ComponentCategory::kEnd> fixed_order_components_ = {};
		std::vector<u_ptr<Component>> other_components_ = {};
		Transform* transform_ = {};

		std::string name_ = {};

		std::vector<u_ptr<Component>> pending_add_components_ = {};

		std::vector<Component*> collision_listeners_ = {};

		uint32 layer_ = 0;

		bool has_initialized_ = false;
		bool is_active_ = true;
		bool is_active_in_hierarchy_ = true;
		bool is_destroyed_ = false;

		bool is_calling_set_layer_ = false;
	};

	template <typename T>
	T* GameObject::GetComponent() const
	{
		static_assert(HasComponentCategory<T>,
			"ComponentCategory.h를 참고하여 컴포넌트 하위 클래스를 생성하세요.");

		T* ret = {};

		constexpr ComponentCategory cat = T::kComponentCategory;

		//고정순서 컴포넌트는 저장된 컴포넌트 번호만 찾아온다.
		if constexpr (ComponentCategory::kScripts < cat && cat < ComponentCategory::kEnd)
		{
			ret = dynamic_cast<T*>(fixed_order_components_[(size_t)cat].get());
		}
		//이외의 사용자 정의 컴포넌트들은 직접 순회를 돌며 찾는다
		//최적화 여지가 있지만 일단은 dynamic cast를 쓰는걸로...
		else
		{
			for (size_t i = 0; i < other_components_.size(); ++i)
			{
				ret = dynamic_cast<T*>(other_components_[i].get());
				if (ret) { break; }
			}
		}

		return ret;
	}
}


