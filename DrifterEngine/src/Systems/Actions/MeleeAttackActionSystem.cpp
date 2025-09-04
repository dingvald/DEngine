#include "pch.h"
#include <Spatial/Helpers.h>
#include "MeleeAttackActionSystem.h"
#include "Spatial/Conversions.h"
#include "Spatial/WorldGrid.h"

#include <Components/CurrentActorComponent.h>
#include <Components/TweeningComponent.h>
#include "Components/Actions/MeleeAttackAction.h"
#include "Components/PositionComponent.h"
#include <Components/HealthComponent.h>
#include <Components/IncomingForceComponent.h>

#include <Skills/SkillIds.h>

#include <Systems/Core/ActorSystem.h>
#include <Systems/Core/TweeningSystem.h>

#include <Systems/Helpers/EasingFunctions.h>
#include <Systems/Helpers/CalculateForceGenerated.h>
#include <Systems/Gameplay/SkillsSystem.h>


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
			.targetOffset = spatial::toFloatSpace(spatial::asTileSpace(meleeAttackAction.direction)) * 0.75f,
			.time = 0.08f,
			.easing = Easing::linear,
			.onFinish = [this, action = meleeAttackAction](entt::handle entity) {
				onTweenReachedTarget(entity, std::move(action));
			}
		};
		Tween moveBackTween = {
			.targetOffset = {0.f, 0.f, 0.f},
			.time = 0.16f,
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

	drft::TilePosition targetPosition = positionComponent.tile + spatial::asTileSpace(action.direction);
	auto hasHealthFilter = [this](entt::entity entity) -> bool { return _registry.all_of<HealthComponent>(entity); };
	auto targets = grid.entitiesAt(targetPosition, hasHealthFilter);

	float force = 0.f;
	if (!targets.empty())
	{
		GeneratedForce generatedForce = calculateForceGenerated(entity, action.itemUsed);
		SkillsSystem::useSkill(SkillId::Strength, (int)(std::sqrtf(generatedForce.fromStrength) * SKILL_POINT_MULIPLIER), entity);
		SkillsSystem::useSkill(SkillId::Agility, (int)(std::sqrtf(generatedForce.fromAgility) * SKILL_POINT_MULIPLIER), entity);
		force = generatedForce.total;
	}
	
	entt::const_handle forceSource = action.itemUsed ? action.itemUsed : entt::const_handle{ entity };
	for (auto&& target : targets)
	{
		_registry.emplace_or_replace<IncomingForceComponent>(target, force, forceSource);
	}
}

void drft::system::MeleeAttackActionSystem::onTweenReturnedToStart(entt::handle entity) const
{
	ActorSystem::setActionComplete(entity, ActionCategory::Act);
}
