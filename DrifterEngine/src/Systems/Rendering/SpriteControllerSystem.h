#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class SpriteControllerSystem : public System
	{
		void init() override;
		void onUpdateEnd() override;

		void onSpriteControllerAdded(entt::registry& registry, entt::entity entity) const;
		void onSpriteChangeRequest(entt::registry& registry, entt::entity entity) const;
	};
}


