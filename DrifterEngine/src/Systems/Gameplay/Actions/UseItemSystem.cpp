#include "pch.h"
#include "UseItemSystem.h"
#include "Components/Components.h"
#include "Systems/Helpers/RemoveFromContainer.h"

void drft::system::UseItemSystem::init()
{
	registry->on_construct<component::action::Use>().connect<&UseItemSystem::onUseItem>(this);
	registry->on_update<component::action::Use>().connect<&UseItemSystem::onUseItem>(this);
}

void drft::system::UseItemSystem::onUpdateEnd()
{
	registry->clear<component::action::Use>();
}

void drft::system::UseItemSystem::onUseItem(entt::registry& registry, entt::entity entity)
{
	auto& useAction = registry.get<component::action::Use>(entity);
	auto& usableComp = registry.get<component::Usable>(useAction.entity);
	String2UseFunc.at(usableComp.action)(registry, entity, useAction.entity, usableComp.params);
	if (usableComp.consumes && useAction.item != component::Item::NONE)
	{
		removeFromContainer(registry, entity, useAction.entity, true);
	}
}
