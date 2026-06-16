#pragma once

#include <Engine/Core/Singleton.h>

#include <Engine/Core/KeyCode.h>
#include <Engine/Core/Constant.h>
#include <Engine/Core/Windows.h>

#include <bitset>

namespace engine
{
	class InputManager
	{
		DECLARE_SINGLETON(InputManager)
    private:
        static inline constexpr size_t kKeyCountMax = 256u;

	public:
		void Update();
        
		inline void SetCurKey(WPARAM key_param, bool is_down) {
			if (key_param < kKeyCountMax) { cur_keys_[key_param] = is_down; }
		}

        inline void ClearCurKey() { cur_keys_.reset(); }

        inline bool IsDown(KeyCode code) {
            return down_keys_[(uint8)code];
        }

        inline bool IsPressed(KeyCode code) {
            return pressed_keys_[(uint8)code];
        }

        inline bool IsUp(KeyCode code) {
            return up_keys_[(uint8)code];
        }

        const std::bitset<kKeyCountMax>& GetDownKeys() const { return down_keys_; }
        const std::bitset<kKeyCountMax>& GetPressedKeys() const { return pressed_keys_; }
        const std::bitset<kKeyCountMax>& GetUpKeys() const { return up_keys_; }

	private:
		std::bitset<kKeyCountMax> prev_keys_ = {};
		std::bitset<kKeyCountMax> cur_keys_ = {};

		std::bitset<kKeyCountMax> down_keys_ = {};
		std::bitset<kKeyCountMax> pressed_keys_ = {};
		std::bitset<kKeyCountMax> up_keys_ = {};
	};
}



