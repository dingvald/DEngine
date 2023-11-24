#include "pch.h"
#include "AttackHostileAction.h"
#include "Components/Components.h"
#include "Spatial/Helpers.h"

drft::goap::AttackHostileAction::AttackHostileAction()
{
	addPrecondition("sees_hostile", true);
	addPrecondition("near_hostile", true);

	addEffect("sees_hostile", false);
	addEffect("target_dead", true);
}

std::optional<sf::Vector2i> drft::goap::AttackHostileAction::setMoveTarget(entt::const_handle agent) const
{
	const auto& ai = getAI(agent);
	if (ai.target == entt::null) return std::nullopt;
	if (auto posComp = agent.registry()->try_get<component::Position>(ai.target))
	{
		return posComp->position;
	}

	return std::nullopt;
}

drft::goap::ActionResult drft::goap::AttackHostileAction::perform(entt::handle agent) const
{
	const auto& ai = getAI(agent);
	if (ai.target == entt::null) return ActionResult::Failed;
	if (auto targetPos = agent.registry()->try_get<component::Position>(ai.target))
	{
		const auto& pos = agent.get<component::Position>();
		sf::Vector2i targetDirection = targetPos->position - pos.position;
		agent.emplace_or_replace<component::action::LaunchAttack>(targetDirection);
		return ActionResult::Continue;
	}
	return ActionResult::Failed;
}

int drft::goap::AttackHostileAction::cost() const
{
	return 2;
}

bool drft::goap::AttackHostileAction::requiresInRange() const
{
	return true;
}

bool drft::goap::AttackHostileAction::isInRange(entt::handle agent) const
{
	const auto& ai = getAI(agent);
	if (ai.target == entt::null) return false;
	if (auto targetPos = agent.registry()->try_get<component::Position>(ai.target))
	{
		const auto& myPos = agent.get<component::Position>();
		return spatial::distance(myPos.position, targetPos->position) <= 1;
	}
	return false;
}
