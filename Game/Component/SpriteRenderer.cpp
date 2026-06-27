#include "Engine/Core/pch.h"
#include "SpriteRenderer.h"

#include <Engine/Core/Debug.h>

namespace engine
{
	SpriteRenderer::SpriteRenderer()
		: Super(STRINGIFY(SpriteRenderer))
	{}
	SpriteRenderer::~SpriteRenderer()
	{}

	void SpriteRenderer::Init()
	{
		Super::Init();
		bool result = SetMesh("Sprite_Mesh");

		ASSERT(result);
	}
}