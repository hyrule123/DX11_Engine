#pragma once
#include <Engine/Core/Entity.h>

#include <Engine/Game/Component/ComponentCategory.h>
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/UtilMacro.h>
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
		public Entity
	{
		CLASS_INFO(GameObject, Entity)
	public:
		GameObject();
		GameObject(const std::string_view name);
		virtual ~GameObject() override;

		virtual void Init();

		void FrameStart();	// Component 추가 및 초기화 담당
		void Update();
		void LateUpdate();
		void FrameEnd();	// Component 제거

		s_ptr<Component> AddComponent(s_ptr<Component> component);

		template <typename T>
		s_ptr<T> AddComponent() {
			static_assert(std::is_base_of_v<Component, T>, "Component의 하위 클래스만 가능!");
			s_ptr<T> comp = std::make_shared<T>();
			AddComponent(comp);
			return comp;
		}

		s_ptr<Component> GetComponent(const std::string_view concrete_class_name) const;

		template <typename T>
		s_ptr<T> GetComponent() const;

		s_ptr<Transform> GetTransform() const { return transform_; }

		void SetName(const std::string_view name) { name_ = name; }
		const std::string& GetName() const { return name_; }

		void SetOwnerScene(s_ptr<Scene> scene) { owner_scene_ = std::move(scene); }
		s_ptr<Scene> GetOwnerScene() const { return owner_scene_.lock(); }

		bool HasInitialized() const { return has_initialized_; }

		void SetActive(bool is_active);
		bool IsActive() const { return is_active_in_hierarchy_; }
		bool IsActiveSelf() const { return is_active_; }
		bool IsActiveInHierarchy() const { return is_active_in_hierarchy_; }
		bool IsDestroyed() const { return is_destroyed_; }

		// 파괴(비가역)
		void Destroy();

	private:

		void UpdateHierarchyState(bool is_active_in_hierarchy);

		void AddComponentInternal(const s_ptr<Component>& component);

		w_ptr<Scene> owner_scene_ = {};

		std::array<s_ptr<Component>, (size_t)ComponentCategory::kEnd> fixed_order_components_ = {};
		std::vector<s_ptr<Component>> other_components_ = {};
		s_ptr<Transform> transform_ = {};

		std::string name_ = {};

		std::vector<s_ptr<Component>> pending_add_components_ = {};

		bool has_initialized_ = false;
		bool is_active_ = true;
		bool is_active_in_hierarchy_ = true;
		bool is_destroyed_ = false;
	};

	template <typename T>
	s_ptr<T> GameObject::GetComponent() const
	{
		static_assert(HasComponentCategory<T>,
			"ComponentCategory.h를 참고하여 컴포넌트 하위 클래스를 생성하세요.");

		s_ptr<T> ret = {};

		constexpr ComponentCategory cat = T::kComponentCategory;

		//고정순서 컴포넌트는 저장된 컴포넌트 번호만 찾아온다.
		if constexpr (ComponentCategory::kScripts < cat && cat < ComponentCategory::kEnd)
		{
			ret = std::dynamic_pointer_cast<T>(fixed_order_components_[(size_t)cat]);
		}
		//이외의 사용자 정의 컴포넌트들은 직접 순회를 돌며 찾는다
		//최적화 여지가 있지만 일단은 dynamic cast를 쓰는걸로...
		else
		{
			for (size_t i = 0; i < other_components_.size(); ++i)
			{
				ret = std::dynamic_pointer_cast<T>(other_components_[i]);
				if (ret) { break; }
			}
		}

		return ret;
	}
}


