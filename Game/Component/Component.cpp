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
		has_initialized_ = true;
	}
	void Component::Awake()
	{
		has_awaken_ = true;
	}
	void Component::Start()
	{
		has_started_ = true;
	}

	void Component::Destroy()
	{
		if (is_destroyed_) { return; }
		is_destroyed_ = true;
		is_enabled_ = false;

		//Awake 이후, 활성화 상태였다면 OnDisable 호출
		if(has_awaken_ && is_enabled_and_active_in_hierarchy_)
		{
			is_enabled_and_active_in_hierarchy_ = false;
			OnDisable();
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