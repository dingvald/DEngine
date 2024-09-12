#include "pch.h"
#include "DeathSystem.h"
#include "Factory/EntityFactory.h"

#include "Components/Components.h"
#include "Components/DescriptionComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/PositionComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/ItemComponent.h"

#include "Events/RequestStateChange.h"
#include "Engine/States/StateIdentifiers.h"
#include "Spatial/Conversions.h"
#include <Spatial/Helpers.h>
#include "Systems/Helpers/FindItemOwner.h"
#include "Random/PercentChance.h"
#include "Utility/EntityHelpers.h"
#include "Events/ItemBreakEvent.h"
#include "Events/SendFloatingMessageEvent.h"


void drft::system::DeathSystem::onUpdate(const float dt)
{
	const auto& factory = _registry.ctx().get<EntityFactory&>();
	auto view = _registry.view<component::action::Die, MaterialComponent, PositionComponent>();
	for (auto [entity, material, pos] : view.each())
	{
		int chance = 80;
		for (auto& matName : material.materials)
		{
			if (rng::percentChance(chance))
			{
				auto dropped = factory.build(matName, _registry);
				dropped.patch<PositionComponent>([&pos](PositionComponent& position)
					{
						position.tile = pos.tile;
					});
				if (matName.compare("Corpse") == 0)
				{
					auto entityName = util::getEntityName({ _registry, entity });
					dropped.patch<MaterialComponent>([&material](MaterialComponent& mat)
						{
							mat.weight = material.weight;
						});
					dropped.patch<DescriptionComponent>([entityName, matName](DescriptionComponent& desc)
						{
							desc.name = entityName + "'s " + matName;
						});
				}
			}
			chance *= 0.5;
		}
		if (_registry.any_of<PlayerComponent>(entity))
		{
			std::filesystem::remove_all(".\\data\\savegame\\");
			_dispatcher.trigger(events::RequestStateStackPush(States::GameOver));
		}
		_registry.destroy(entity);
	}

	// Equipped item breaking
	auto itemView = _registry.view<component::action::Die, ItemComponent>(entt::exclude<PositionComponent>);
	for (auto [entity, item] : itemView.each())
	{
		auto owner = findItemOwner(_registry, item.id, WhereToLook::Bodies);
		_dispatcher.trigger(events::ItemBreakEvent(item.id, owner));
		_dispatcher.trigger(events::SendFloatingMessageEvent{
			.message = util::getEntityName({_registry, entity}) + " broke!",
			.color = sf::Color::Yellow,
			.tracksEntity = owner,
			.position = spatial::toXY(spatial::toFloatSpace(_registry.get<PositionComponent>(owner).tile)),
			.velocity = {0,-0.25},
			.isScreenSpace = false,
			.ttl = 100
			});
		_registry.destroy(entity);
	}
}
