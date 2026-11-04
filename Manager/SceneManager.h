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
		void ChangeScene(std::unique_ptr<Scene> scene);
		void ChangeScene(const HashedStringView& concrete_class_name);

		void FrameStart();
		void Update();
		void FixedUpdate();
		void LateUpdate();
		void FrameEnd();

		Scene* GetCurrentScene() const { return cur_scene_.get(); }

	private:
		void ChangeSceneNow();
		void ShutdownCurrentScene();

	private:
		std::unique_ptr<Scene> cur_scene_;
		std::unique_ptr<Scene> next_scene_;
	};
}