#include "pch.h"
#include "State.h"
#include "Engine/StateStack.h"

using namespace drft;

State::State(StateStack& stack, StateContext& context)
	: _stack(&stack)
	, _context(context)
{}

State::~State()
{}

bool State::handleEvent(const sf::Event& ev)
{
	return false;
}

void drft::State::onPush()
{}

void drft::State::onPop()
{}

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

const StateContext& State::getContext() const
{
	return _context;
}

StateContext& drft::State::getContext()
{
	return _context;
}
