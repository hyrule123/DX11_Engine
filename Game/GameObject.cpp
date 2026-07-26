#include "Engine/Core/pch.h"
#include "GameObject.h"

#include <Engine/Game/Component/Transform.h>
#include <Engine/Game/Component/ComponentCategory.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	GameObject::GameObject()
		: Super(STRINGIFY(GameObject))
	{
	}

	GameObject::GameObject(const std::string_view name)
		: GameObject()
	{
		SetName(name);
	}

	GameObject::~GameObject()
	{}

	void GameObject::Init()
	{
		has_initialized_ = true;
		transform_ = AddComponent<Transform>();
	}

	void GameObject::FrameStart()
	{
		//먼저 싹 다 넣고
		for (const auto& com : pending_add_components_)
		{
			AddComponentInternal(com);
		}

		//Awake까지는 무조건 호출(다른 컴포넌트 탐색 보장)
		for (const auto& com : pending_add_components_)
		{
			if (!(com->HasAwaken()))
			{
				com->Awake();
			}
		}

		//GameObject의 활성화 상태에 따라 OnEnable 호출
		for (const auto& com : pending_add_components_)
		{
			if (IsActive() &&  com->IsEnabled())
			{
				com->OnEnable();
			}
		}

		pending_add_components_.clear();
	}

	void GameObject::Update()
	{
		for (const auto& com : other_components_)
		{
			if (com && com->IsEnabled())
			{
				if (!(com->HasStarted()))
				{
					com->Start();
				}
				com->Update();
			}
		}

		for (const auto& com : fixed_order_components_)
		{
			if (com && com->IsEnabled())
			{
				if (!(com->HasStarted()))
				{
					com->Start();
				}
				com->Update();
			}
		}
	}

	void GameObject::LateUpdate()
	{
		for (const auto& com : other_components_)
		{
			if (com && com->IsEnabled())
			{
				com->LateUpdate();
			}
		}

		for (const auto& com : fixed_order_components_)
		{
			if (com && com->IsEnabled())
			{
				com->LateUpdate();
			}
		}
	}

	void GameObject::FrameEnd()
	{
		for (auto& com : other_components_)
		{
			if (com && com->IsDestroyed())
			{
				com->OnDestroy();
				com = nullptr;
			}
		}

		for (auto& com : fixed_order_components_)
		{
			if (com && com->IsDestroyed())
			{
				com->OnDestroy();
				com = nullptr;
			}
		}

		//Other Components는 vector이므로 nullptr들인 항목은 제거
		std::erase_if(
			other_components_, 
			[](const s_ptr<Component>& com) { return com == nullptr; }
		);
	}

	s_ptr<Component> GameObject::AddComponent(s_ptr<Component> component)
	{
		if (component)
		{
			pending_add_components_.push_back(component);

			component->SetOwnerGameObject(std::static_pointer_cast<GameObject>(shared_from_this()));
			if (!component->HasInitialzed())
			{
				component->Init();
			}
		}

		return component;
	}

	s_ptr<Component> GameObject::GetComponent(const std::string_view concrete_class_name) const
	{
		for (size_t i = 0; i < fixed_order_components_.size(); ++i)
		{
			if (fixed_order_components_[i]
				&&
				fixed_order_components_[i]->GetConcreteClassName() == concrete_class_name)
			{
				return fixed_order_components_[i];
			}
		}

		for (size_t i = 0; i < other_components_.size(); ++i)
		{
			if (other_components_[i]
				&&
				other_components_[i]->GetConcreteClassName() == concrete_class_name)
			{
				return other_components_[i];
			}
		}

		return nullptr;
	}

	void GameObject::SetActive(bool is_active)
	{
		if (is_destroyed_) { return; }
		if (is_active_ == is_active) { return; }

		//일단 나 자신의 상태를 변경
		is_active_ = is_active;

		//부모의 상태를 확인하고, ActiveInHierarchy를 갱신
		Transform* parent_tr = transform_->GetParent();
		if (parent_tr)
		{
			GameObject* parent = parent_tr->GetOwnerGameObject().get();
			bool parent_active_in_hierarchy = parent ? parent->IsActiveInHierarchy() : true;

			UpdateHierarchyState(parent_active_in_hierarchy);
		}
		else
		{
			UpdateHierarchyState(is_active_);
		}
	}

	void GameObject::Destroy()
	{
		if (is_destroyed_) { return; }
		is_destroyed_ = true;

		is_active_ = false;
		is_active_in_hierarchy_ = false;

		//내꺼 파괴하고
		for (const auto& com : other_components_)
		{
			if (com)
			{
				com->Destroy();
			}
		}
		for (const auto& com : fixed_order_components_)
		{
			if (com)
			{
				com->Destroy();
			}
		}

		//연결된 자식들도 모두 파괴
		//Transform*은 제거될 때 부모-자식 관계가 끊어지므로, 복사본을 만들어서 순회하며 제거
		std::vector<Transform*> children_copy = transform_->GetChildren();
		for (Transform* child_tr : children_copy)
		{
			//Transform의 소유자는 반드시 있음이 보장
			child_tr->GetOwnerGameObject()->Destroy();
		}
	}

	void GameObject::UpdateHierarchyState(bool is_active_in_hierarchy)
	{
		const bool new_is_active_in_hierarchy = is_active_in_hierarchy && is_active_;

		if (is_active_in_hierarchy_ == new_is_active_in_hierarchy) { return; }

		is_active_in_hierarchy_ = new_is_active_in_hierarchy;

		for (const auto& com : other_components_)
		{
			if (com)
			{
				com->UpdateEnableState(new_is_active_in_hierarchy);
			}
		}
		for (const auto& com : fixed_order_components_)
		{
			if (com)
			{
				com->UpdateEnableState(new_is_active_in_hierarchy);
			}
		}

		const auto& child_tr = transform_->GetChildren();
		for (size_t i = 0; i < child_tr.size(); ++i)
		{
			if (auto child = child_tr[i]->GetOwnerGameObject())
			{
				child->UpdateHierarchyState(new_is_active_in_hierarchy);
			}
		}
	}

	void GameObject::AddComponentInternal(const s_ptr<Component>& component)
	{
		//넣을 때 null 체크 했으므로 무조건 있다고 가정
		ComponentCategory cat = component->GetComponentCategory();

		if (ComponentCategory::kScripts < cat)
		{
			if (fixed_order_components_[(size_t)cat])
			{
				ERROR_MESSAGE("컴포넌트 중복 추가됨. 확인 필요.");
			}

			fixed_order_components_[(size_t)cat] = component;
		}
		else
		{
			other_components_.push_back(component);
		}
	}
}

