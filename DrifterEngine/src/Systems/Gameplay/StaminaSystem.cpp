#include "pch.h"
#include "StaminaSystem.h"
#include "Components/Components.h"
#include "Components/Actions/MoveAction.h"
#include "Components/StaminaComponent.h"

#include <Systems/Gameplay/SkillsSystem.h>
#include <Skills/SkillIds.h>

namespace Internal
{
	static float calculateMaxStaminaForLevel(int level)
	{
		float result = 10.f;
		result += (logf(level) / logf(1.3));
		return result;
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
	_registry.on_construct<StaminaComponent>().connect<&StaminaSystem::onStaminaAdded>(this);
	_registry.on_construct<component::action::ConsumeStamina>().connect<&StaminaSystem::onStaminaConsumed>(this);
	_registry.on_construct<MoveAction>().connect<&StaminaSystem::onMoveActionAdded>(this);
}

void drft::system::StaminaSystem::start()
{
	SkillsSystem::registerLevelUpHandler(SkillId::Endurance, Internal::onEnduranceLevelUp, _registry);
}

void drft::system::StaminaSystem::updateEnd()
{
	_registry.clear<component::action::ConsumeStamina>();
}

void drft::system::StaminaSystem::onStaminaAdded(entt::registry& registry, entt::entity entity) const
{
	auto& staminaComponent = _registry.get<StaminaComponent>(entity);
	if (staminaComponent.max == std::numeric_limits<float>::min())
	{
		int endurance = SkillsSystem::getSkillLevel(SkillId::Endurance, { registry, entity });
		staminaComponent.max = Internal::calculateMaxStaminaForLevel(endurance);
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
			const float skillPointsToAdd = totalConsumed * 15.f;
			SkillsSystem::useSkill(SkillId::Endurance, skillPointsToAdd, { registry, entity });
		}
		stamina->current = std::clamp(stamina->current - totalConsumed, 0.f, stamina->max);
	}
}

void drft::system::StaminaSystem::onMoveActionAdded(entt::registry& registry, entt::entity entity) const
{
	if (auto stamina = _registry.try_get<StaminaComponent>(entity))
	{
		_registry.emplace_or_replace<component::action::ConsumeStamina>(entity, -0.25f);
	}
}
