#include "Engine/Core/pch.h"
#include "GameObject.h"

#include <Engine/Game/Component/Transform.h>
#include <Engine/Game/Component/ComponentCategory.h>

#include <Engine/Core/Debug.h>
#include <Engine/Core/Constant.h>

namespace engine
{
	GameObject::GameObject()
		: Super(GameObject::kClassConcreteName)
	{
	}

	GameObject::GameObject(const HashedStringView& concrete_class_name)
		: Super(concrete_class_name)
	{}

	GameObject::~GameObject()
	{}

	void GameObject::Init()
	{
		Super::Init();
		has_initialized_ = true;
		transform_ = AddComponent<Transform>();
	}

	void GameObject::FlushPendingComponents()
	{
		std::vector<Component*> flushed_components = {};
		flushed_components.reserve(pending_add_components_.size());

		//먼저 싹 다 넣고
		for (size_t i = 0; i < pending_add_components_.size(); ++i)
		{
			Component* flushed = AddPendingComponent(std::move(pending_add_components_[i]));
			if (flushed)
			{
				flushed_components.push_back(flushed);
			}
		}

		//pending 대기열은 비워준다
		pending_add_components_.clear();

		// 삽입 성공한 Component에 대해 Awake까지는 무조건 호출(다른 컴포넌트 탐색 보장)
		for (size_t i = 0; i < flushed_components.size(); ++i)
		{
			if (false == flushed_components[i]->HasAwaken())
			{
				flushed_components[i]->Awake();
			}
		}

		//GameObject의 활성화 상태에 따라 OnEnable 호출
		for (size_t i = 0; i < flushed_components.size(); ++i)
		{
			if (IsActive() &&  flushed_components[i]->IsEnabled())
			{
				flushed_components[i]->OnEnable();
			}
		}
	}

	void GameObject::Update()
	{
		for (const auto& com : other_components_)
		{
			if (com && com->IsEnabled())
			{
				if (false == com->HasBegunPlay())
				{
					com->BeginPlay();
				}
				com->Update();
			}
		}

		for (const auto& com : fixed_order_components_)
		{
			if (com && com->IsEnabled())
			{
				if (false == com->HasBegunPlay())
				{
					com->BeginPlay();
				}
				com->Update();
			}
		}
	}

	void GameObject::FixedUpdate()
	{
		for (const auto& com : other_components_)
		{
			if (com && com->IsEnabled())
			{
				if (false == com->HasBegunPlay())
				{
					com->BeginPlay();
				}
				com->FixedUpdate();
			}
		}
		for (const auto& com : fixed_order_components_)
		{
			if (com && com->IsEnabled())
			{
				if (false == com->HasBegunPlay())
				{
					com->BeginPlay();
				}
				com->FixedUpdate();
			}
		}
	}

	void GameObject::LateUpdate()
	{
		for (const auto& com : other_components_)
		{
			if (com && com->IsEnabled())
			{
				if (false == com->HasBegunPlay())
				{
					com->BeginPlay();
				}
				com->LateUpdate();
			}
		}

		for (const auto& com : fixed_order_components_)
		{
			if (com && com->IsEnabled())
			{
				if (false == com->HasBegunPlay())
				{
					com->BeginPlay();
				}
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
			[](const u_ptr<Component>& com) { return com == nullptr; }
		);

		// 혹시나 pending_add_components_에 Destroy된 녀석이 있을 수 있으므로 제거
		std::erase_if(
			pending_add_components_,
			[](const u_ptr<Component>& com) { return com->IsDestroyed(); }
		);
	}

	Component* GameObject::AddComponent(const HashedStringView& concrete_class_name)
	{
		u_ptr<Component> comp = EntityManager::GetInst().CreateEntityAs<Component>(concrete_class_name);
		if (comp)
		{
			return AddComponent(std::move(comp));
		}
		return nullptr;
	}

	Component* GameObject::GetComponent(const HashedStringView& concrete_class_name) const
	{
		for (size_t i = 0; i < fixed_order_components_.size(); ++i)
		{
			if (fixed_order_components_[i]
				&&
				fixed_order_components_[i]->GetConcreteClassName() == concrete_class_name)
			{
				return fixed_order_components_[i].get();
			}
		}

		for (size_t i = 0; i < other_components_.size(); ++i)
		{
			if (other_components_[i]
				&&
				other_components_[i]->GetConcreteClassName() == concrete_class_name)
			{
				return other_components_[i].get();
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
			GameObject* parent = parent_tr->GetOwnerGameObject();
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
		if (IsDestroyed()) { return; }
		is_destroyed_ = true;
		is_active_ = false;
		is_active_in_hierarchy_ = false;

		//Handle 무효화, wh_ptr에서 받아올 수 없음
		InvalidateHandle();

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

	void GameObject::SetLayer(uint32 layer)
	{
		if (layer_ == layer) { return; }

		if (is_calling_set_layer_)
		{
			ASSERT_MESSAGE(false, "OnLayerChanged()에서 SetLayer()를 호출하면 안됨");
			return;
		}
		is_calling_set_layer_ = true;

		if(kMaxLayers <= layer)
		{
			ASSERT_MESSAGE(false, "layer 값이 유효 범위를 벗어났습니다.");
			is_calling_set_layer_ = false;
			return;
		}

		uint32 prev_layer = layer_;
		layer_ = layer;

		for (const auto& com : other_components_)
		{
			if (com && com->HasAwaken())
			{
				com->OnLayerChanged(prev_layer, layer_);
			}
		}
		for (const auto& com : fixed_order_components_)
		{
			if (com && com->HasAwaken())
			{
				com->OnLayerChanged(prev_layer, layer_);
			}
		}

		is_calling_set_layer_ = false;
	}

	void GameObject::BroadcastCollisionEnter2D(const Collision2D& col_info)
	{
		for (const auto& listener : collision_listeners_)
		{
			listener->OnCollisionEnter2D(col_info);
		}
	}

	void GameObject::BroadcastCollisionStay2D(const Collision2D & col_info)
	{
		for (const auto& listener : collision_listeners_)
		{
			listener->OnCollisionStay2D(col_info);
		}
	}

	void GameObject::BroadcastCollisionExit2D(Collider2D * other)
	{
		for (const auto& listener : collision_listeners_)
		{
			listener->OnCollisionExit2D(other);
		}
	}

	void GameObject::BroadcastTriggerEnter2D(Collider2D * other)
	{
		for (const auto& listener : collision_listeners_)
		{
			listener->OnTriggerEnter2D(other);
		}
	}

	void GameObject::BroadcastTriggerStay2D(Collider2D * other)
	{
		for (const auto& listener : collision_listeners_)
		{
			listener->OnTriggerStay2D(other);
		}
	}

	void GameObject::BroadcastTriggerExit2D(Collider2D * other)
	{
		for (const auto& listener : collision_listeners_)
		{
			listener->OnTriggerExit2D(other);
		}
	}

	Component* GameObject::AddComponent(u_ptr<Component> component)
	{
		Component* ret = component.get();
		if (component)
		{
			pending_add_components_.push_back(std::move(component));
			ret->SetOwnerGameObject(this);
		}
		return ret;
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

	Component* GameObject::AddPendingComponent(u_ptr<Component> component)
	{
		// const가 아닌 어떤 함수도 호출하지 마시오!!!!!!!!(단순히 자리에 넣는 용도)

		//넣을 때 null 체크 했으므로 무조건 있다고 가정
		if (component->IsDestroyed()) { return nullptr; }	// 이미 Destroy된 녀석은 넣지 않음

		ComponentCategory cat = component->GetComponentCategory();

		Component* ret = component.get();

		if (ComponentCategory::kScripts < cat)
		{
			if (nullptr == fixed_order_components_[(size_t)cat])
			{
				fixed_order_components_[(size_t)cat] = std::move(component);
			}
			else
			{
				ASSERT_MESSAGE(false, "컴포넌트 중복 추가됨. 확인 필요.");
				ret = nullptr;
			}
		}
		else
		{
			other_components_.push_back(std::move(component));
		}

		return ret;
	}
}