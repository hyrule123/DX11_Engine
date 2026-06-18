#ifndef HLSL_REGISTER
#define HLSL_REGISTER

#include <Engine/HLSL/CppShared/CoreMinimal.hlsli>

#ifdef __HLSL

#define REG_T(slot_num) t##slot_num
#define REG_C(slot_num) c##slot_num

#endif //__HLSL

#ifdef __cplusplus

#define REG_T(slot_num) slot_num
#define REG_C(slot_num) slot_num

#endif //__cplusplus

// =============== T BUFFER =================

// 0 ~ 31: Textures

// 0 ~ 15: Geometry Pass
#define SLOT_T_BASE_COLOR REG_T(0)

// 16 ~ 31: Lighting Pass

// 32 ~ 63: SRV

// ==========================================

// =============== C BUFFER =================
#define SLOT_C_WVP REG_C(0)

// ==========================================

#endif//HLSL_REGISTER