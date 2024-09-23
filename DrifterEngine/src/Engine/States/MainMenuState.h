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
		MainMenuState(StateStack& stack, StateContext& context, tgui::Group::Ptr gui);

		bool handleEvent(const sf::Event& ev) override;

	private:
		void onNewGame();
		void onContinue();
		void onSettings();
		void onExit();
		void onPressDown();
		void onPressUp();
		bool hasSaveFile() const;
	};
}


