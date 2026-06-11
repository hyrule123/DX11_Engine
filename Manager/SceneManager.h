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
		void ChangeScene(s_ptr<Scene> scene) { next_scene_ = scene; };

	private:
		bool Init();

		void Update();
		void Render();

		void ChangeScene();

	private:
		s_ptr<Scene> cur_scene_;
		s_ptr<Scene> next_scene_;
	};
}