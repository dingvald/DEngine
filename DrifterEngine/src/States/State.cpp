#include "pch.h"
#include "State.h"
#include "Engine/StateStack.h"

using namespace drft;

State::State(StateStack& stack, StateContext& context)
	: _stack(stack)
	, _context(context)
{
	_guiGroup = tgui::Group::create();
	context.gui.add(_guiGroup);
	_guiGroup->setFocused(true);
}

State::~State()
{
	getContext().gui.remove(_guiGroup);
}


bool State::handleEvent(const sf::Event& ev)
{
	return NotFullyProcessed{};
}

bool drft::State::update(const float)
{
	return true;
}

bool drft::State::fixedUpdate()
{
	return true;
}

void drft::State::onEnter()
{
	getContext().window.setMouseCursorVisible(true);
}

void drft::State::onExit()
{}

void drft::State::onPush()
{}

void drft::State::onPop()
{}

void State::requestStackPush(States stateId)
{
	_stack.pushState(stateId);
}

void State::requestStackPop()
{
	_stack.popState();
}

void State::requestStackClear()
{
	_stack.clearStates();
}

const StateContext& State::getContext() const
{
	return _context;
}

StateContext& drft::State::getContext()
{
	return _context;
}
