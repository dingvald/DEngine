#include "pch.h"
#include "EscapeHostilesAction.h"

#include "Components/Components.h"
#include "Components/AIComponent.h"
#include "Components/PositionComponent.h"
#include "Components/MaterialComponent.h"

#include "Systems/Gameplay/FactionSystem.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Helpers.h"
#include "Random/Random.h"
#include "GOAP/Sensors/Utility/IsHostile.h"
#include "GOAP/Sensors/Utility/IsEntityInSurroundings.h"

drft::goap::EscapeHostilesAction::EscapeHostilesAction()
{
	addPrecondition(spotted_hostile, true);

	addEffect(escape_hostile, true);
}

std::optional<drft::TilePosition> drft::goap::EscapeHostilesAction::trySetTarget(entt::handle agent) const
{
	auto& ai = getAI(agent);
	std::optional<TilePosition> result = {};
	std::optional<TilePosition> closestHostile = {};
	auto& myPos = agent.get<PositionComponent>().tile;

	for (auto&& [entity, _] : ai.surroundings.at(SensorType::Visual))
	{
		auto otherHandle = entt::const_handle{ *agent.registry(), entity };
		if (!otherHandle.all_of<PositionComponent>()) continue;
		if (system::FactionSystem::resolveRelationship(agent, otherHandle) != system::Relationship::Hostile) continue;

		
		auto& otherPos = otherHandle.get<PositionComponent>();
		if (!closestHostile.has_value())
		{
			closestHostile = otherPos.tile;
		}
		else
		{
			const int currentTargetDistance = spatial::distance3d(myPos, closestHostile.value());
			const int newTargetDistance = spatial::distance3d(myPos, otherPos.tile);
			if (newTargetDistance < currentTargetDistance)
			{
				closestHostile = otherPos.tile;
			}
		}
	}
	if (closestHostile.has_value())
	{
		int dx = myPos.x - closestHostile.value().x;
		int dy = myPos.y - closestHostile.value().y;
		result = myPos + TilePosition{dx, dy, 0};
	}
	else
	{
		static rng::Random random{ rng::GlobalSeed };

		int randx = random.intInRange(-1, 1);
		int randy = random.intInRange(-1, 1);
		const auto& grid = agent.registry()->ctx().get<const spatial::WorldGrid&>();
		const auto& tilepos = agent.get<PositionComponent>().tile;
		const TilePosition newRandomDirection = tilepos + TilePosition{ randx, randy, 0 };

		auto blockerFilter = [&agent](entt::entity entity) -> bool
		{
			if (auto physical = agent.registry()->try_get<MaterialComponent>(entity))
			{
				return physical->blocks;
			}
			return false;
		};

		auto blockers = grid.entitiesAt(newRandomDirection, blockerFilter);

		int safetyCount = 0; // in case entity is surrounded
		while (safetyCount < 8 && !blockers.empty())
		{
			randx = random.intInRange(-1, 1);
			randy = random.intInRange(-1, 1);
			blockers = grid.entitiesAt(tilepos + TilePosition{ randx, randy, 0 }, blockerFilter);
			++safetyCount;
		}
		result = myPos + TilePosition{ randx, randy, 0 };
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