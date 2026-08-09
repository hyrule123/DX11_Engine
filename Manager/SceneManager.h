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
		void ChangeScene(u_ptr<Scene> scene);

		void FrameStart();
		void Update();
		void FixedUpdate();
		void LateUpdate();
		void FrameEnd();

		void ChangeSceneNow();

	private:
		u_ptr<Scene> cur_scene_;
		u_ptr<Scene> next_scene_;
	};
}