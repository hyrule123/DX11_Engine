#include "Engine/Core/pch.h"
#include "Transform.h"

namespace engine
{
	Transform::Transform()
		: Super(CLASS_TO_STRING(Transform), ComponentCategory::kTransform)
	{
	}
	Transform::~Transform()
	{
	}
	void Transform::Init()
	{
		Super::Init();
	}
}