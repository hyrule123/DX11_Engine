#pragma once

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Core/Windows.h>

#include <string_view>
#include <array>

namespace engine
{
	constexpr size_t kMaxKeyCount = 256;

	enum class KeyCode : uint8 {
		kNone = 0x00,

		// ==========================================
		// 마우스 버튼
		// ==========================================
		kMouseLeft = VK_LBUTTON,  // 0x01
		kMouseRight = VK_RBUTTON,  // 0x02
		kMouseMiddle = VK_MBUTTON,  // 0x04

		// ==========================================
		// 시스템 및 제어 키
		// ==========================================
		kEscape = VK_ESCAPE,
		kEnter = VK_RETURN,
		kSpace = VK_SPACE,
		kTab = VK_TAB,
		kBackspace = VK_BACK,

		// 모디파이어 (조합키)
		kShift = VK_SHIFT,
		kControl = VK_CONTROL,
		kAlt = VK_MENU,       // Win32 API에서 Alt 키는 VK_MENU

		// ==========================================
		// 방향키
		// ==========================================
		kLeft = VK_LEFT,
		kRight = VK_RIGHT,
		kUp = VK_UP,
		kDown = VK_DOWN,

		// ==========================================
		// 숫자 키 (상단 배열) - ASCII 값과 동일 ('0' ~ '9')
		// ==========================================
		kNum0 = '0', kNum1 = '1', kNum2 = '2', kNum3 = '3', kNum4 = '4',
		kNum5 = '5', kNum6 = '6', kNum7 = '7', kNum8 = '8', kNum9 = '9',

		// ==========================================
		// 알파벳 키 - 대문자 ASCII 값과 동일 ('A' ~ 'Z')
		// ==========================================
		kA = 'A', kB = 'B', kC = 'C', kD = 'D', kE = 'E',
		kF = 'F', kG = 'G', kH = 'H', kI = 'I', kJ = 'J',
		kK = 'K', kL = 'L', kM = 'M', kN = 'N', kO = 'O',
		kP = 'P', kQ = 'Q', kR = 'R', kS = 'S', kT = 'T',
		kU = 'U', kV = 'V', kW = 'W', kX = 'X', kY = 'Y', kZ = 'Z',

		// ==========================================
		// F1 ~ F12 펑션 키
		// ==========================================
		kF1 = VK_F1, kF2 = VK_F2, kF3 = VK_F3, kF4 = VK_F4,
		kF5 = VK_F5, kF6 = VK_F6, kF7 = VK_F7, kF8 = VK_F8,
		kF9 = VK_F9, kF10 = VK_F10, kF11 = VK_F11, kF12 = VK_F12
	};

	//Immediately Invoked Lambda Expression, IILE
	constexpr std::array<std::string_view, kMaxKeyCount> kKeyCodeNames = []() 
		{
		std::array<std::string_view, kMaxKeyCount> names{};

		// 마우스 버튼
		names[VK_LBUTTON] = "Mouse Left";
		names[VK_RBUTTON] = "Mouse Right";
		names[VK_MBUTTON] = "Mouse Middle";
		names[VK_XBUTTON1] = "Mouse X1";
		names[VK_XBUTTON2] = "Mouse X2";

		// 특수 제어 키
		names[VK_BACK] = "Backspace";
		names[VK_TAB] = "Tab";
		names[VK_RETURN] = "Enter";
		names[VK_SHIFT] = "Shift";
		names[VK_CONTROL] = "Ctrl";
		names[VK_MENU] = "Alt";
		names[VK_CAPITAL] = "Caps Lock";
		names[VK_ESCAPE] = "Escape";
		names[VK_SPACE] = "Space";
		names[VK_DELETE] = "Delete";

		// 방향키
		names[VK_LEFT] = "Left";
		names[VK_UP] = "Up";
		names[VK_RIGHT] = "Right";
		names[VK_DOWN] = "Down";

		// 상단 숫자 키 (0~9)
		names['0'] = "0"; names['1'] = "1"; names['2'] = "2"; names['3'] = "3"; names['4'] = "4";
		names['5'] = "5"; names['6'] = "6"; names['7'] = "7"; names['8'] = "8"; names['9'] = "9";

		// 알파벳 키 (A~Z)
		names['A'] = "A"; names['B'] = "B"; names['C'] = "C"; names['D'] = "D"; names['E'] = "E";
		names['F'] = "F"; names['G'] = "G"; names['H'] = "H"; names['I'] = "I"; names['J'] = "J";
		names['K'] = "K"; names['L'] = "L"; names['M'] = "M"; names['N'] = "N"; names['O'] = "O";
		names['P'] = "P"; names['Q'] = "Q"; names['R'] = "R"; names['S'] = "S"; names['T'] = "T";
		names['U'] = "U"; names['V'] = "V"; names['W'] = "W"; names['X'] = "X"; names['Y'] = "Y"; names['Z'] = "Z";

		// F1 ~ F12
		names[VK_F1] = "F1";   names[VK_F2] = "F2";   names[VK_F3] = "F3";   names[VK_F4] = "F4";
		names[VK_F5] = "F5";   names[VK_F6] = "F6";   names[VK_F7] = "F7";   names[VK_F8] = "F8";
		names[VK_F9] = "F9";   names[VK_F10] = "F10"; names[VK_F11] = "F11"; names[VK_F12] = "F12";


		// 기본값 세팅
		for (size_t i = 0; i < kMaxKeyCount; ++i) {
			if (names[i].empty())
			{
				names[i] = "Unknown";
			}
		}

		return names;
		}();
}