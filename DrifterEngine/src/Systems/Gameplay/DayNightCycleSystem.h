#pragma once
#include "Systems/System.h"
#include "Events/GameTickEvent.h"

namespace drft::system
{
	class DayNightCycleSystem : public System
	{
	public:
		void init() override;
		void onStart() override;
		void fixedUpdate() override;
		void save(cereal::JSONOutputArchive& oarchive) override;
		void load(cereal::JSONInputArchive& iarchive) override;

	private:
		void onGameTickEvent(const events::GameTickEvent& ev);
		sf::Color determineSunColor() const;

	private:
		int _seconds = 0;
		int _minutes = 0;
		int _hours = 20;
		int _days = 0;
	};
}


