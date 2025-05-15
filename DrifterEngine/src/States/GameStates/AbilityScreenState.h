#pragma once
#include <States/State.h>

namespace drft
{
	class AbilityScreenState : public State
	{
	public:
		AbilityScreenState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
	};
}