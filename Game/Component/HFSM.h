#pragma once
#include <Engine/Game/Component/Component.h>

#include <Engine/Core/CoreMinimal.h>



#include <Engine/Game/Component/AIContext.h>

namespace engine
{
	class HFSMState;
	class BlackBoard;

    class HFSM :
        public Component
    {
		ENTITY_INFO(HFSM, Component)
		COMPONENT_CATEGORY(ComponentCategory::kBehavior)
    public:
		HFSM();
		virtual ~HFSM() override;
		virtual void Init() override {};

		virtual void Awake() override;
		virtual void Update() override;

		HFSMState* AddState(const HashedStringView& state_name, u_ptr<HFSMState> state);

		void SetRootState(HFSMState* root_state) { root_ = root_state; }

		void SetInitialState(const HashedStringView& state_name);

		HFSMState* GetRootState() const { return root_; }

		BlackBoard* GetBlackboard() const { return blackboard_.get(); }

	private:
		void ValidateStates() const;
		void ChangeState(const HashedStringView& state_name);

		StringHashTable<u_ptr<HFSMState>> states_ = {};

		HFSMState* root_ = {};

		HFSMState* current_state_ = {};

		wh_ptr<BlackBoard> blackboard_ = {};

		AIContext ai_context_ = {};
    };
}


