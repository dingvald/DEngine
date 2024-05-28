#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class OpenableSystem : public System
	{
	private:
		void init() override;

		void openInteraction(entt::entity actor, entt::entity subject) const;
		void closeInteraction(entt::entity actor, entt::entity subject) const;

		void toggleInteractionFunction(entt::entity openable) const;

		void onConstructOpenable(entt::registry& registry, entt::entity entity) const;
		void onDestroyOpenable(entt::registry& registry, entt::entity entity) const;
	};
}


