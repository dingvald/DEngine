#pragma once
#include "State.h"
#include "GUI/GUIElement.h"
#include "Utility/SpriteBatch.h"

namespace drft
{
	class WorldMapState : public State
	{
	public:
		WorldMapState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;
		void onPush() override;
		void onPop() override;

	private:
		sf::Vector2i _currentPosition;
		gui::Panel _mapBackground;
		sf::RectangleShape _currentPositionTile;
		SpriteBatch _map;
	};
}


