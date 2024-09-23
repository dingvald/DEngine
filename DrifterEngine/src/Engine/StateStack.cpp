#include "pch.h"
#include "StateStack.h"

using namespace drft;

StateStack::StateStack(StateContext context, tgui::Gui& gui)
	: _context(context)
	, _gui(gui)
{}

void StateStack::update(const float dt)
{
	for (auto state = _stack.rbegin(); state != _stack.rend(); ++state)
	{
		if (!state->ptr->update(dt)) break;
	}

	applyPendingChanges();
}

void drft::StateStack::fixedUpdate()
{
	for (auto state = _stack.rbegin(); state != _stack.rend(); ++state)
	{
		if (!state->ptr->fixedUpdate()) break;
	}

	applyPendingChanges();
}

void StateStack::render(sf::RenderTarget& target)
{
	for (auto& state : _stack)
	{
		state.ptr->render(target);
	}

	applyPendingChanges();
}

void StateStack::handleEvent(const sf::Event& event)
{
	for (auto state = _stack.rbegin(); state != _stack.rend(); ++state)
	{
		if (!state->ptr->handleEvent(event)) break;
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

State::StatePtr StateStack::createState(States stateID)
{
	assert(_factories.contains(stateID));

	auto guiGroup = createGuiGroup(stateID);
	guiGroup->setVisible(true);
	guiGroup->setFocused(true);

	return _factories.at(stateID)(guiGroup);
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
				State::StatePtr newState = createState(stateID);
				newState->onPush();
				_stack.emplace_back(stateID, std::move(newState));
			}
			break;
			case Pop:
			{
				_stack.back().ptr->onPop();
				disableGuiGroup(_stack.back().id);
				_stack.pop_back();
			}
			break;
			case Clear:
			{
				while (!_stack.empty())
				{
					_stack.back().ptr->onPop();
					disableGuiGroup(_stack.back().id);
					_stack.pop_back();
				}
			}
			break;
		}
	}

	_pendingList.clear();
}

tgui::Group::Ptr drft::StateStack::createGuiGroup(States stateID)
{
	std::string groupID = std::to_string(static_cast<unsigned int>(stateID));
	if (auto group = _gui.get<tgui::Group>(groupID))
	{
		_gui.remove(group);
	}

	auto group = tgui::Group::create();
	_gui.add(group, groupID);

	return group;
}

void drft::StateStack::disableGuiGroup(States stateID)
{
	std::string groupID = std::to_string(static_cast<unsigned int>(stateID));
	if (auto group = _gui.get<tgui::Group>(groupID))
	{
		group->setVisible(false);
	}
}
