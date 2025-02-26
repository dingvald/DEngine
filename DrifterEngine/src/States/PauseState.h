#pragma once
#include <States/State.h>

namespace drft
{
	class PauseState : public State
	{
	public:
		PauseState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev);

	private:
		void onContinueButton();
		void onSettingsButton();
		void onExitButton();
	};
}


