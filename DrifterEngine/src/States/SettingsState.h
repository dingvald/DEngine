#pragma once
#include <States/State.h>

namespace drft
{
	class SettingsState : public State
	{
	public:
		SettingsState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev);

	private:
		void createSettingsListGroup();
	};
}