#pragma once

#include <Engine/Core/Singleton.h>

#include <Engine/Core/CoreMinimal.h>

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
		void LateUpdate();
		void Render();

		void ChangeSceneNow();

	private:
		s_ptr<Scene> cur_scene_;
		s_ptr<Scene> next_scene_;
	};
}