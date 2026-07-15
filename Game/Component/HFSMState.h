#pragma once
#include <Engine/Core/Entity.h>

#include <Engine/Core/CoreMinimal.h>



#include <Engine/Game/Component/AIContext.h>

namespace engine
{
    class HFSM;

    class HFSMState :
        public Entity
    {
		CLASS_INFO(HFSMState, Entity)
    public:
        HFSMState(const std::string_view concrete_class_name);
		virtual ~HFSMState() override;

		void OnAwakeRecursive(const AIContext& ai_context);

		virtual void OnAwake(const AIContext& ai_context) {};
        virtual void OnEnter(const AIContext& ai_context) {};
        virtual void OnUpdate(const AIContext& ai_context) {};
		virtual void OnExit(const AIContext& ai_context) {};

		virtual HashedStringView CheckTransition(const AIContext& ai_context) { return ""_hash; }

        void SetParentState(HFSMState* parent_state);
		HFSMState* GetParentState() const { return parent_state_; }
		const std::vector<HFSMState*>& GetChildStates() const { return child_states_; }

		const std::vector<HFSMState*>& GetAncestorStates() const { return ancestor_states_; }

        // 재귀, 상위 상태 경로를 갱신
		void RefreshAncestorStates(const std::vector<HFSMState*>& ancestors);

		void SetOwnerHFSM(const s_ptr<HFSM>& hfsm) { owner_hfsm_ = hfsm; }
		s_ptr<HFSM> GetOwnerHFSM() const { return owner_hfsm_.lock(); }

    private:
        void AddChildState(HFSMState* child_state);

		w_ptr<HFSM> owner_hfsm_ = {};

        HFSMState* parent_state_ = {};
		std::vector<HFSMState*> child_states_ = {};

		std::vector<HFSMState*> ancestor_states_ = {};
    };
}


