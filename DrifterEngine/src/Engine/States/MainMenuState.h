#pragma once
#include <Engine/States/State.h>
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

	private:
		void onNewGameButton();
		void onContinueButton();
		void onSettingsButton();
		void onExitButton();

		bool hasSaveFile() const;
	};
}


