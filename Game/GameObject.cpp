#include "Engine/Core/pch.h"
#include "GameObject.h"

#include <Engine/Game/Component/Transform.h>
#include <Engine/Game/Component/ComponentCategory.h>

#include <Engine/Core/Debug.h>
#include <Engine/Core/Constants.h>

#include <span>

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
	{

	}

	void GameObject::Init()
	{
		Super::Init();
		transform_ = AddComponent<Transform>();
	}

	void GameObject::FlushPendingComponents()
	{
		has_pending_components_ = false;

		std::vector<Component*> awaken_components;

		auto CheckAwake = [this, &awaken_components](Component* com) -> void
			{
				// 삽입 성공한 Component에 대해 Awake까지는 무조건 호출(다른 컴포넌트 탐색 보장)
				if (com && com->IsDestroyed() == false && com->HasAwaken() == false)
				{
					com->MarkAwaken();
					com->ReplaySubscriptions();
					com->Awake();
					awaken_components.push_back(com);
				}
			};

		size_t snapshot_size = other_components_.size();
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			CheckAwake(other_components_[i].get());
		}

		snapshot_size = fixed_order_components_.size();
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			CheckAwake(fixed_order_components_[i].get());
		}

		for (Component* com : awaken_components)
		{
			// 조건 판단 내부에서 진행됨
			com->UpdateEnableState(IsActiveInHierarchy());
		}
	}

	void GameObject::Update()
	{
		auto UpdateFunc = [](Component* com) -> void
			{
				if (com->IsEnabled())
				{
					com->Update();
				}
			};

		size_t snapshot_size = other_components_.size();
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			if (other_components_[i]) { UpdateFunc(other_components_[i].get()); }
		}

		snapshot_size = fixed_order_components_.size();
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			if (fixed_order_components_[i]) { UpdateFunc(fixed_order_components_[i].get()); }
		}
	}

	void GameObject::FixedUpdate()
	{
		auto  FixedUpdateFunc = [](Component* com) -> void
			{
				if (com->IsEnabled())
				{
					com->FixedUpdate();
				}
			};

		size_t snapshot_size = other_components_.size();
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			if (other_components_[i]) { FixedUpdateFunc(other_components_[i].get()); }
		}

		snapshot_size = fixed_order_components_.size();
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			if (fixed_order_components_[i]) { FixedUpdateFunc(fixed_order_components_[i].get()); }
		}
	}

	void GameObject::LateUpdate()
	{
		auto LateUpdateFunc = [](Component* com) -> void
			{
				if (com->IsEnabled())
				{
					com->LateUpdate();
				}
			};

		size_t snapshot_size = other_components_.size();
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			if (other_components_[i]) { LateUpdateFunc(other_components_[i].get()); }
		}

		snapshot_size = fixed_order_components_.size();
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			if (fixed_order_components_[i]) { LateUpdateFunc(fixed_order_components_[i].get()); }
		}
	}

	void GameObject::FrameEnd(std::vector<std::unique_ptr<SceneEntity>>& graveyard)
	{
		uint32 pass_count = 0;
		for (; pass_count < kMaxDrainLoopCount; ++pass_count)
		{
			bool any_destroyed = false;

			//Destroy된 Component들의 OnDestroy 호출 및 Graveyard 이동
			for (std::unique_ptr<Component>& com : other_components_)
			{
				if (com && com->IsDestroyed())
				{
					com->OnDestroy();
					graveyard.push_back(std::move(com));
					any_destroyed = true;
				}
			}

			//vector이므로 snapshot 사용
			size_t snapshot_size = fixed_order_components_.size();
			for (size_t i = 0; i < snapshot_size; ++i)
			{
				if (fixed_order_components_[i] && fixed_order_components_[i]->IsDestroyed())
				{
					fixed_order_components_[i]->OnDestroy();
					graveyard.push_back(std::move(fixed_order_components_[i]));
					any_destroyed = true;
				}
			}

			if (!any_destroyed) { break; }
		}

		// 최대 pass 도달 시 확인 필요(debug)
		ASSERT_F(pass_count < kMaxDrainLoopCount, "Destroy Pass Count exceeded. Possible infinite loop in destruction.");

		//Other Components는 vector이므로 nullptr들인 항목은 제거
		std::erase_if(
			other_components_, 
			[](const std::unique_ptr<Component>& com) { return com == nullptr; }
		);

		for(uint32 i = 0; i < (uint32)SubscribeType::kEND; ++i)
		{
			if (is_listeners_dirty_.test(i))
			{
				std::erase_if(
					listeners_[i],
					[](Component* listener) { return listener == nullptr; }
				);
				is_listeners_dirty_.reset(i);
			}
		}
	}

	Component* GameObject::AddComponent(const HashedStringView& concrete_class_name)
	{
		std::unique_ptr<Component> comp = EntityManager::GetInst().CreateEntityAs<Component>(concrete_class_name);
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
		bool parent_active_in_hierarchy = true;
		if (Transform* parent_tr = transform_->GetParent())
		{
			if (GameObject* parent = parent_tr->GetOwnerGameObject())
			{
				parent_active_in_hierarchy = parent->IsActiveInHierarchy();
			}
		}
		UpdateHierarchyState(parent_active_in_hierarchy);
	}

	void GameObject::Destroy()
	{
		if (IsDestroyed()) { return; }
		is_destroyed_ = true;
		is_active_ = false;
		is_active_in_hierarchy_ = false;

		//Handle 무효화, weak_handle_ptr에서 받아올 수 없음
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
			ASSERT_F(false, "OnLayerChanged()에서 SetLayer()를 호출하면 안됨");
			return;
		}
		is_calling_set_layer_ = true;

		if(kMaxLayers <= layer)
		{
			ASSERT_F(false, "layer 값이 유효 범위를 벗어났습니다.");
			is_calling_set_layer_ = false;
			return;
		}

		uint32 prev_layer = layer_;
		layer_ = layer;

		for(Component* listener : listeners_[(size_t)SubscribeType::kLayerChanged])
		{
			if (listener) { listener->OnLayerChanged(prev_layer, layer_); }
		}

		is_calling_set_layer_ = false;
	}

	void GameObject::Subscribe(SubscribeType type, Component* listener)
	{
		// Destroy 되었으면 어차피 이것도 제거될 예정이므로 걍 냅둔다
		if (IsDestroyed()) { return; }
		ASSERT(listener);

		// Component에서 구독 여부 확인을 자체적으로 진행하므로, 중복 여부는 여기서 검사하지 않는다.
		listeners_[(size_t)type].push_back(listener);
	}

	void GameObject::Unsubscribe(SubscribeType type, Component* listener)
	{
		// Destroy 되었으면 어차피 이것도 제거될 예정이므로 걍 냅둔다
		if (IsDestroyed()) { return; }
		ASSERT(listener);

		auto it = std::find(listeners_[(size_t)type].begin(), listeners_[(size_t)type].end(), listener);
		if (it != listeners_[(size_t)type].end()) {
			is_listeners_dirty_[(size_t)type] = true;
			(*it) = nullptr;
		}
	}

	void GameObject::BroadcastTransformDirty(Transform* transform)
	{
		const auto& listeners = listeners_[(size_t)SubscribeType::kTransformDirty];
		for (size_t i = 0; i < listeners.size(); ++i)
		{
			if (listeners[i])
			{
				listeners[i]->OnTransformDirty(transform);
			}
		}
	}

	void GameObject::BroadcastLayerChanged(uint32 prev_layer, uint32 new_layer)
	{
		const auto& listeners = listeners_[(size_t)SubscribeType::kLayerChanged];
		for (size_t i = 0; i < listeners.size(); ++i)
		{
			if (listeners[i])
			{
				listeners[i]->OnLayerChanged(prev_layer, new_layer);
			}
		}
	}

	void GameObject::BroadcastCollisionEnter2D(const Collision2D& col_info)
	{
		const auto& listeners = listeners_[(size_t)SubscribeType::kCollision];
		for(size_t i = 0; i < listeners.size(); ++i)
		{
			if (listeners[i])
			{
				listeners[i]->OnCollisionEnter2D(col_info);
			}
		}
	}

	void GameObject::BroadcastCollisionExit2D(Collider2D * other)
	{
		const auto& listeners = listeners_[(size_t)SubscribeType::kCollision];
		for (size_t i = 0; i < listeners.size(); ++i)
		{
			if (listeners[i])
			{
				listeners[i]->OnCollisionExit2D(other);
			}
		}
	}

	void GameObject::BroadcastTriggerEnter2D(Collider2D * other)
	{
		const auto& listeners = listeners_[(size_t)SubscribeType::kCollision];
		for (size_t i = 0; i < listeners.size(); ++i)
		{
			if (listeners[i])
			{
				listeners[i]->OnTriggerEnter2D(other);
			}
		}
	}

	void GameObject::BroadcastTriggerExit2D(Collider2D * other)
	{
		const auto& listeners = listeners_[(size_t)SubscribeType::kCollision];
		for (size_t i = 0; i < listeners.size(); ++i)
		{
			if (listeners[i])
			{
				listeners[i]->OnTriggerExit2D(other);
			}
		}
	}

	Component* GameObject::AddComponent(std::unique_ptr<Component> component)
	{
		Component* ret = component.get();
		if (component)
		{
			ret->SetOwnerGameObject(this);

			if (component->IsDestroyed()) { return nullptr; }	// 이미 Destroy된 녀석은 넣지 않음

			ComponentCategory cat = component->GetComponentCategory();

			Component* ret = component.get();

			if (ComponentCategory::kScripts < cat)
			{
				if (nullptr == fixed_order_components_[(size_t)cat])
				{
					fixed_order_components_[(size_t)cat] = std::move(component);
					has_pending_components_ = true;
				}
				else
				{
					ASSERT_F(false, "컴포넌트 중복 추가됨. 확인 필요.");
					ret = nullptr;
				}
			}
			else
			{
				other_components_.push_back(std::move(component));
				has_pending_components_ = true;
			}
		}
		return ret;
	}

	void GameObject::UpdateHierarchyState(bool is_active_in_hierarchy)
	{
		if (is_destroyed_) { return; }

		const bool new_is_active_in_hierarchy = is_active_in_hierarchy && is_active_;

		if (is_active_in_hierarchy_ == new_is_active_in_hierarchy) { return; }
		is_active_in_hierarchy_ = new_is_active_in_hierarchy;

		auto UpdateEnableStateFunc = [this](Component* com) -> void
			{
				if (com)
				{
					com->UpdateEnableState(is_active_in_hierarchy_);
				}
			};

		size_t snapshot_size = other_components_.size();
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			UpdateEnableStateFunc(other_components_[i].get());
		}
		snapshot_size = fixed_order_components_.size();
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			UpdateEnableStateFunc(fixed_order_components_[i].get());
		}

		// 중간에 부모자식관계가 변경될 가능성이 있으므로, Value로 떠놓은 뒤 루프에서 다시 한번 확인한다.
		std::vector<Transform*> child_transforms = transform_->GetChildren();
		for(Transform* child_tr : child_transforms)
		{
			ASSERT(child_tr);

			// 다를 경우 부모가 바뀐 것 - 새 hierarchy 쪽에서 처리할 일
			if (child_tr->GetParent() != transform_) { continue; }

			if (GameObject* child = child_tr->GetOwnerGameObject())
			{
				child->UpdateHierarchyState(is_active_in_hierarchy_);
			}
		}
	}

	void GameObject::OnDestroy()
	{
		for (std::unique_ptr<Component>& com : other_components_)
		{
			if (com)
			{
				com->OnDestroy();
			}
		}
		for (std::unique_ptr<Component>& com : fixed_order_components_)
		{
			if (com)
			{
				com->OnDestroy();
			}
		}
	}
}