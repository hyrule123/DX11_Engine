#include "Engine/Core/pch.h"
#include "HFSMState.h"

#include <Engine/Core/Debug.h>

namespace engine
{
	HFSMState::HFSMState(const std::string_view concrete_class_name)
		: Super(concrete_class_name)
	{}
	HFSMState::~HFSMState()
	{}
	void HFSMState::SetParentState(HFSMState* parent_state)
	{
		ASSERT(this != parent_state);

		//일단은 parent_state_가 nullptr이 아니면 안되게 막아둠. 나중에 필요하면 수정
		ASSERT(nullptr == parent_state_);
		parent_state_ = parent_state;

		if (parent_state_)
		{
			parent_state_->AddChildState(this);
		}
	}


	void HFSMState::RefreshAncestorStates(const std::vector<HFSMState*>& ancestors)
	{
		ancestor_states_.clear();
		ancestor_states_.reserve(ancestors.size() + 1);
		ancestor_states_.insert(ancestor_states_.end(), ancestors.begin(), ancestors.end());
		ancestor_states_.push_back(this);

		for (HFSMState* child : child_states_)
		{
			child->RefreshAncestorStates(ancestor_states_);
		}
	}

	void HFSMState::AddChildState(HFSMState* child_state)
	{
		ASSERT(this != child_state);
		if (child_state)
		{
			child_states_.push_back(child_state);
		}
	}
}