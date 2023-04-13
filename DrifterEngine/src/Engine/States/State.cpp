#include "pch.h"
#include "State.h"
#include "Engine/StateStack.h"

using namespace drft;

State::State(StateStack& stack, Context context)
	: _stack(&stack)
	, _context(context)
{}

State::~State()
{}

bool State::handleEvent(const sf::Event& ev)
{
	return false;
}

void State::requestStackPush(States stateId)
{
	_stack->pushState(stateId);
}

void State::requestStackPop()
{
	_stack->popState();
}

void State::requestStackClear()
{
	_stack->clearStates();
}

State::Context State::getContext() const
{
	return _context;
}
