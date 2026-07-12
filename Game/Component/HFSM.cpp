#include "Engine/Core/pch.h"
#include "HFSM.h"

#include <Engine/Game/Component/HFSMState.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	HFSM::HFSM()
		: Super(HFSM::kClassConcreteName, HFSM::kComponentCategory)
	{
		root_ = std::make_unique<HFSMState>("RootState");
	}
	HFSM::~HFSM()
	{}
	void HFSM::Awake()
	{
		Super::Awake();

		// 모든 상태의 ancestor_states_를 갱신
		root_->RefreshAncestorStates({});

		ValidateStates();

		const auto& ancestors = current_state_->GetAncestorStates();
		for (HFSMState* state : ancestors)
		{
			state->OnEnter();
		}
	}
	void HFSM::Update()
	{
		Super::Update();

		if (current_state_)
		{
			const auto& ancestors = current_state_->GetAncestorStates();

			for (HFSMState* state : ancestors)
			{
				state->OnUpdate();
			}

			HashedStringView* next_state_name = current_state_->CheckTransition();
			if (next_state_name)
			{
				ChangeState(*next_state_name);
			}
		}
	}

	void HFSM::AddState(const HashedStringView& state_name, u_ptr<HFSMState> state)
	{
		ASSERT(false == state_name.IsEmpty());
		ASSERT(state);

		//중복 삽입 방지
		ASSERT(states_.find(state_name) == states_.end());

		state->SetOwnerHFSM(std::static_pointer_cast<HFSM>(shared_from_this()));
		states_.insert(std::make_pair(state_name.GetStringView(), std::move(state)));
	}

	void HFSM::SetInitialState(const HashedStringView& state_name)
	{
		auto iter = states_.find(state_name);
		if (iter != states_.end())
		{
			current_state_ = iter->second.get();
		}
		ASSERT_MESSAGE(current_state_, "State not found");
	}

	void HFSM::ValidateStates() const
	{
#ifndef NDEBUG
		ASSERT(current_state_);

		for (const auto& pair : states_)
		{
			const auto& state = pair.second;
			ASSERT(state);
			
			// 모든 상태는 부모 상태를 가져야 함(root 상태가 최종 부모여야 함)
			auto* parent = state->GetParentState();
			ASSERT(parent);
		}
#endif NDEBUG
	}

	void HFSM::ChangeState(const HashedStringView& state_name)
	{
		auto iter = states_.find(state_name);
		if (iter != states_.end())
		{
			if (current_state_)
			{
				const auto& hierarchy = current_state_->GetAncestorStates();

				for (auto* state : hierarchy)
				{
					state->OnExit();
				}
			}
			current_state_ = iter->second.get();
		}

		ASSERT_MESSAGE(current_state_, "State not found");
	}
}