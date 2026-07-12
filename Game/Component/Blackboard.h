#pragma once
#include <Engine/Game/Component/Component.h>

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Util/StringHashTable.h>

#include <any>

namespace engine
{
    class Blackboard :
        public Component
    {
		CLASS_INFO(Blackboard, Component)
		COMPONENT_CATEGORY(ComponentCategory::kBlackboard)

	public:
		Blackboard();
		virtual ~Blackboard() override;

		virtual void Init() override {};

		template <typename T>
		void SetValue(const HashedStringView& key, T&& value) {
			cont_[key] = std::any(std::forward<T>(value));
		}

		template <typename T>
		T* GetValue(const HashedStringView& key) {
			return std::any_cast<T>(cont_.find(key));
		}

		bool Remove(const HashedStringView& key) {
			return (bool)cont_.erase(key);
		}

	private:
		StringHashTable<std::any> cont_ = {};
    };
}


