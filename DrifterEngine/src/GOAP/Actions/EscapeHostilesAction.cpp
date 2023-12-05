#include "pch.h"
#include "EscapeHostilesAction.h"
#include "Components/Components.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Helpers.h"
#include "Random/RandomNumberGenerator.h"
#include "GOAP/Sensors/Utility/IsHostile.h"
#include "GOAP/Sensors/Utility/IsEntityInSurroundings.h"

drft::goap::EscapeHostilesAction::EscapeHostilesAction()
{
	addPrecondition(spotted_hostile, true);

	addEffect(escape_hostile, true);
}

std::optional<sf::Vector2i> drft::goap::EscapeHostilesAction::trySetTarget(entt::handle agent) const
{
	auto& ai = getAI(agent);
	std::optional<sf::Vector2i> result = {};
	std::optional<sf::Vector2i> closestHostile = {};
	auto& myPos = agent.get<component::Position>().position;

	for (auto&& [entity, _] : ai.surroundings.at(SensorType::Visual))
	{
		auto otherHandle = entt::const_handle{ *agent.registry(), entity };
		if (!otherHandle.all_of<component::Position>()) continue;
		if (system::FactionSystem::resolveRelationship(agent, otherHandle) != system::Relationship::Hostile) continue;

		
		auto& otherPos = otherHandle.get<component::Position>();
		if (!closestHostile.has_value())
		{
			closestHostile = otherPos.position;
		}
		else
		{
			const int currentTargetDistance = spatial::distance(myPos, closestHostile.value());
			const int newTargetDistance = spatial::distance(myPos, otherPos.position);
			if (newTargetDistance < currentTargetDistance)
			{
				closestHostile = otherPos.position;
			}
		}
	}
	if (closestHostile.has_value())
	{
		int dx = myPos.x - closestHostile.value().x;
		int dy = myPos.y - closestHostile.value().y;
		result = myPos + sf::Vector2i{dx, dy};
	}
	else
	{
		int randx = rng::RandomNumberGenerator::intInRange(-1, 1);
		int randy = rng::RandomNumberGenerator::intInRange(-1, 1);
		const auto& grid = agent.registry()->ctx().get<const spatial::WorldGrid&>();
		const auto& tilepos = agent.get<component::Position>().position;

		auto blockerFilter = [&agent](entt::entity entity) -> bool
		{
			if (auto physical = agent.registry()->try_get<component::Physical>(entity))
			{
				return physical->blocks;
			}
			return false;
		};

		auto blockers = grid.entitiesAt(tilepos + sf::Vector2i(randx, randy), blockerFilter);

		int safetyCount = 0; // in case entity is surrounded
		while (safetyCount < 8 && !blockers.empty())
		{
			randx = rng::RandomNumberGenerator::intInRange(-1, 1);
			randy = rng::RandomNumberGenerator::intInRange(-1, 1);
			blockers = grid.entitiesAt(tilepos + sf::Vector2i(randx, randy), blockerFilter);
			++safetyCount;
		}
		result = myPos + sf::Vector2i{ randx, randy };
	}

	return result;
}

drft::goap::ActionResult drft::goap::EscapeHostilesAction::perform(entt::handle agent) const
{
	getAI(agent).blackboard.merge(effects());
	return ActionResult::Complete;
}

int drft::goap::EscapeHostilesAction::cost() const
{
	return 1;
}

bool drft::goap::EscapeHostilesAction::isInRange(entt::handle agent) const
{
	if (isEntityInSurroundings(agent, { SensorType::Visual, SensorType::Auditory }, filter::isHostile)) return true;
	return true;
}