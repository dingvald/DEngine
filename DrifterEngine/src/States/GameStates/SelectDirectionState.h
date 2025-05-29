#pragma once
#include <States/State.h>

class TargetingParameters;

namespace drft
{
	class SelectDirectionState : public State
	{
	public:
		SelectDirectionState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;

		void onPush() override;
		void onPop() override;

	private:
		void placeTargetingTiles();

		void selectDirection(sf::Vector2i direction);

	private:
		entt::handle _sessionEntity;
		std::vector<entt::entity> _targetingTileEntities;
	};
}


