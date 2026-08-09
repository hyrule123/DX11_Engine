#include "Engine/Core/pch.h"
#include "HFSM.h"

#include <Engine/Game/Component/HFSMState.h>
#include <Engine/Game/Component/BlackBoard.h>
#include <Engine/Game/Component/Transform.h>

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

		blackboard_ = GetOwnerGameObject()->GetComponent<BlackBoard>();
		ASSERT(!blackboard_.expired());

		wh_ptr<Component> com = blackboard_;
		wh_ptr<BlackBoard> bb = static_handle_cast<BlackBoard>(com);

		int tst = bb.get()->GetTest();

		ValidateStates();

		// 모든 상태의 ancestor_states_를 갱신
		root_->RefreshAncestorStates({});

		ai_context_.black_board = blackboard_.get();
		ai_context_.game_object = GetOwnerGameObject();
		ai_context_.transform = GetOwnerGameObject()->GetComponent<Transform>();

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

		if (current_state_)
		{
			const auto& ancestors = current_state_->GetAncestorStates();

			HashedStringView next_state_name = ""_hash;

			for (HFSMState* state : ancestors)
			{
				next_state_name = state->CheckTransition(ai_context_);

				//State 이름이 비어있지 않고, 현재 State 이름과 다르다면 번환
				if (false == next_state_name.IsEmpty() 
					&&
					current_state_->GetStateName() != next_state_name)
				{
					ChangeState(next_state_name);
					break;
				}
			}
			
			const auto& new_ancestors = current_state_->GetAncestorStates();
			// 현재 상태의 ancestor_states_를 순회하며 OnUpdate 호출
			for (HFSMState* state : new_ancestors)
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

		state->SetOwnerHFSM(this);
		state->SetStateName(state_name);
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
		if (iter == states_.end())
		{
			//의도한 상태일수도 있음. 로그만 찍고 나가기
			DEBUG_LOG("해당하는 State가 없음.");
			return;
		}

		BlackBoard* blackboard = blackboard_.get();
		//null check는 삽입시 진행했음
		HFSMState* state = iter->second.get();

		if (current_state_)
		{
			const auto& prev_hierarchy = current_state_->GetAncestorStates();
			const auto& next_hierarchy = state->GetAncestorStates();

			//간단한 LCA
			size_t min_size = std::min(prev_hierarchy.size(), next_hierarchy.size());
			size_t lca_index = 0;

			for (size_t i = 0; i < min_size; ++i)
			{
				lca_index = i;
				//공통조상 아닌부분 찾기
				if (prev_hierarchy[i] != next_hierarchy[i])
				{
					break;
				}
			}

			// 나가려는 쪽은 OnExit 싹 호출(index -1이 될수있으므로 int32로 변환)
			for (int32 i = (int32)(prev_hierarchy.size()) - 1; i >= (int32)lca_index; --i)
			{
				prev_hierarchy[i]->OnExit(ai_context_);
			}
			// 들어오는 쪽은 OnEnter 싹 호출
			for (size_t i = lca_index; i < next_hierarchy.size(); ++i)
			{
				next_hierarchy[i]->OnEnter(ai_context_);
			}
		}
		current_state_ = state;
	}
}