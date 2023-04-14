#pragma once
#include "State.h"

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
		PauseState(StateStack& stack, Context context);

		bool handleEvent(const sf::Event& ev);
		bool update(const float dt);
		void render(sf::RenderTarget& target);
	private:
		void addMenuOption(std::string&& name, std::function<void()> callback);

	private:
		std::vector<MenuOption> _menuOptions;
		int _cursorIndex = 0;
	};
}


