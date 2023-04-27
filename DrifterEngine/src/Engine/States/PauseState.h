#pragma once
#include "State.h"
#include "GUI/GUIElement.h"

namespace drft
{
	class PauseState : public State
	{
	private:
		struct MenuOption
		{
			sf::Text text;
			std::function<void()> callback;
		};

	public:
		PauseState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev);
		bool update(const float dt);
		void render(sf::RenderTarget& target);

	private:
		gui::Panel _pauseBackground;
		gui::List _pauseWindow{ true };
	};
}


