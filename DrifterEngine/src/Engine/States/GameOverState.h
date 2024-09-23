#pragma once
#include "State.h"
#include "GUI/GUIElement.h"


namespace drft
{
	class GameOverState : public State
	{
	public:
		GameOverState(StateStack& stack, StateContext& context, tgui::Group::Ptr gui);

		bool handleEvent(const sf::Event& ev);
		bool update(const float dt);
		void render(sf::RenderTarget& target);

	private:
		gui::Panel _gameOverPanel;
	};
}


