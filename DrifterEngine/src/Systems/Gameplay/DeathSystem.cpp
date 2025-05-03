#include "pch.h"
#include "DeathSystem.h"
#include "Factory/EntityFactory.h"

#include "Components/Components.h"
#include "Components/DescriptionComponent.h"
#include "Components/MaterialCompositionComponent.h"
#include <Components/BaseMaterialComponent.h>
#include <Components/WeightComponent.h>
#include "Components/PositionComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/ItemComponent.h"

#include "Events/RequestStateChange.h"
#include <States/StateIdentifiers.h>
#include "Random/PercentChance.h"

#pragma optimize("", off)

void drft::system::DeathSystem::update()
{
	const auto& factory = _registry.ctx().get<EntityFactory&>();
	auto view = _registry.view<component::action::Die, PositionComponent>();
	for (auto [entity, pos] : view.each())
	{
		const float chance = 80.f;
		if (auto materialComp = _registry.try_get<MaterialCompositionComponent>(entity))
		{
			for (auto&& [matName, percent] : materialComp->materials)
			{
				if (!rng::percentChance(chance * percent)) continue;

				auto dropped = factory.build(matName, _registry);
				dropped.emplace_or_replace<PositionComponent>(pos);
				if (dropped.all_of<BaseMaterialComponent>())
				{
					dropped.emplace_or_replace<WeightComponent>(1.0f);
				}
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
