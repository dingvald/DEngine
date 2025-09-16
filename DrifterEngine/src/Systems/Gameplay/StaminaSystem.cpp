#include "pch.h"
#include "StaminaSystem.h"
#include "Components/Components.h"
#include "Components/Actions/MoveAction.h"
#include "Components/StaminaComponent.h"
#include <Components/SkillsComponent.h>

#include <Systems/Gameplay/SkillsSystem.h>
#include <Skills/SkillIds.h>
#include <Utility/Math/Logarithmic.h>

namespace Internal
{
	// See Desmos graph: https://www.desmos.com/calculator/eksnemfbjo
	static const float a = 4.2f;
	static const float b = 0.1f;
	static const float c = 10.f;
	static const float d = 1.18f;
	static const float e = 4.7f;

	static float calculateMaxStaminaForLevel(int level)
	{
		return drft::math::log(level, a, b, c, d, e);
	}

	static void onEnduranceLevelUp(int level, entt::handle entity)
	{
		if (auto stamina = entity.try_get<StaminaComponent>())
		{
			stamina->max -= calculateMaxStaminaForLevel(level - 1);
			stamina->max += calculateMaxStaminaForLevel(level);
		}
	}
}

void drft::system::StaminaSystem::init()
{
	_registry.on_construct<StaminaComponent>().connect<&StaminaSystem::onStaminaComponentAdded>(this);
	_registry.on_construct<component::action::ConsumeStamina>().connect<&StaminaSystem::onStaminaConsumed>(this);
	_registry.on_construct<MoveAction>().connect<&StaminaSystem::onMoveActionAdded>(this);
	_observer.connect(_registry, entt::collector.group<StaminaComponent, SkillsComponent>());
}

void drft::system::StaminaSystem::start()
{
	SkillsSystem::registerLevelUpHandler(SkillId::Endurance, Internal::onEnduranceLevelUp, _registry);
	for (auto&& entity : _observer)
	{
		setupStaminaComponentOnStart({ _registry, entity });
	}
	_observer.clear();
}

void drft::system::StaminaSystem::updateEnd()
{
	_registry.clear<component::action::ConsumeStamina>();
}

void drft::system::StaminaSystem::setupStaminaComponentOnStart(entt::handle entity) const
{
	auto staminaComponent = entity.try_get<StaminaComponent>();
	if (!staminaComponent) return;

	if (staminaComponent->max == std::numeric_limits<float>::min())
	{
		int endurance = SkillsSystem::getSkillLevel(SkillId::Endurance, entity);
		staminaComponent->max = Internal::calculateMaxStaminaForLevel(endurance);
		staminaComponent->current = staminaComponent->max;
	}
}

void drft::system::StaminaSystem::onStaminaComponentAdded(entt::registry& registry, entt::entity entity) const
{
	auto& staminaComponent = registry.get<StaminaComponent>(entity);
	if (staminaComponent.current == std::numeric_limits<float>::min())
	{
		staminaComponent.current = staminaComponent.max;
	}
}

void drft::system::StaminaSystem::onStaminaConsumed(entt::registry& registry, entt::entity entity) const
{
	if (auto stamina = _registry.try_get<StaminaComponent>(entity))
	{
		auto& consumeStaminaAction = _registry.get<component::action::ConsumeStamina>(entity);
		const float totalConsumed = stamina->baseConsumption + consumeStaminaAction.amount;
		if (totalConsumed > 0.f)
		{
			const float skillPointsToAdd = totalConsumed * 10.f;
			SkillsSystem::useSkill(SkillId::Endurance, skillPointsToAdd, { registry, entity });
		}
		stamina->current = std::clamp(stamina->current - totalConsumed, 0.f, stamina->max);
	}
}

void drft::system::StaminaSystem::onMoveActionAdded(entt::registry& registry, entt::entity entity) const
{
	auto& moveAction = registry.get<MoveAction>(entity);
	if (moveAction.direction == sf::Vector2i{ 0,0 }) return;

	if (auto stamina = _registry.try_get<StaminaComponent>(entity))
	{
		_registry.emplace_or_replace<component::action::ConsumeStamina>(entity, -0.25f);
	}
}
