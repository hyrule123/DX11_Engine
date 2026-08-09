#include "Engine/Core/pch.h"
#include "Component.h"

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
	void Component::Awake()
	{
		has_awaken_ = true;
	}
	void Component::OnEnable()
	{
		if (has_collision_subscribed_)
		{
			RegisterCollisionListener();
		}
	}
	void Component::BeginPlay()
	{
		has_begun_play_ = true;
	}

	void Component::OnDisable()
	{
		if (has_collision_subscribed_)
		{
			UnregisterCollisionListener();
		}
	}

	void Component::Destroy()
	{
		if (IsDestroyed()) { return; }
		is_destroyed_ = true;
		is_enabled_ = false;
		InvalidateHandle();

		//Awake 이후, 활성화 상태였다면 OnDisable 호출
		if (is_enabled_and_active_in_hierarchy_)
		{
			is_enabled_and_active_in_hierarchy_ = false;
			if (has_awaken_)
			{
				OnDisable();
			}
		}
	}

	void Component::SubscribeCollisionEvents(bool subscribe)
	{
		if (subscribe == has_collision_subscribed_) { return; }
		has_collision_subscribed_ = subscribe;

		// Awake 이후일 경우 GameObject에 등록/해제
		if (has_collision_subscribed_)
		{
			RegisterCollisionListener();
		}
		else
		{
			UnregisterCollisionListener();
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
	void Component::RegisterCollisionListener()
	{
		auto owner = GetOwnerGameObject();
		//owner이 아직 등록되지 않은 시점에는 연기(OnEnable에서 다시 호출됨)
		if (owner && has_collision_sub_registered_ == false)
		{
			has_collision_sub_registered_ = true;
			owner->AddCollisionListener(this);
		}
	}
	void Component::UnregisterCollisionListener()
	{
		auto owner = GetOwnerGameObject();
		if (owner && has_collision_sub_registered_)
		{
			has_collision_sub_registered_ = false;
			owner->RemoveCollisionListener(this);
		}
	}
}