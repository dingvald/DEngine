#include "pch.h"
#include "ActionRegistry.h"
#include "Actions/SpotHostileAction.h"
#include "Actions/RandomMoveAction.h"
#include "Actions/AttackHostileAction.h"
#include "Actions/EscapeHostilesAction.h"
#include "Actions/InvestigateHostileAction.h"

drft::goap::ActionRegistry::ActionMap drft::goap::ActionRegistry::_actions = {};

void drft::goap::ActionRegistry::bind()
{
	_actions.emplace(AiAction::RandomMove, std::make_unique<RandomMoveAction>());
	_actions.emplace(AiAction::SpotHostile, std::make_unique<SpotHostileAction>());
	_actions.emplace(AiAction::AttackHostile, std::make_unique<AttackHostileAction>());
	_actions.emplace(AiAction::EscapeHostiles, std::make_unique<EscapeHostilesAction>());
	_actions.emplace(AiAction::InvestigateHostile, std::make_unique<InvestigateHostileAction>());
}

const drft::goap::IAction& drft::goap::ActionRegistry::get(AiAction actionName)
{
	if (!_actions.contains(actionName)) throw std::exception("Action name does not exist.");
	return *_actions.at(actionName);
}
