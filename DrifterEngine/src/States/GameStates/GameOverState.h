#pragma once
#include <States/State.h>


namespace drft
{
	class GameOverState : public State
	{
	public:
		GameOverState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev);
	};
}


