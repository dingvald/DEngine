#include "pch.h"
#include "IAction.h"
#include "Spatial/Helpers.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Systems/Helpers/HasLineOfSight.h"

bool drft::goap::IAction::isInRange(sf::Vector2i agentPosition, sf::Vector2i targetPosition) const
{
	float distance = spatial::distance(agentPosition, targetPosition);
	if (_range.isValueWithinInclusive(static_cast<int>(distance))) return true;
	return false;
}

bool drft::goap::IAction::isValid(const WorldState& worldState) const
{
	return worldState.isSupersetOf(_preconditions);
}

const drft::goap::WorldState& drft::goap::IAction::preconditions() const
{
	return _preconditions;
}

const drft::goap::WorldState& drft::goap::IAction::effects() const
{
	return _effects;
}

void drft::goap::IAction::addPrecondition(const std::string& key, int val)
{
	_preconditions.add(key, val);
}

void drft::goap::IAction::addEffect(const std::string& key, int val)
{
	_preconditions.add(key, val);
}

void drft::goap::IAction::setRange(int min, int max)
{
	_range.min = min;
	_range.max = max;
}

entt::entity drft::goap::IAction::findTarget(entt::const_handle aiEntity, std::function<bool(entt::const_handle)> selector) const
{
	const auto& ai = aiEntity.get<component::AI>();
	const auto& pos = aiEntity.get<component::Position>();

	float closestRange = static_cast<float>(ai.sightRange);
	entt::entity closestTarget = entt::null;

	auto view = aiEntity.registry()->view<const component::Position, component::tag::Active>();
	for (auto [otherEntity, otherPos] : view.each())
	{
		const float distance = spatial::distance(pos.position, otherPos.position);
		if (distance < ai.sightRange && distance < closestRange)
		{
			entt::const_handle otherHandle = { *aiEntity.registry(), otherEntity};
			if (system::hasLineOfSight(pos.position, otherPos.position, *aiEntity.registry()) && selector(otherHandle))
			{
				closestRange = distance;
				closestTarget = otherEntity;
			}
		}
	}

	return closestTarget;
}

component::AI& drft::goap::IAction::getAI(entt::handle aiEntity) const
{
	if (auto ai = aiEntity.try_get<component::AI>())
	{
		return *ai;
	}
	throw std::exception("Entity does not have AI component.");
}
