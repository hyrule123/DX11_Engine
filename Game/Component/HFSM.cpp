#include "Engine/Core/pch.h"
#include "HFSM.h"

#include <Engine/Game/Component/HFSMState.h>
#include <Engine/Game/Component/BlackBoard.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	HFSM::HFSM()
		: Super(HFSM::kClassConcreteName, HFSM::kComponentCategory)
	{
	}
	HFSM::~HFSM()
	{}
	void HFSM::Awake()
	{
		Super::Awake();

		s_ptr<Blackboard> blackboard = GetOwner()->GetComponent<Blackboard>();

		blackboard_ = blackboard;
		ASSERT(!blackboard_.expired());

		ValidateStates();

		// 모든 상태의 ancestor_states_를 갱신
		root_->RefreshAncestorStates({});

		ai_context_.blackboard = blackboard_;
		ai_context_.owner = GetOwner();
		ai_context_.hfsm = std::static_pointer_cast<HFSM>(shared_from_this());

		//등록 순서 보장(DFS)으로 OnAwake 호출
		root_->OnAwakeRecursive(ai_context_);

		const auto& ancestors = current_state_->GetAncestorStates();
		for (HFSMState* state : ancestors)
		{
			state->OnEnter(ai_context_);
		}
	}
	void HFSM::Update()
	{
		Super::Update();

		s_ptr<Blackboard> blackboard = blackboard_.lock();

		if (current_state_)
		{
			const auto& ancestors = current_state_->GetAncestorStates();

			HashedStringView next_state_name = ""_hash;

			for (auto* state : ancestors)
			{
				next_state_name = state->CheckTransition(ai_context_);
				if (!(next_state_name.IsEmpty())) 
				{ 
					//전환될 상태가 반환될 경우 중단 후 즉시 전환
					ChangeState(next_state_name);
					break; 
				}
			}

			// 현재 상태의 ancestor_states_를 순회하며 OnUpdate 호출
			for (HFSMState* state : ancestors)
			{
				state->OnUpdate(ai_context_);
			}
		}
	}

	HFSMState* HFSM::AddState(const HashedStringView& state_name, u_ptr<HFSMState> state)
	{
		ASSERT(false == state_name.IsEmpty());
		ASSERT(state);

		//중복 삽입 방지
		ASSERT(states_.find(state_name) == states_.end());

		HFSMState* ret = state.get();

		state->SetOwnerHFSM(std::static_pointer_cast<HFSM>(shared_from_this()));
		states_.insert(state_name, std::move(state));

		return ret;
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
		ASSERT(root_);
		ASSERT(current_state_);

		for (const auto& pair : states_.cont)
		{
			const auto& state = pair.second;
			ASSERT(state);
			
			// 모든 상태는 부모 상태를 가져야 함(root 상태가 최종 부모여야 함)
			auto* parent = state->GetParentState();

			//부모가 있거나, root 상태일 경우만 유효
			ASSERT(parent || state.get() == root_);
		}
#endif NDEBUG
	}

	void HFSM::ChangeState(const HashedStringView& state_name)
	{
		auto iter = states_.find(state_name);
		s_ptr<Blackboard> blackboard = blackboard_.lock();
		if (iter != states_.end())
		{
			//null check는 삽입시 진행했음
			HFSMState* state = iter->second.get();

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
							prev_hierarchy[j]->OnExit(ai_context_);
						}
						// 들어오는 쪽은 OnEnter 싹 호출
						for (size_t j = i; j < next_hierarchy.size(); ++j)
						{
							next_hierarchy[j]->OnEnter(ai_context_);
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