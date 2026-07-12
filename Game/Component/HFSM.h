#pragma once
#include <Engine/Game/Component/Component.h>

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Util/StringHashTable.h>

namespace engine
{
	class HFSMState;

    class HFSM :
        public Component
    {
		CLASS_INFO(HFSM, Component)
		COMPONENT_CATEGORY(ComponentCategory::kBehavior)
    public:
		HFSM();
		virtual ~HFSM() override;
		virtual void Init() override {};

		virtual void Awake() override;
		virtual void Update() override;

		void AddState(const HashedStringView& state_name, u_ptr<HFSMState> state);

		void SetInitialState(const HashedStringView& state_name);

		HFSMState* GetRootState() const { return root_.get(); }

		void ValidateStates() const;

	private:
		void ChangeState(const HashedStringView& state_name);

		StringHashTable<u_ptr<HFSMState>> states_ = {};

		u_ptr<HFSMState> root_ = {};

		HFSMState* current_state_ = {};
    };
}


