#include "pch.h"
#include "IAction.h"
#include "Spatial/Helpers.h"
#include "Components/Components.h"
#include "Components/AIComponent.h"
#include "Components/Tags.h"
#include "Systems/Helpers/HasLineOfSight.h"

std::optional<drft::TilePosition> drft::goap::IAction::trySetTarget(entt::handle agent) const
{
	if (isInRange(agent)) return std::nullopt;
	throw std::exception("Need to override.");
}

bool drft::goap::IAction::isValid(const WorldState& worldState) const
{
	return worldState.contains(_preconditions);
}

const drft::goap::WorldState& drft::goap::IAction::preconditions() const
{
	return _preconditions;
}

const drft::goap::WorldState& drft::goap::IAction::effects() const
{
	return _effects;
}

void drft::goap::IAction::addPrecondition(WorldStateType state, int val)
{
	_preconditions.add(state, val);
}

void drft::goap::IAction::addEffect(WorldStateType state, int val)
{
	_effects.add(state, val);
}

const AIComponent& drft::goap::IAction::getAI(entt::const_handle aiEntity) const
{
	if (auto ai = aiEntity.try_get<AIComponent>())
	{
		return *ai;
	}
	throw std::exception("Entity does not have AI component.");
}

AIComponent& drft::goap::IAction::getAI(entt::handle aiEntity) const
{
	if (auto ai = aiEntity.try_get<AIComponent>())
	{
		return *ai;
	}
	throw std::exception("Entity does not have AI component.");
}


