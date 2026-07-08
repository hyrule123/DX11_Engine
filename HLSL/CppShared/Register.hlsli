#ifndef HLSL_REGISTER
#define HLSL_REGISTER

#include <Engine/HLSL/CppShared/CoreMinimal.hlsli>

#ifdef __HLSL

#define REG_T(slot_num) t##slot_num
#define REG_B(slot_num) b##slot_num
#define REG_S(slot_num) s##slot_num

#endif //__HLSL

#ifdef __cplusplus

#define REG_T(slot_num) slot_num
#define REG_B(slot_num) slot_num
#define REG_S(slot_num) slot_num

#endif //__cplusplus

// =============== T BUFFER =================

// 0 ~ 31: Textures

// 0 ~ 15: Geometry Pass
#define SLOT_T_BASE_COLOR REG_T(0)

// 16 ~ 31: Lighting Pass

// 32 ~ 63: SRV
#define SLOT_T_PER_INSTANCE REG_T(32)

// ==========================================

// =============== B BUFFER =================
#define SLOT_B_PER_INSTANCE REG_B(0)
#define SLOT_B_PER_PASS REG_B(1)

// ==========================================

// =============== S BUFFER =================

#define SLOT_S_POINT_CLAMP REG_S(0)
#define SLOT_S_END 1

// ==========================================

#endif//HLSL_REGISTER