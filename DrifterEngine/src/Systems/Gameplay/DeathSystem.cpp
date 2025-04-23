#include "pch.h"
#include "DeathSystem.h"
#include "Factory/EntityFactory.h"

#include "Components/Components.h"
#include "Components/DescriptionComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/PositionComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/ItemComponent.h"

#include "Events/RequestStateChange.h"
#include <States/StateIdentifiers.h>
#include "Spatial/Conversions.h"
#include <Spatial/Helpers.h>
#include "Systems/Helpers/FindItemOwner.h"
#include "Random/PercentChance.h"
#include "Utility/EntityHelpers.h"
#include "Events/SendFloatingMessageEvent.h"


void drft::system::DeathSystem::update()
{
	const auto& factory = _registry.ctx().get<EntityFactory&>();
	auto view = _registry.view<component::action::Die, PositionComponent>();
	for (auto [entity, pos] : view.each())
	{
		int chance = 80;
		if (auto materialComp = _registry.try_get<MaterialComponent>(entity))
		{
			for (auto& matName : materialComp->materials)
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
						dropped.patch<MaterialComponent>([&materialComp](MaterialComponent& mat)
							{
								mat.weight = materialComp->weight;
							});
						dropped.patch<DescriptionComponent>([entityName, matName](DescriptionComponent& desc)
							{
								desc.name = entityName + "'s " + matName;
							});
					}
				}
				chance *= 0.5;
			}
		}
		
		if (_registry.any_of<PlayerInputComponent>(entity))
		{
			std::filesystem::remove_all(".\\data\\savegame\\");
			_dispatcher.trigger(events::RequestStateStackPush(States::GameOver));
		}
		_registry.destroy(entity);
	}
}
