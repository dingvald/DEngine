#include "pch.h"
#include "MeleeAttackActionSystem.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include <Spatial/Helpers.h>

#include "Components/Components.h"
#include "Components/Actions/MeleeAttackAction.h"
#include "Components/AttackerComponent.h"
#include "Components/PositionComponent.h"
#include "Components/ProjectileComponent.h"
#include <Components/TweeningComponent.h>

#include <Systems/Core/TweeningSystem.h>
#include <Systems/Core/ActorSystem.h>

#include "Components/Tags.h"
#include "Utility/EntityHelpers.h"
#include "Utility/SpriteOptions.h"
#include <Systems/Helpers/EasingFunctions.h>
#include "Systems/Helpers/SpawnEffect.h"

using namespace entt::literals;

const std::unordered_map<std::string, entt::id_type> DamageTypeToEffectTexture
{
	{"slashing", "slash_effect"_hs},
	{"crushing", "impact_effect"_hs},
	{"piercing", "slash_effect"_hs}
};

void drft::system::MeleeAttackActionSystem::init()
{
	_registry.on_construct<MeleeAttackAction>().connect<&MeleeAttackActionSystem::onMeleeAttackActionAdded>(this);
}

void drft::system::MeleeAttackActionSystem::update()
{
	auto view = _registry.view<MeleeAttackAction>();
	for (auto&& [entity, meleeAttackAction] : view.each())
	{
		Tween moveToTween = {
			.targetOffset = spatial::toFloatSpace(spatial::asTileSpace(meleeAttackAction.direction)) * 0.3f,
			.time = 4,
			.easing = Easing::easeOutBack,
			.onFinish = [this, entity, action = meleeAttackAction](entt::handle) {
				processMeleeAttackAction(entity, std::move(action));
			}
		};
		entt::handle handle = { _registry, entity };
		TweeningSystem::tween(handle, moveToTween);

		handle.remove<MeleeAttackAction>();
	}
}

void drft::system::MeleeAttackActionSystem::onMeleeAttackActionAdded(entt::registry& registry, entt::entity entity) const
{
	auto& meleeAttack = registry.get<MeleeAttackAction>(entity);
	if (auto attackerComponent = registry.try_get<AttackerComponent>(entity))
	{
		meleeAttack.damageTypes["crushing"] += attackerComponent->baseDamage;
	}
}

void drft::system::MeleeAttackActionSystem::processMeleeAttackAction(entt::entity entity, MeleeAttackAction action) const
{
	for (auto target : action.targets)
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

	ActorSystem::completeAction({ _registry, entity }, ActionCategory::Act);
}

entt::id_type drft::system::MeleeAttackActionSystem::getEffectTexture(const std::unordered_map<std::string, int>& damageTypes) const
{
	entt::id_type result = {};
	int maxDamage = 0;

	for (auto&& [type, damage] : damageTypes)
	{
		if (damage > maxDamage)
		{
			maxDamage = damage;
			result = DamageTypeToEffectTexture.at(type);
		}
	}

	return result;
}
