#include "pch.h"
#include "ActionRegistry.h"

drft::goap::ActionRegistry::ActionMap drft::goap::ActionRegistry::_actions = {};

void drft::goap::ActionRegistry::bind()
{


}

const drft::goap::IAction& drft::goap::ActionRegistry::get(AiAction actionName)
{
	if (!_actions.contains(actionName)) throw std::exception("Action name does not exist.");
	return *_actions.at(actionName);
}
