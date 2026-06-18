#pragma once

#include <Engine/Core/Singleton.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/KeyCode.h>
#include <Engine/Core/Constant.h>
#include <Engine/Core/Windows.h>

#include <bitset>
#include <array>

namespace engine
{
	class InputManager
	{
		DECLARE_SINGLETON(InputManager)
    private:
        static inline constexpr size_t kKeyCountMax = 256u;

	public:
		void Update();
        
		void SetKey(WPARAM key_param, bool is_down) {
			if (key_param < kKeyCountMax) { cur_keys_[key_param] = is_down; }
		}

        void SetMousePos(int32 x, int32 y) { 
            mouse_pos_.x = x, mouse_pos_.y = y; 
        }

        void ClearKeys() { cur_keys_.reset(); }

        bool GetKeyDown(KeyCode code) {
            return down_keys_[(uint8)code];
        }

        bool GetKey(KeyCode code) {
            return cur_keys_[(uint8)code];
        }

        bool GetKeyUp(KeyCode code) {
            return up_keys_[(uint8)code];
        }

        const std::bitset<kKeyCountMax>& GetDownKeys() const { return down_keys_; }
        const std::bitset<kKeyCountMax>& GetKeys() const { return cur_keys_; }
        const std::bitset<kKeyCountMax>& GetUpKeys() const { return up_keys_; }

        //클라이언트 좌상단 기준, 마우스 상대 좌표를 반환 
        int32_2 GetMousePos() const { return mouse_pos_; }

	private:
		std::bitset<kKeyCountMax> prev_keys_ = {};
		std::bitset<kKeyCountMax> cur_keys_ = {};

		std::bitset<kKeyCountMax> down_keys_ = {};
		std::bitset<kKeyCountMax> up_keys_ = {};

        int32_2 mouse_pos_ = {};
	};
}



