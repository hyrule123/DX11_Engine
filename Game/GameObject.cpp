#include "Engine/Core/pch.h"
#include "GameObject.h"

#include <Engine/Game/Component/Transform.h>
#include <Engine/Game/Component/ComponentCategory.h>

namespace engine
{
	GameObject::GameObject()
		: Super(CLASS_TO_STRING(GameObject))
	{
	}

	GameObject::~GameObject()
	{}

	s_ptr<Component> GameObject::AddComponent(s_ptr<Component> component)
	{
		if (component)
		{
			pending_add_components_.push_back(component);
			
			component->SetOwner(std::static_pointer_cast<GameObject>(shared_from_this()));
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

	void GameObject::FrameStart()
	{
		for (const auto& com : pending_add_components_)
		{
			AddComponentInternal(com);
		}

		pending_add_components_.clear();
	}

	void GameObject::AddComponentInternal(const s_ptr<Component>& component)
	{
		//넣을 때 null 체크 했으므로 무조건 있다고 가정

		ComponentCategory cat = component->GetComponentCategory();

		if (cat < ComponentCategory::kOthers)
		{
			if (fixed_order_components_[(size_t)cat])
			{
				DEBUG_LOG("컴포넌트 중복 추가됨. 확인 필요.");
			}

			fixed_order_components_[(size_t)cat] = component;
		}
		else
		{
			other_components_.push_back(component);
		}
	}
}

