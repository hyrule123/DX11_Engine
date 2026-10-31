#pragma once

#include <Engine/Core/Singleton.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/KeyCode.h>
#include <Engine/Core/Constants.h>
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

        void SetMousePos(int32_t x, int32_t y) { 
            mouse_pos_.x = x, mouse_pos_.y = y; 
        }

        void ClearKeys() { cur_keys_.reset(); }

        bool GetKeyDown(KeyCode code) {
            return down_keys_[(uint8_t)code];
        }

        bool GetKey(KeyCode code) {
            return cur_keys_[(uint8_t)code];
        }

        bool GetKeyUp(KeyCode code) {
            return up_keys_[(uint8_t)code];
        }

        const std::bitset<kKeyCountMax>& GetDownKeys() const { return down_keys_; }
        const std::bitset<kKeyCountMax>& GetKeys() const { return cur_keys_; }
        const std::bitset<kKeyCountMax>& GetUpKeys() const { return up_keys_; }

        // 클라이언트 좌상단 기준 마우스 좌표
        // 버튼을 누른 채(캡처 중) 창 밖으로 나가면 클라이언트 영역 밖 좌표(음수, 창 크기 초과)가 올 수 있음 — 클램프는 사용하는 쪽 책임
        int32_2 GetMousePos() const { return mouse_pos_; }

        // 마우스 버튼만 해제 (캡처를 잃었을 때 — 키보드 상태는 유지)
        void ClearMouseButtons()
        {
            cur_keys_[VK_LBUTTON] = false;
            cur_keys_[VK_RBUTTON] = false;
            cur_keys_[VK_MBUTTON] = false;
        }

	private:
		std::bitset<kKeyCountMax> prev_keys_ = {};
		std::bitset<kKeyCountMax> cur_keys_ = {};

		std::bitset<kKeyCountMax> down_keys_ = {};
		std::bitset<kKeyCountMax> up_keys_ = {};

        int32_2 mouse_pos_ = {};
	};
}



