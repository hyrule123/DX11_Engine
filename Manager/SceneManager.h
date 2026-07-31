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
	private:
		void Init();

	public:
		void ChangeScene(s_ptr<Scene> scene);

		void FrameStart();
		void Update();
		void FixedUpdate();
		void LateUpdate();
		void FrameEnd();

		void ChangeSceneNow();

	private:
		s_ptr<Scene> cur_scene_;
		s_ptr<Scene> next_scene_;
	};
}