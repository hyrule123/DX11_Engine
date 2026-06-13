#include "Engine/Core/pch.h"
#include "GameObject.h"

#include <Engine/Game/Component/Transform.h>
#include <Engine/Game/Component/Script.h>
#include <Engine/Game/Component/ComponentCategory.h>



namespace engine
{
	GameObject::GameObject()
		: Super(CLASS_TO_STRING(GameObject))
	{
		//임시 코드(Transform 등록)
		s_ptr<Transform> tf = std::make_shared<Transform>();
		fixed_order_components_[(size_t)ComponentCategory::kTransform] = tf;

		s_ptr<Script> sc = std::make_shared<Script>();
		other_components_.push_back(sc);
	}

	GameObject::~GameObject()
	{}

	s_ptr<Component> GameObject::AddComponent(s_ptr<Component> component)
	{
		if (!component) { return nullptr; }

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

		s_ptr<GameObject> ths = std::static_pointer_cast<GameObject>(shared_from_this());
		component->SetOwner(ths);
		component->Init();
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
}

