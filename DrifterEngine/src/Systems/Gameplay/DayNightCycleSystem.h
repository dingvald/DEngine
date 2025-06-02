#pragma once
#include "Systems/System.h"
#include "Events/GameTickEvent.h"

namespace drft
{
	struct TilePosition;
}

namespace drft::system
{
	class DayNightCycleSystem : public System
	{
	public:
		using System::System;

		void init() override;
		void start() override;
		void update() override;

	private:
		void onGameTickEvent(const events::GameTickEvent& ev);
		void onConstructDateAndTimeTracker(entt::registry& registry, entt::entity entity);
		sf::Color determineGlobalIllumination(drft::TilePosition position) const;
		void setSurfaceLight();

	private:
		entt::entity _dateAndTimeTracker = entt::null;
	};
}


