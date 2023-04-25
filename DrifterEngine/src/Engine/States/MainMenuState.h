#pragma once
#include "State.h"
#include "GUI/GUIElement.h"

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
		MainMenuState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;

	private:
		bool hasSaveFile() const;

	private:
		gui::List _mainMenuWindow{ true };
	};
}


