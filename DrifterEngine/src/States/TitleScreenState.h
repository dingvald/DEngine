#pragma once
#include <States/State.h>

namespace drft
{
	class TitleScreenState : public State
	{
	public:
		TitleScreenState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update() override;
	};
}


