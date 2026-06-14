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
	}

	void GameObject::FrameStart()
	{
		//먼저 싹 다 넣고
		for (const auto& com : pending_add_components_)
		{
			AddComponentInternal(com);
		}

		//Awake 호출(다른 컴포넌트 탐색 보장)
		for (const auto& com : pending_add_components_)
		{
			if (!(com->HasAwaken()))
			{
				com->Awake();
			}
		}

		pending_add_components_.clear();
	}

	void GameObject::Update()
	{
		//DEBUG_LOG("Update 돌고 있습니다");
		for (const auto& com : fixed_order_components_)
		{
			if (com)
			{
				if (!(com->HasStarted()))
				{
					com->Start();
				}

				com->Update();
			}
		}

		for (const auto& com : other_components_)
		{
			if (com)
			{
				com->Update();
			}
		}
	}

	void GameObject::LateUpdate()
	{
		for (const auto& com : fixed_order_components_)
		{
			if (com)
			{
				com->LateUpdate();
			}
		}
			
		for (const auto& com : other_components_)
		{
			if (com)
			{
				com->LateUpdate();
			}
		}
	}

	void GameObject::Render()
	{
		//DEBUG_LOG("Render 돌고 있습니다");
		for (const auto& com : fixed_order_components_)
		{
			if (com)
			{
				com->Render();
			}
		}

		for (const auto& com : other_components_)
		{
			if (com)
			{
				com->Render();
			}
		}
	}

	void GameObject::FrameEnd()
	{
		//컴포넌트 제거 프로세스
	}

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

	void GameObject::AddComponentInternal(const s_ptr<Component>& component)
	{
		//넣을 때 null 체크 했으므로 무조건 있다고 가정

		ComponentCategory cat = component->GetComponentCategory();

		if (cat < ComponentCategory::kOthers)
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

