#include "pch.h"
#include "CraftItemSystem.h"
#include "Components/Components.h"
#include "Factory/EntityFactory.h"
#include "Systems/Helpers/ItemDatabase.h"
#include "Utility/EntityHelpers.h"

void drft::system::CraftItemSystem::init()
{
	_registry->on_construct<component::action::Craft>().connect<&CraftItemSystem::onCraftItem>(this);
	_registry->on_update<component::action::Craft>().connect<&CraftItemSystem::onCraftItem>(this);
}

void drft::system::CraftItemSystem::update(const float dt)
{
	auto equipView = _registry->view<component::action::Craft>();

	for (auto entity : equipView)
	{
		_registry->remove<component::action::Craft>(entity);
	}
}

void drft::system::CraftItemSystem::onCraftItem(entt::registry& registry, entt::entity entity)
{
	auto& container = registry.get<component::Container>(entity);
	auto& craft = registry.get<component::action::Craft>(entity);

	std::vector<component::Item::ID> toRemove;
	for (auto [matName, amount] : craft.recipe)
	{
		int count = 0;
		for (auto item : container.contents)
		{
			auto itemEntity = ItemDatabase::getEntityFromItemID(item);
			if (matName.compare(util::getEntityName({ registry, itemEntity })) == 0)
			{
				toRemove.push_back(item);
				registry.destroy(itemEntity);
				++count;
			}

			if (count == amount) break;
		}
	}

	registry.patch<component::Container>(entity,
		[&toRemove](component::Container& cont)
		{
			for (auto item : toRemove)
			{
				cont.contents.erase(std::remove(cont.contents.begin(), cont.contents.end(), item), cont.contents.end());
			}
		});

	const auto& factory = registry.ctx().get<const EntityFactory&>();

	auto newItem = factory.build(craft.itemName, registry);
	newItem.remove<component::Position>();
	auto& itemComp = newItem.get<component::Item>();

	registry.patch<component::Container>(entity,
		[&itemComp](component::Container& cont)
		{
			cont.contents.push_back(itemComp.id);
		});
}
