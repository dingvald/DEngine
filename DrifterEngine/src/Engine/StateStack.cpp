#include "pch.h"
#include "StateStack.h"

using namespace drft;

StateStack::StateStack(State::Context context)
	: _context(context)
{}

void StateStack::update(const float dt)
{
	for (auto state = _stack.rbegin(); state != _stack.rend(); ++state)
	{
		if (!(*state)->update(dt)) return;
	}

	applyPendingChanges();
}

void StateStack::render(sf::RenderTarget& target)
{
	for (auto& state : _stack)
	{
		state->render(target);
	}

	applyPendingChanges();
}

void StateStack::handleEvent(const sf::Event& event)
{
	for (auto state = _stack.rbegin(); state != _stack.rend(); ++state)
	{
		if (!(*state)->handleEvent(event)) return;
	}

	applyPendingChanges();
}

void StateStack::pushState(States stateID)
{
	_pendingList.push_back({ Action::Push, stateID });
}

void StateStack::popState()
{
	_pendingList.push_back({ Action::Pop, States::None });
}

void StateStack::clearStates()
{
	_pendingList.push_back({ Action::Clear, States::None });
}

bool StateStack::isEmpty() const
{
	return _stack.empty();
}

State::Ptr StateStack::createState(States stateID)
{
	assert(_factories.contains(stateID));
	return _factories.at(stateID)();
}

void StateStack::applyPendingChanges()
{
	if (_pendingList.empty()) return;

	for (auto [action, stateID] : _pendingList)
	{
		switch (action)
		{
			case Push:
				_stack.push_back(createState(stateID));
				break;

			case Pop:
				_stack.pop_back();
				break;

			case Clear:
				_stack.clear();
				break;
		}
	}

	_pendingList.clear();
}
