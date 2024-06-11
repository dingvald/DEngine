#pragma once
#include "Systems/System.h"

struct SpriteOptions;
struct SpriteControllerComponent;

namespace drft::system
{
	class SpriteControllerSystem : public System
	{
		void init() override;
		void onUpdateEnd() override;

		void onSpriteControllerAdded(entt::registry& registry, entt::entity entity) const;
		void onSpriteChangeRequest(entt::registry& registry, entt::entity entity) const;
		void handleNewSpriteState(entt::handle handle, SpriteControllerComponent& controller, entt::id_type stateId) const;
		void joinWithRenderComponent(entt::handle entity, const SpriteOptions& options) const;
	};
}


