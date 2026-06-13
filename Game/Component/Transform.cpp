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

		DEBUG_LOG("Transform Init 호출!!");
	}

	void Transform::Awake()
	{
		Super::Awake();

		DEBUG_LOG("Transform Awake 호출!!");
	}


}