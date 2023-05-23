#pragma once
#include "Systems/System.h"
#include "Events/GameTickEvent.h"

namespace drft::spatial
{
	class WorldGrid;
}

namespace drft::system
{
	class LiquidSystem : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;
		void onFixedUpdateEnd() override;

	private:
		void addInLiquidEffect(sf::Vector2f position, sf::Color color);

	private:
		spatial::WorldGrid* _grid;
		std::vector<entt::entity> _inLiquidEffects;
	};
}


