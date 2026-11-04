#pragma once
#include <Engine/Game/SceneEntity.h>

#include <Engine/Core/UtilMacro.h>
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constants.h>
#include <Engine/Core/Debug.h>

#include <Engine/Render/RenderPass/RenderPassTypes.h>

#include <string_view>
#include <vector>

#include <type_traits>
#include <bitset>

namespace engine
{
	class GameObject;
	class Camera;
	class Renderer;
	class SceneEntity;
	class CollisionSystem2D;

	class Scene 
		: public SceneEntity
	{
		ENTITY_INFO(Scene, SceneEntity);

		friend class SceneManager;
	public:
		Scene(const HashedStringView& concrete_class_name);
		virtual ~Scene() override;

		//추가 예약, 실제 추가는 FlushPendingGameObjects() 타이밍
		template <typename T> requires std::is_base_of_v<GameObject, T>
		T* AddGameObject() {
			return static_cast<T*>(AddGameObject(EntityManager::CreateEntity<T>()));
		}

		GameObject* AddGameObject(const HashedStringView& concrete_class_name);
		GameObject* FindGameObject(const std::string_view name) const;

		CollisionSystem2D* GetCollisionSystem2D() { return collision_system_2D_.get(); }

		void SetCollisionMask(uint32_t layer_a, uint32_t layer_b, bool can_collide);

		const std::array<std::bitset<kMaxLayers>, kMaxLayers>& GetCollisionMask() const { return collision_mask_; }

		const RenderViewDesc& GetRenderViewDesc() const { return render_view_desc_; }
		
		void SetMainCamera(weak_handle_ptr<Camera> camera) { render_view_desc_.camera = camera; }

	protected:
		virtual void Init() = 0;
		virtual void OnFrameStart() {};
		virtual void OnActivate() {};
		virtual void OnShutdown() {};

		void SetRenderViewDesc(const RenderViewDesc& desc) { render_view_desc_ = desc; }
		void SetMainRenderTarget(std::shared_ptr<RenderTargetGroup> rtg) { render_view_desc_.rtg = rtg; }
		void SetMainViewClearSettings(const ClearSettings& clear_settings) { render_view_desc_.clear_settings = clear_settings; }
		void SetRenderPassOrder(std::vector<render_pass::ID> order) { render_view_desc_.pass_order = std::move(order); }

	private:
		void FrameStart();
		void FixedUpdate();
		void Update();

		void LateUpdate();
		void FrameEnd();
		void Shutdown();

		GameObject* AddGameObject(std::unique_ptr<GameObject> obj);
		void FlushPendingAdd();

		void DrainDestroyed();

	private:
		std::vector<std::unique_ptr<GameObject>> game_objects_ = {};

		std::unique_ptr<CollisionSystem2D> collision_system_2D_;

		std::array<std::bitset<kMaxLayers>, kMaxLayers> collision_mask_ = {};

		std::vector<std::unique_ptr<SceneEntity>> graveyard_[2] = {};
		bool graveyard_toggle_ = false;

		RenderViewDesc render_view_desc_ = {};
	};
}