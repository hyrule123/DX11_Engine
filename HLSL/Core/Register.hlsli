#ifndef HLSL_REGISTER
#define HLSL_REGISTER

#include <Engine/HLSL/Core/CoreMinimal.hlsli>

#ifdef __HLSL

#define REG_IMPL(slot_type, slot_num) slot_type##slot_num
#define REG_T(slot_num) REG_IMPL(t, slot_num)
#define REG_B(slot_num) REG_IMPL(b, slot_num)
#define REG_S(slot_num) REG_IMPL(s, slot_num)
#define REG_U(slot_num) REG_IMPL(u, slot_num)

#endif //__HLSL

#ifdef __cplusplus

#define REG_T(slot_num) slot_num
#define REG_B(slot_num) slot_num
#define REG_S(slot_num) slot_num
#define REG_U(slot_num) slot_num

#endif //__cplusplus

// =============== B BUFFER =================

// Per Frame
#define REG_B_PER_FRAME_START      0    // b0~b1  (2칸)
#define REG_B_PER_FRAME_0          REG_B(0)
#define REG_B_PER_FRAME_1          REG_B(1)
#define REG_B_PER_FRAME_END        REG_B_PER_PASS_START
#define REG_B_PER_FRAME_COUNT 	(REG_B_PER_FRAME_END - REG_B_PER_FRAME_START)

// Per Render Pass
#define REG_B_PER_PASS_START       2   // b2~b4  (3칸)
#define REG_B_PER_PASS_0           REG_B(2)	// Camera 예약됨
#define REG_B_PER_PASS_1           REG_B(3)
#define REG_B_PER_PASS_2           REG_B(4)
#define REG_B_PER_PASS_END         REG_B_PER_PIPELINE_START
#define REG_B_PER_PASS_COUNT 	(REG_B_PER_PASS_END - REG_B_PER_PASS_START)

// Per Pipeline State
#define REG_B_PER_PIPELINE_START   5    // b5~b6  (2칸)
#define REG_B_PER_PIPELINE_0       REG_B(5)
#define REG_B_PER_PIPELINE_1       REG_B(6)
#define REG_B_PER_PIPELINE_END     REG_B_PER_MATERIAL_START
#define REG_B_PER_PIPELINE_COUNT 	(REG_B_PER_PIPELINE_END - REG_B_PER_PIPELINE_START)

// Per Material
#define REG_B_PER_MATERIAL_START   7    // b7~b10 (4칸)
#define REG_B_PER_MATERIAL_0       REG_B(7)
#define REG_B_PER_MATERIAL_1       REG_B(8)
#define REG_B_PER_MATERIAL_2       REG_B(9)
#define REG_B_PER_MATERIAL_3       REG_B(10)
#define REG_B_PER_MATERIAL_END     REG_B_PER_OBJECT_START
#define REG_B_PER_MATERIAL_COUNT 	(REG_B_PER_MATERIAL_END - REG_B_PER_MATERIAL_START)

// Per Draw / Dispatch
#define REG_B_PER_OBJECT_START   11   // b11~b13 (3칸)
#define REG_B_PER_OBJECT_0       REG_B(11)
#define REG_B_PER_OBJECT_1       REG_B(12)
#define REG_B_PER_OBJECT_2       REG_B(13)
#define REG_B_PER_OBJECT_END     14
#define REG_B_PER_OBJECT_COUNT 	(REG_B_PER_OBJECT_END - REG_B_PER_OBJECT_START)
// ==========================================


// =============== T BUFFER =================
// Bind 빈도 순으로 10개씩 배치
// 규약: [START, END) — END는 미포함, 다음 구간의 START와 동일
// 변경 빈도순 배치: Frame → Pass → Pipeline → Material → Instance

// !!!중요사항!!!
// 아래 매크로를 한 단계 더 추상화하여 사용할 것
// #define REG_T_TEXTURE REG_T_PER_FRAME_1

// Per Frame
#define REG_T_PER_FRAME_START      0
#define REG_T_PER_FRAME_0          REG_T(0)
#define REG_T_PER_FRAME_1          REG_T(1)
#define REG_T_PER_FRAME_2          REG_T(2)
#define REG_T_PER_FRAME_3          REG_T(3)
#define REG_T_PER_FRAME_4          REG_T(4)
#define REG_T_PER_FRAME_5          REG_T(5)
#define REG_T_PER_FRAME_6          REG_T(6)
#define REG_T_PER_FRAME_7          REG_T(7)
#define REG_T_PER_FRAME_8          REG_T(8)
#define REG_T_PER_FRAME_9          REG_T(9)
#define REG_T_PER_FRAME_END        REG_T_PER_PASS_START
#define REG_T_PER_FRAME_COUNT      (REG_T_PER_FRAME_END - REG_T_PER_FRAME_START)

// Per Render Pass
#define REG_T_PER_PASS_START       10
#define REG_T_PER_PASS_0           REG_T(10)
#define REG_T_PER_PASS_1           REG_T(11)
#define REG_T_PER_PASS_2           REG_T(12)
#define REG_T_PER_PASS_3           REG_T(13)
#define REG_T_PER_PASS_4           REG_T(14)
#define REG_T_PER_PASS_5           REG_T(15)
#define REG_T_PER_PASS_6           REG_T(16)
#define REG_T_PER_PASS_7           REG_T(17)
#define REG_T_PER_PASS_8           REG_T(18)
#define REG_T_PER_PASS_9           REG_T(19)
#define REG_T_PER_PASS_END         REG_T_PER_PIPELINE_START
#define REG_T_PER_PASS_COUNT 	 (REG_T_PER_PASS_END - REG_T_PER_PASS_START)

// Per Pipeline State
#define REG_T_PER_PIPELINE_START   20
#define REG_T_PER_PIPELINE_0       REG_T(20)
#define REG_T_PER_PIPELINE_1       REG_T(21)
#define REG_T_PER_PIPELINE_2       REG_T(22)
#define REG_T_PER_PIPELINE_3       REG_T(23)
#define REG_T_PER_PIPELINE_4       REG_T(24)
#define REG_T_PER_PIPELINE_5       REG_T(25)
#define REG_T_PER_PIPELINE_6       REG_T(26)
#define REG_T_PER_PIPELINE_7       REG_T(27)
#define REG_T_PER_PIPELINE_8       REG_T(28)
#define REG_T_PER_PIPELINE_9       REG_T(29)
#define REG_T_PER_PIPELINE_END     REG_T_PER_MATERIAL_START
#define REG_T_PER_PIPELINE_COUNT  (REG_T_PER_PIPELINE_END - REG_T_PER_PIPELINE_START)

// Per Material
#define REG_T_PER_MATERIAL_START   30
#define REG_T_PER_MATERIAL_0       REG_T(30)
#define REG_T_PER_MATERIAL_1       REG_T(31)
#define REG_T_PER_MATERIAL_2       REG_T(32)
#define REG_T_PER_MATERIAL_3       REG_T(33)
#define REG_T_PER_MATERIAL_4       REG_T(34)
#define REG_T_PER_MATERIAL_5       REG_T(35)
#define REG_T_PER_MATERIAL_6       REG_T(36)
#define REG_T_PER_MATERIAL_7       REG_T(37)
#define REG_T_PER_MATERIAL_8       REG_T(38)
#define REG_T_PER_MATERIAL_9       REG_T(39)
#define REG_T_PER_MATERIAL_END     REG_T_PER_OBJECT_START
#define REG_T_PER_MATERIAL_COUNT  (REG_T_PER_MATERIAL_END - REG_T_PER_MATERIAL_START)

// Per Draw / Dispatch
#define REG_T_PER_OBJECT_START   40
#define REG_T_PER_OBJECT_0       REG_T(40)
#define REG_T_PER_OBJECT_1       REG_T(41)
#define REG_T_PER_OBJECT_2       REG_T(42)
#define REG_T_PER_OBJECT_3       REG_T(43)
#define REG_T_PER_OBJECT_4       REG_T(44)
#define REG_T_PER_OBJECT_5       REG_T(45)
#define REG_T_PER_OBJECT_6       REG_T(46)
#define REG_T_PER_OBJECT_7       REG_T(47)
#define REG_T_PER_OBJECT_8       REG_T(48)
#define REG_T_PER_OBJECT_9       REG_T(49)
#define REG_T_PER_OBJECT_END     50
#define REG_T_PER_OBJECT_COUNT  (REG_T_PER_OBJECT_END - REG_T_PER_OBJECT_START)
// ==========================================



// =============== S BUFFER =================

#define REG_S_POINT_CLAMP REG_S(0)
#define REG_S_END 1

// ==========================================

#endif//HLSL_REGISTER