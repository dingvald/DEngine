#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class SprintingSystem : public System
	{
	public:
		using System::System;

		void init() override;
		void onFixedUpdate() override;
		void shutdown() override;

	private:
		void onSprintingAdded(entt::registry& registry, entt::entity);
		void onSprintingRemoved(entt::registry& registry, entt::entity);
		void applySprintBuff(entt::registry& registry, entt::entity entity);
		void removeSprintBuff(entt::registry& registry, entt::entity entity);
		void addSprintEffect(entt::registry& registry, entt::entity entity);
		void removeSprintEffect(entt::registry& registry, entt::entity entity);
		void animateSprintEffects() const;

	private:
		std::unordered_map<entt::entity, entt::entity> _sprintEffects;
	};
}


