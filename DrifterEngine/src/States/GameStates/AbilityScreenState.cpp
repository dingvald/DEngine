#include "pch.h"
#include "AbilityScreenState.h"

namespace drft
{
	AbilityScreenState::AbilityScreenState(StateStack& stack, StateContext& context)
		: State(stack, context)
	{
		// Initialize your ability screen here
	}

	bool AbilityScreenState::handleEvent(const sf::Event& ev)
	{
		// Handle input events for the ability screen
		return false;
	}
}