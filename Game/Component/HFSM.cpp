#include "Engine/Core/pch.h"
#include "HFSM.h"

#include <Engine/Game/Component/HFSMState.h>
#include <Engine/Game/Component/Blackboard.h>

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

		blackboard_ = GetOwner()->GetComponent<Blackboard>();
		ASSERT(!blackboard_.expired());

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

			HashedStringView next_state_name = current_state_->CheckTransition();
			if (!next_state_name.IsEmpty())
			{
				ChangeState(next_state_name);
			}
		}
	}

	void HFSM::AddState(const HashedStringView& state_name, u_ptr<HFSMState> state)
	{
		ASSERT(false == state_name.IsEmpty());
		ASSERT(state);

		//중복 삽입 방지
		ASSERT(nullptr == states_.find(state_name));

		state->SetOwnerHFSM(std::static_pointer_cast<HFSM>(shared_from_this()));
		states_.insert(state_name, std::move(state));
	}

	void HFSM::SetInitialState(const HashedStringView& state_name)
	{
		u_ptr<HFSMState>* state = states_.find(state_name);
		if (state)
		{
			current_state_ = state->get();
		}
		ASSERT_MESSAGE(current_state_, "State not found");
	}

	void HFSM::ValidateStates() const
	{
#ifndef NDEBUG
		ASSERT(current_state_);

		for (const auto& pair : states_.cont)
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
		u_ptr<HFSMState>* state_pp = states_.find(state_name);
		if (state_pp)
		{
			//null check는 삽입시 진행했음
			HFSMState* state = state_pp->get();

			if (current_state_)
			{
				const auto& prev_hierarchy = current_state_->GetAncestorStates();
				const auto& next_hierarchy = state->GetAncestorStates();

				//간단한 LCA
				size_t min_size = std::min(prev_hierarchy.size(), next_hierarchy.size());

				for (size_t i = 0; i < min_size; ++i)
				{
					//공통조상 아닌부분 찾기
					if (prev_hierarchy[i] != next_hierarchy[i])
					{
						// 나가려는 쪽은 OnExit 싹 호출
						for (size_t j = prev_hierarchy.size() - 1; j >= i; --j)
						{
							prev_hierarchy[j]->OnExit();
						}
						// 들어오는 쪽은 OnEnter 싹 호출
						for (size_t j = i; j < next_hierarchy.size(); ++j)
						{
							next_hierarchy[j]->OnEnter();
						}
						break;
					}
				}
			}
			current_state_ = state;
		}
		ASSERT_MESSAGE(current_state_, "State not found");
	}
}