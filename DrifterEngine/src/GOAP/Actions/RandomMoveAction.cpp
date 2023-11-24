#include "pch.h"
#include "RandomMoveAction.h"
#include "Spatial/WorldGrid.h"
#include "Components/Components.h"
#include "Random/RandomNumberGenerator.h"

drft::goap::RandomMoveAction::RandomMoveAction()
{
	addEffect("look_busy", true);
}

drft::goap::ActionResult drft::goap::RandomMoveAction::perform(entt::handle agent) const
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

	agent.emplace<component::action::Move>(sf::Vector2i(randx, randy));
    return ActionResult::Complete;
}

int drft::goap::RandomMoveAction::cost() const
{
    return 1;
}

bool drft::goap::RandomMoveAction::requiresInRange() const
{
    return false;
}

bool drft::goap::RandomMoveAction::isInRange(entt::handle agent) const
{
    return false;
}
