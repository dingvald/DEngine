#pragma once
#include "Systems/System.h"
#include "Events/GameTickEvent.h"

namespace drft::system
{
	class DayNightCycleSystem : public System
	{
	public:
		using System::System;

		void init() override;
		void onStart() override;
		void onFixedUpdate() override;

	private:
		void onGameTickEvent(const events::GameTickEvent& ev);
		void onConstructDateAndTimeTracker(entt::registry& registry, entt::entity entity);
		sf::Color determineGlobalIllumination() const;

	private:
		entt::entity _dateAndTimeTracker = entt::null;
	};
}


