#include "Engine/Core/pch.h"
#include "Component.h"

#include <Engine/Core/Debug.h>

namespace engine
{
	Component::Component(const HashedStringView& concrete_class_name, ComponentCategory category)
		: Super(concrete_class_name)
		, category_(category)
	{}

	Component::~Component()
	{}

	void Component::Init()
	{
		Super::Init();
		has_initialized_ = true;
	}

	void Component::BeginPlay()
	{
		has_begun_play_ = true;
	}

	void Component::Destroy()
	{
		if (IsDestroyed()) { return; }
		is_destroyed_ = true;
		is_enabled_ = false;

		//Awake 이후, 활성화 상태였다면 OnDisable 호출
		if (is_enabled_and_active_in_hierarchy_)
		{
			is_enabled_and_active_in_hierarchy_ = false;
			if (has_awaken_)
			{
				OnDisable();
			}
		}

		InvalidateHandle();
		UnsubscribeAll();
	}

	void Component::Subscribe(SubscribeType type)
	{
		// 이미 구독중일 경우 return
		if (IsDestroyed() || is_subscribing_.test((size_t)type)) { return; }
		is_subscribing_.set((size_t)type, true);
		if (has_awaken_)
		{
			GameObject* owner = GetOwnerGameObject();
			ASSERT(owner);
			owner->Subscribe(type, this);
		}
	}

	void Component::Unsubscribe(SubscribeType type)
	{
		if (IsDestroyed() || is_subscribing_.test((size_t)type) == false) { return; }
		is_subscribing_.set((size_t)type, false);
		if (has_awaken_)
		{
			GameObject* owner = GetOwnerGameObject();
			ASSERT(owner);
			owner->Unsubscribe(type, this);
		}
	}

	void Component::UnsubscribeAll()
	{
		for (uint32 i = 0; i < (uint32)SubscribeType::kEND; ++i)
		{
			if (is_subscribing_.test(i))
			{
				Unsubscribe((SubscribeType)i);
			}
		}
	}

	void Component::ReplaySubscriptions()
	{
		GameObject* owner = GetOwnerGameObject();
		ASSERT(owner);
		for (size_t i = 0; i < (size_t)SubscribeType::kEND; ++i)
		{
			if (is_subscribing_.test(i))
			{
				owner->Subscribe((SubscribeType)i, this);
			}
		}
	}

	void Component::UpdateEnableState(bool is_active_in_hierarchy)
	{
		if (is_destroyed_) { return; }

		const bool new_state = is_enabled_ && is_active_in_hierarchy;

		if (new_state == is_enabled_and_active_in_hierarchy_) { return; }
		
		is_enabled_and_active_in_hierarchy_ = new_state;

		// Awake 호출 전에는 OnEnable/OnDisable 호출하지 않음
		// 사용자가 Scene 돌기 전 비활성화 상태로 넣어놓을 수도 있기 때문
		if (has_awaken_ && new_state)
		{
			OnEnable();
		}
		else
		{
			OnDisable();
		}
	}
}