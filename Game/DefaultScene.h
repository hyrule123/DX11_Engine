#pragma once

#include <Engine/Game/Scene.h>

namespace engine
{

	class DefaultScene
		: public Scene
	{
	public:
		DefaultScene();
		virtual ~DefaultScene() override;

		virtual void Init() override;
		
		virtual void Render() override;

	private:
		s_ptr<class Mesh> test_mesh_;
	};
}