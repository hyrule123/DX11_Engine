#pragma once

#include <Engine/Core/Common.h>

#include <Engine/Core/Singleton.h>

namespace engine
{
	class Scene;
	class SceneManager
	{
		DECLARE_SINGLETON(SceneManager)

		friend class GameEngine;

	public:
		void ChangeScene(s_ptr<Scene> scene);

	private:
		void Init();

		void FrameStart();
		void Update();
		void Render();

		void ChangeSceneNow();

	private:
		s_ptr<Scene> cur_scene_;
		s_ptr<Scene> next_scene_;
	};
}