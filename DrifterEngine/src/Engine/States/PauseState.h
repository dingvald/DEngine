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
		PauseState(StateStack& stack, StateContext& context, tgui::Group::Ptr gui);

		bool handleEvent(const sf::Event& ev);

	private:
		void onContinue();
		void onSettings();
		void onExit();
	};
}


