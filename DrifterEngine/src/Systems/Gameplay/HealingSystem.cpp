#include "pch.h"
#include "HealingSystem.h"
#include "Components/Components.h"

void drft::system::HealingSystem::init()
{
	_registry->on_construct<component::action::ConsumeEntity>().connect<&HealingSystem::onConstructConsumeEntity>(this);
}

void drft::system::HealingSystem::onConstructConsumeEntity(entt::registry& registry, entt::entity entity) const
{
	auto& consumeEntity = registry.get<component::action::ConsumeEntity>(entity);
	if (auto healing = _registry->try_get<component::Healing>(consumeEntity.consumed))
	{
		component::action::TakeDamage damage = { .amount = -(healing->amount), .source = consumeEntity.consumed };
		_registry->emplace<component::action::TakeDamage>(entity, std::move(damage));
	}
}
