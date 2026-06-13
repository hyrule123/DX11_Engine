#include "Engine/Core/pch.h"
#include "Transform.h"

namespace engine
{
	Transform::Transform()
		: Component("Transform", ComponentCategory::kTransform)
	{
	}
	Transform::~Transform()
	{
	}
	void Transform::Init()
	{
	}
}