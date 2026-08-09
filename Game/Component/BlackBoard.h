#pragma once
#include <Engine/Game/Component/Component.h>

#include <Engine/Core/CoreMinimal.h>



#include <any>

namespace engine
{
    class BlackBoard :
        public Component
    {
		ENTITY_INFO(BlackBoard, Component)
		COMPONENT_CATEGORY(ComponentCategory::kBlackboard)

	public:
		BlackBoard();
		virtual ~BlackBoard() override;

		virtual void Init() override {};

		template <typename T>
		void SetValue(const HashedStringView& key, T&& value) {
			cont_[key] = std::any(std::forward<T>(value));
		}

		template <typename T>
		T* GetValue(const HashedStringView& key) {
			auto iter = cont_.find(key);
			if (iter != cont_.end()) {
				return std::any_cast<T>(&iter->second);
			}
			return nullptr;
		}

		bool Remove(const HashedStringView& key) {
			return (bool)cont_.erase(key);
		}

		int GetTest() const { return test; }

	private:
		StringHashTable<std::any> cont_ = {};

		int test = 1;
    };
}


