#include "pch.h"
#include <Spatial/Helpers.h>
#include "MeleeAttackActionSystem.h"
#include "Spatial/Conversions.h"
#include "Spatial/WorldGrid.h"

#include <Components/CurrentActorComponent.h>
#include <Components/TweeningComponent.h>
#include "Components/Actions/MeleeAttackAction.h"
#include "Components/Components.h"
#include "Components/PositionComponent.h"

#include <Systems/Core/ActorSystem.h>
#include <Systems/Core/TweeningSystem.h>

#include <Components/RenderComponent.h>
#include <Systems/Helpers/EasingFunctions.h>
#include "Systems/Helpers/SpawnEffect.h"
#include "Utility/SpriteOptions.h"

using namespace entt::literals;

const std::unordered_map<std::string, entt::id_type> DamageTypeToEffectTexture
{
	{"slashing", "slash_effect"_hs},
	{"crushing", "impact_effect"_hs},
	{"piercing", "slash_effect"_hs},
};

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

	sf::Vector3i targetPosition = positionComponent.tile + spatial::vec3FromPlanar(action.direction);
	auto& targets = grid.entitiesAt(spatial::asTileSpace(targetPosition));
	for (auto target : targets)
	{
		_registry.emplace_or_replace<component::action::IncomingDamage>(target, action.damageTypes, entity);
	}

	if (auto positionComponent = _registry.try_get<PositionComponent>(entity))
	{
		sf::Color effectColor = sf::Color::White;
		entt::id_type effectTextureId = getEffectTexture(action.damageTypes);

		std::vector<SpriteOptions> frames =
		{
			SpriteOptions{.uvCoords = sf::Vector2i{0, 0}, .texture = effectTextureId, .uvSize = DefaultTileTextureSize, .layer = static_cast<unsigned int>(RenderLayer::EffectsFront), .color = effectColor},
			SpriteOptions{.uvCoords = sf::Vector2i{1, 0}},
		};

		const sf::Vector3i targetPosition = positionComponent->tile + spatial::vec3FromPlanar(action.direction);
		spawnEffect(_registry, {
			.frames = frames,
			.position = spatial::asTileSpace(targetPosition),
			.animationSpeed = 20.0f
			});
	}
}

void drft::system::MeleeAttackActionSystem::onTweenReturnedToStart(entt::handle entity) const
{
	ActorSystem::setActionComplete(entity, ActionCategory::Act);
}

entt::id_type drft::system::MeleeAttackActionSystem::getEffectTexture(const std::unordered_map<std::string, int>& damageTypes) const
{
	entt::id_type result = "impact_effect"_hs; // Default to the impact effect
	int maxDamage = 0;

	for (auto&& [type, damage] : damageTypes)
	{
		if (damage <= maxDamage) continue;
		if (!DamageTypeToEffectTexture.contains(type)) continue;

		maxDamage = damage;
		result = DamageTypeToEffectTexture.at(type);
	}

	return result;
}
