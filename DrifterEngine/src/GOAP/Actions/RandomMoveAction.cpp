#include "pch.h"
#include "RandomMoveAction.h"
#include "Spatial/WorldGrid.h"
#include "Components/Actions/MoveAction.h"
#include "Components/PositionComponent.h"
#include "Components/MaterialComponent.h"
#include "Random/Random.h"

drft::goap::RandomMoveAction::RandomMoveAction()
{
	addEffect(look_busy, true);
}

drft::goap::ActionResult drft::goap::RandomMoveAction::perform(entt::handle agent) const
{
	static rng::Random random{ rng::GlobalSeed };

	int randx = random.intInRange(-1, 1);
	int randy = random.intInRange(-1, 1);
	const auto& grid = agent.registry()->ctx().get<const spatial::WorldGrid&>();
	const auto& tilepos = agent.get<PositionComponent>().position;

	auto blockerFilter = [&agent](entt::entity entity) -> bool
	{
		if (auto material = agent.registry()->try_get<MaterialComponent>(entity))
		{
			return material->blocks;
		}
		return false;
	};

	auto blockers = grid.entitiesAt(tilepos + sf::Vector2i(randx, randy), blockerFilter);

	int safetyCount = 0; // in case entity is surrounded
	while (safetyCount < 8 && !blockers.empty())
	{
		randx = random.intInRange(-1, 1);
		randy = random.intInRange(-1, 1);
		blockers = grid.entitiesAt(tilepos + sf::Vector2i(randx, randy), blockerFilter);
		++safetyCount;
	}

	agent.emplace_or_replace<MoveAction>(sf::Vector2i(randx, randy));
    return ActionResult::Complete;
}

int drft::goap::RandomMoveAction::cost() const
{
    return 1;
}

bool drft::goap::RandomMoveAction::isInRange(entt::handle agent) const
{
	return true;
}

