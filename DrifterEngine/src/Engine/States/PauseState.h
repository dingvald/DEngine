#pragma once
#include <Engine/States/State.h>
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

	private:
		void onContinueButton();
		void onSettingsButton();
		void onExitButton();

	private:
		tgui::Group::Ptr _group;
	};
}


