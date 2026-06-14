#pragma once
#include <Engine/Core/Entity.h>

#include <Engine/Game/Component/ComponentCategory.h>
#include <Engine/Core/Type.h>
#include <Engine/Core/UtilMacro.h>
#include <array>
#include <vector>
#include <string>
#include <type_traits>

namespace engine
{
	class Component;
	class Scene;

	class GameObject :
		public Entity
	{
		CLASS_INFO(GameObject, Entity)
	public:
		GameObject();
		virtual ~GameObject() override;

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

		void SetName(const std::string_view name) { name_ = name; }
		const std::string& GetName() const { return name_; }

		void SetOwner(s_ptr<Scene> owner) { owner_ = std::move(owner); }
		const w_ptr<Scene>& GetOwner() const { return owner_; }

		virtual void Init();

		virtual void FrameStart();
		virtual void Update();
		virtual void LateUpdate();
		virtual void Render();
		virtual void FrameEnd();

	private:
		void AddComponentInternal(const s_ptr<Component>& component);

		w_ptr<Scene> owner_ = {};

		std::array<s_ptr<Component>, (size_t)ComponentCategory::kEnd> fixed_order_components_ = {};
		std::vector<s_ptr<Component>> other_components_ = {};

		std::string name_ = {};

		std::vector<s_ptr<Component>> pending_add_components_ = {};

		bool has_initialized_ = false;
	};

	template <typename T>
	s_ptr<T> GameObject::GetComponent() const
	{
		static_assert(HasComponentCategory<T>,
			"ComponentCategory.h를 참고하여 컴포넌트 하위 클래스를 생성하세요.");

		s_ptr<T> ret = {};

		constexpr ComponentCategory cat = T::kComponentCategory;

		//고정순서 컴포넌트는 저장된 컴포넌트 번호만 찾아온다.
		if constexpr (cat < ComponentCategory::kEnd)
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


