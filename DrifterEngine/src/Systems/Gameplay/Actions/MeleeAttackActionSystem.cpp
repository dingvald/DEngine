#include "pch.h"
#include "MeleeAttackActionSystem.h"
#include "Systems/Helpers/SpendActionPoints.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include <Spatial/Helpers.h>

#include "Components/Components.h"
#include "Components/Actions/MeleeAttackAction.h"
#include "Components/AttackerComponent.h"
#include "Components/PositionComponent.h"
#include "Components/ProjectileComponent.h"

#include "Components/Tags.h"
#include "Utility/EntityHelpers.h"
#include "Utility/SpriteOptions.h"
#include "Systems/Helpers/SpawnEffect.h"

using namespace entt::literals;

void drft::system::MeleeAttackActionSystem::init()
{
	_registry.on_construct<MeleeAttackAction>().connect<&MeleeAttackActionSystem::onMeleeAttackActionAdded>(this);
}

void drft::system::MeleeAttackActionSystem::onUpdateLate(const float dt)
{
	auto view = _registry.view<MeleeAttackAction>();
	for (auto&& [entity, meleeAttackAction] : view.each())
	{
		processMeleeAttackAction(entity, meleeAttackAction);
	}
}

void drft::system::MeleeAttackActionSystem::onUpdateEnd()
{
	_registry.clear<MeleeAttackAction>();
}

void drft::system::MeleeAttackActionSystem::onMeleeAttackActionAdded(entt::registry& registry, entt::entity entity) const
{
	auto& meleeAttack = registry.get<MeleeAttackAction>(entity);
	if (auto attackerComponent = registry.try_get<AttackerComponent>(entity))
	{
		meleeAttack.damageTypes["crushing"] += attackerComponent->baseDamage;
	}
}

void drft::system::MeleeAttackActionSystem::processMeleeAttackAction(entt::entity entity, MeleeAttackAction& action)
{
	for (auto target : action.targets)
	{
		_registry.emplace_or_replace<component::action::IncomingDamage>(target, action.damageTypes, entity);
	}

	if (auto positionComponent = _registry.try_get<PositionComponent>(entity))
	{
		sf::Color effectColor = sf::Color::White;

		std::vector<SpriteOptions> frames = // crushing
		{
			SpriteOptions{.uvCoords = sf::Vector2i{0, 0}, .texture = "impact_effect"_hs, .uvSize = DefaultTileTextureSize, .layer = static_cast<unsigned int>(RenderLayer::EffectsFront), .color = effectColor},
			SpriteOptions{.uvCoords = sf::Vector2i{1, 0}, .texture = "impact_effect"_hs, .uvSize = DefaultTileTextureSize, .layer = static_cast<unsigned int>(RenderLayer::EffectsFront), .color = effectColor},
		};

		if (action.damageTypes.contains("slashing") || action.damageTypes.contains("piercing"))
		{
			frames = // slashing
			{
				SpriteOptions{.uvCoords = sf::Vector2i{0, 0}, .texture = "slash_effect"_hs, .uvSize = DefaultTileTextureSize, .layer = static_cast<unsigned int>(RenderLayer::EffectsFront), .color = effectColor},
				SpriteOptions{.uvCoords = sf::Vector2i{1, 0}, .texture = "slash_effect"_hs, .uvSize = DefaultTileTextureSize, .layer = static_cast<unsigned int>(RenderLayer::EffectsFront), .color = effectColor},
			};
		}
		const sf::Vector3i targetPosition = positionComponent->tile + spatial::vec3FromPlanar(action.direction);
		spawnEffect(_registry, {
			.frames = frames,
			.position = spatial::asTileSpace(targetPosition),
			.animationSpeed = 20.0f
			});
	}

	spendActionPoints(BASE_ACTION_COST, ActionType::Act, { _registry, entity });
}