#include "pch.h"
#include "StateStack.h"

using namespace drft;

StateStack::StateStack(StateContext& context)
	: _context(context)
{}

void StateStack::update()
{
	for (auto state = _stack.rbegin(); state != _stack.rend(); ++state)
	{
		if (!(*state)->update()) break;
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

void drft::StateStack::guiRender(sf::RenderTarget& target)
{
	for (auto& state : _stack)
	{
		state->guiRender(target);
	}
	applyPendingChanges();
}

bool StateStack::handleEvent(const sf::Event& event)
{
	bool handled = false;
	for (auto state = _stack.rbegin(); state != _stack.rend(); ++state)
	{
		handled = (*state)->handleEvent(event);
		if (handled) break;
	}
	applyPendingChanges();
	return handled;
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

void drft::StateStack::clearStatesNow()
{
	clearStates();
	applyPendingChanges();
}

bool StateStack::isEmpty() const
{
	return _stack.empty();
}

State::StatePtr StateStack::createState(States stateID)
{
	assert(_factories.contains(stateID));

	return _factories.at(stateID)();
}

void StateStack::applyPendingChanges()
{
	if (_pendingList.empty()) return;

	for (auto&& [action, stateID] : _pendingList)
	{
		switch (action)
		{
			case Push:
			{
				if (!_stack.empty())
				{
					_stack.back()->onExit();
				}
				State::StatePtr newState = createState(stateID);
				newState->onPush();
				newState->onEnter();
				_stack.emplace_back(std::move(newState));
			}
			break;
			case Pop:
			{
				_stack.back()->onExit();
				_stack.back()->onPop();
				_stack.pop_back();
				if (!_stack.empty())
				{
					_stack.back()->onEnter();
				}
			}
			break;
			case Clear:
			{
				while (!_stack.empty())
				{
					_stack.back()->onExit();
					_stack.back()->onPop();
					_stack.pop_back();
				}
			}
			break;
		}
	}

	_pendingList.clear();
}