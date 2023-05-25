#include "pch.h"
#include "UseItemSystem.h"
#include "Components/Components.h"

void drft::system::UseItemSystem::init()
{
	registry->on_construct<component::action::Use>().connect<&UseItemSystem::onUseItem>(this);
	registry->on_update<component::action::Use>().connect<&UseItemSystem::onUseItem>(this);
}

void drft::system::UseItemSystem::update(const float dt)
{
	auto useView = registry->view<component::action::Use>();
	for (auto entity : useView)
	{
		
	}
}

void drft::system::UseItemSystem::onUseItem(entt::registry& registry, entt::entity entity)
{
	auto& useAction = registry.get<component::action::Use>(entity);
	auto& usableComp = registry.get<component::Usable>(useAction.entity);
	String2UseFunc.at(usableComp.action)(registry, entity, usableComp.params);
	if (usableComp.consumes)
	{
		registry.patch<component::Container>(entity,
			[useAction](component::Container& cont)
			{
				cont.contents.erase(std::remove(cont.contents.begin(), cont.contents.end(), useAction.item), cont.contents.end());
			});
	}
	registry.remove<component::action::Use>(entity);
}
