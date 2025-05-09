#include "pch.h"
#include <Spatial/Helpers.h>
#include "MeleeAttackActionSystem.h"
#include "Spatial/Conversions.h"
#include "Spatial/WorldGrid.h"

#include <Components/CurrentActorComponent.h>
#include <Components/TweeningComponent.h>
#include "Components/Actions/MeleeAttackAction.h"
#include "Components/PositionComponent.h"
#include <Components/WeightComponent.h>
#include <Components/IncomingForceComponent.h>

#include <Skills/SkillIds.h>

#include <Systems/Core/ActorSystem.h>
#include <Systems/Core/TweeningSystem.h>

#include <Systems/Helpers/EasingFunctions.h>
#include <Systems/Gameplay/SkillsSystem.h>

#pragma optimize ("", off)

static const float C_CONSTANT = 0.5f;
static const float K_CONSTANT = 1.5f;
static constexpr float SKILL_POINT_MULIPLIER = 10.f;

using namespace entt::literals;

void drft::system::MeleeAttackActionSystem::update()
{
	auto view = _registry.view<MeleeAttackAction, CurrentActorComponent>();
	for (auto&& [entity, meleeAttackAction, currentActor] : view.each())
	{
		if (currentActor.state == CurrentActorState::InProgress) continue;

		Tween moveToTween = {
			.targetOffset = spatial::toFloatSpace(spatial::asTileSpace(meleeAttackAction.direction)) * 0.4f,
			.time = 0.12f,
			.easing = Easing::linear,
			.onFinish = [this, action = meleeAttackAction](entt::handle entity) {
				onTweenReachedTarget(entity, std::move(action));
			}
		};
		Tween moveBackTween = {
			.targetOffset = {0.f, 0.f, 0.f},
			.time = 0.22f,
			.easing = Easing::easeOutBack,
			.onFinish = [this](entt::handle entity) {
				onTweenReturnedToStart(entity);
			}
		};

		entt::handle handle = { _registry, entity };
		TweeningSystem::tween(handle, moveToTween);
		TweeningSystem::tween(handle, moveBackTween);

		ActorSystem::setActionInProgress(handle);
		handle.remove<MeleeAttackAction>();
	}
}

void drft::system::MeleeAttackActionSystem::onTweenReachedTarget(entt::handle entity, MeleeAttackAction action) const
{
	const auto& grid = _registry.ctx().get<spatial::WorldGrid&>();
	const auto& positionComponent = entity.get<PositionComponent>();

	const float force = calculateForceGenerated(entity, action.itemUsed);

	drft::TilePosition targetPosition = positionComponent.tile + spatial::asTileSpace(action.direction);
	auto& targets = grid.entitiesAt(targetPosition);
	for (auto&& target : targets)
	{
		_registry.emplace_or_replace<IncomingForceComponent>(target, force, action.itemUsed);
	}
}

void drft::system::MeleeAttackActionSystem::onTweenReturnedToStart(entt::handle entity) const
{
	ActorSystem::setActionComplete(entity, ActionCategory::Act);
}

float drft::system::MeleeAttackActionSystem::calculateForceGenerated(entt::handle actor, entt::const_handle item) const
{
	const float strength = static_cast<float>(SkillsSystem::getSkillLevel(SkillId::Strength, actor));
	const float agility = static_cast<float>(SkillsSystem::getSkillLevel(SkillId::Agility, actor));

	float weight = 0.5f;
	if (actor.entity() != item.entity())
	{
		auto weightComponent = item.try_get<WeightComponent>();
		if (weightComponent) weight = weightComponent->value;
	}

	float strengthContribution = 0.f;
	float agilityContribution = 0.f;

	const float strengthCap = sqrtf(strength);
	if (weight <= strengthCap) 
	{
		strengthContribution = strength * (weight / strengthCap);
	}
	else
	{
		strengthContribution = strength * strength / (weight * weight);
	}

	agilityContribution = agility / (weight + 1.0f);

	SkillsSystem::useSkill(SkillId::Strength, (int)(std::sqrtf(strengthContribution) * SKILL_POINT_MULIPLIER), actor);
	SkillsSystem::useSkill(SkillId::Agility, (int)(std::sqrtf(agilityContribution) * SKILL_POINT_MULIPLIER), actor);

	return (strengthContribution + agilityContribution);
}
