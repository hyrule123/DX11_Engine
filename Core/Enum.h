#pragma once
#include <Engine/Util/EnumFlags.h>

#include <Engine/Core/CoreTypes.h>
#include <Engine/Core/Debug.h>


namespace engine
{
	enum class SubscribeType : uint8
	{
		kCollision,
		kTransformDirty,
		kLayerChanged,
		kEND
	};
}






