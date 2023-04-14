#pragma once
#include "State.h"

namespace drft
{
	class MainMenuState : public State
	{
	private:
		struct MenuOption
		{
			sf::Text text;
			std::function<void()> callback;
		};

	public:
		MainMenuState(StateStack& stack, Context context);

		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;

	private:
		void addMenuOption(std::string&& name, std::function<void()> callback);
		bool hasSaveFile() const;

	private:
		std::vector<MenuOption> _menuOptions;
		int _cursorIndex = 0;
	};
}


